
#include "Pintucheck.h"
#include <wx/dir.h>
#include <ctime>
#include "mysqldatabase.h"
#include "PintuFileAlarm.h"


const wxString CPintucheck::FILESPEC = wxT("*.*");

CPintucheck::CPintucheck(void):m_updateTime(0)
{
}


CPintucheck::~CPintucheck(void)
{
}



bool CPintucheck::writeToDataBase(const std::map<wxString,UINT>& filesInfo)
{
	wxString sqlSentence = wxT("INSERT INTO file_generate (updateTime, \
							   type, \
							   file_name, \
							   status, \
							   radarcd) VALUES");

	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);

	if(pDataBase)
	{
		try
		{

			for(auto item = filesInfo.cbegin(); item != filesInfo.cend(); item++)
			{
				auto sqlConSentence = sqlSentence;
				sqlConSentence.Append(wxString::Format(wxT("(%d, "),item->second));
				sqlConSentence.Append(wxString::Format(wxT("%d, "), 4));
				sqlConSentence.Append(wxString::Format(wxT("'%s', "), item->first));
				sqlConSentence.Append(wxString::Format(wxT("%d, "), 1));
				sqlConSentence.Append(wxString::Format(wxT("'%s')"),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
	
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


DWORD WINAPI CPintucheck::fileMonitor(LPVOID para)
{
	CPintucheck* self = static_cast<CPintucheck*>(para);
	if(self)
	{
		UINT newTime = 0;
		std::map<wxString,UINT> newFilesInfo;

		CPintuFileAlarm pintuFileAlarm;
		pintuFileAlarm.setFileTraits();

		while(!GetPlug()->GetExit())
		{
			
			self->getLatestFileName(newFilesInfo,&newTime);

			if(!newFilesInfo.empty())
			{
				for(auto item = newFilesInfo.cbegin(); item != newFilesInfo.cend(); item++)
				{
					pintuFileAlarm.insertFileName(item->first);
				}
				
				self->parseFile(newFilesInfo,newTime);

			}
			pintuFileAlarm.pintuFileCheck();
			::wxSleep(3);
		}
	}

	return TRUE;
}

void CPintucheck::getLatestFileName(std::map<wxString,UINT>& filesInfo, UINT* newTime)
{
	
	auto dirName = GetPlug()->getPintuDir();
	if(dirName.IsEmpty())
	{
		return;
	}
	wxDir fileDir(dirName);
	wxString fullFilePath;
	wxString fileName;

	if(!::wxDirExists(dirName))
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"拼图文件监控目录不存在，请重新选择目录");
		::wxSleep(5);
		return;
	}

	if ( !fileDir.IsOpened() )
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"拼图文件监控目录异常");

	}
	else
	{

		bool flag = fileDir.GetFirst(&fileName, CPintucheck::FILESPEC, wxDIR_FILES );
		while ( flag )
		{

			fullFilePath = dirName;
			fullFilePath += wxT("\\");
			fullFilePath += fileName;

			wxFileName fileAtrribute(fullFilePath);
			if(!fileAtrribute.IsOk())
			{
				GetPlug()->AddLog(LOG_TYPE_ERROR,fullFilePath.mb_str(wxConvLocal));
				break;
			}

			auto updateTime = ::wxFileModificationTime(fullFilePath);
			if(m_updateTime < updateTime)
			{
				filesInfo[fileName] = updateTime;
				*newTime = updateTime;
				GetPlug()->AddLog(LOG_TYPE_MESSAGE,fullFilePath.mb_str(wxConvLocal));

				if(filesInfo.size() >= 5)
				{
					break;
				}
			}

			flag = fileDir.GetNext(&fileName);
		}
	}

}

void CPintucheck::parseFile(std::map<wxString,UINT>& filesInfo,UINT newTime)
{

	if(writeToDataBase(filesInfo))
	{
		m_updateTime = newTime;
		filesInfo.clear();
		GetPlug()->setPintuTime(m_updateTime);
	}
}