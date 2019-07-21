#include "SoftwareCheck.h"
#include "mysqldatabase.h"
#include <TLHELP32.H> 



static const UINT SOFTWARETIMERID = 130;

BEGIN_EVENT_TABLE(CSoftwareCheck,wxEvtHandler)
	EVT_TIMER(SOFTWARETIMERID,CSoftwareCheck::onTimeout)
END_EVENT_TABLE()

CSoftwareCheck::CSoftwareCheck(void):m_Timer(this,SOFTWARETIMERID)
{
}


CSoftwareCheck::~CSoftwareCheck(void)
{
}

void CSoftwareCheck::getSoftwareName()
{
	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);	

	if(pDataBase)
	{
		try
		{
			auto siteInfo = GetPlug()->getSiteInfo();
			auto computerType = GetPlug()->getComputerType();
			char sqlQuery[128] = {'\0'};
			sprintf_s(sqlQuery,128,"SELECT softwareName FROM radar_software_type WHERE radarcd = '%s' AND computerName = '%s'",siteInfo.c_str(),computerType.c_str());
			
			auto resultNum = pDataBase->ExecuteQuery(sqlQuery);
			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = pDataBase->GetStringValue(index,"softwareName",length);
					if(sqlTmpStr)
					{
						wxString softwareName(sqlTmpStr,wxConvLocal,std::strlen(sqlTmpStr));
						m_softwareNames.insert(softwareName);
					}
				}
			}

			pDataBase->FreeResult(sqlError);
			

		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}
	}
}

void CSoftwareCheck::startTimer()
{
	m_Timer.Start(10*60*1000);
}

void CSoftwareCheck::onTimeout(wxTimerEvent& event)
{
	getSoftwareName();
	std::map<wxString,UINT> programStatus;
	for(auto item = m_softwareNames.cbegin();item != m_softwareNames.cend(); item++)
	{
		programStatus[*item] = ABNORMAL;
	}

	PROCESSENTRY32 pe32;

	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"¿ìÕÕ¾ä±ú»ñÈ¡Ê§°Ü");
		return;
	}

	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		wxString name(pe32.szExeFile,std::wcslen(pe32.szExeFile));
		if(m_softwareNames.find(name) != m_softwareNames.cend())
		{
			DWORD errCode;
			HANDLE hProc;
			hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, pe32.th32ProcessID);

			::GetExitCodeProcess(hProc, &errCode);
			if (errCode == STILL_ACTIVE)
			{
				programStatus[name] = NORMAL;
			}
			else
			{
				programStatus[name] = ABNORMAL;
			}

		}
	

		bMore = ::Process32Next(hProcessSnap, &pe32);

	}
	::CloseHandle(hProcessSnap);
	writeToDataBase(programStatus);


}

bool CSoftwareCheck::writeToDataBase(const std::map<wxString,UINT>& status)
{
	wxString sqlSentence = wxT("INSERT INTO radar_software (updateTime, \
																	computerType, \
																	softwareName, \
																	run_status, \
																	radarcd) VALUES");

	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);	
	if (pDataBase)
	{
		try
		{
			auto computerType = GetPlug()->getComputerType();
			for(auto item = status.cbegin(); item !=  status.cend(); item++)
			{
				auto sqlConSentence = sqlSentence;
				sqlConSentence.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
				sqlConSentence.Append(wxString::Format(wxT("'%s', "),wxString(computerType.c_str(),wxConvLocal)));
				sqlConSentence.Append(wxString::Format(wxT("'%s', "), item->first));
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second));
				sqlConSentence.Append(wxString::Format(wxT("'%s')"), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));

				if(!pDataBase->ExecuteNoQuery(sqlConSentence,sqlError))
				{
					throw sqlError;
				}

			}

			return true;
		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}
	}
	return false;


}
