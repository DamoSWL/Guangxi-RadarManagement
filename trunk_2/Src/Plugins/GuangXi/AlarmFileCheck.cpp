
#include "AlarmFileCheck.h"
#include <wx/dir.h>
#include <ctime>
#include "mysqldatabase.h"


CAlarmFileCheck::CAlarmFileCheck(void):m_updateTime(0)
{
}


CAlarmFileCheck::~CAlarmFileCheck(void)
{
}

void CAlarmFileCheck::fileMonitor()
{
	m_alarmCheckHandler = CreateThread(nullptr, 0, CAlarmFileCheck::alarmFileMonitor, this, 0, nullptr);
}

void CAlarmFileCheck::quit()
{
	if(WaitForSingleObject(m_alarmCheckHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_alarmCheckHandler);
	}
}

DWORD WINAPI CAlarmFileCheck::alarmFileMonitor(LPVOID para)
{
	CAlarmFileCheck* self = static_cast<CAlarmFileCheck*>(para);
	if(self)
	{
		UINT newTime = 0;
		std::map<UINT,wxString> alarmFileInfo;
		self->setUpdateTime(GetPlug()->getUpdateTime().alarmTime);

		while(!GetPlug()->GetExit())
		{
			self->getLatestFileName(alarmFileInfo,&newTime);
			if(!alarmFileInfo.empty())
			{
				for(auto item = alarmFileInfo.cbegin(); item != alarmFileInfo.cend(); item++)
				{
					auto fullPath = GetPlug()->getBaojingDir();
					fullPath += wxT("\\");
					fullPath += item->second;
					wxFile file(fullPath);
					if(file.IsOpened())
					{
						RADAR_ALARM_DATA alarmData;
						while(!file.Eof())
						{
							file.Read((void*)&alarmData,sizeof(RADAR_ALARM_DATA));
							self->getAlarmInfo(alarmData);
							GetPlug()->setAlarmFlag(alarmData.MONITOR_ALARM_FLAG);
						}

						file.Close();

						if(!self->parseAlarmInfo(newTime))
						{
							break;
						}
				
					}
				
				}

				alarmFileInfo.clear();
				
			}

			::wxSleep(3);
		}
	}

	return TRUE;
}

void CAlarmFileCheck::getLatestFileName(std::map<UINT,wxString>& fileInfo,UINT* newTime)
{
	auto dirName = GetPlug()->getBaojingDir();

	if(dirName.IsEmpty())
	{
		return;
	}
	wxDir fileDir(dirName);
	wxString fullFilePath;
	wxString fileName;

	if(!::wxDirExists(dirName))
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"报警文件监控目录不存在，请重新选择目录");
		return;
	}

	if ( !fileDir.IsOpened() )
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"报警文件监控目录异常");

	}
	else
	{

		bool flag = fileDir.GetFirst(&fileName, wxT("*.bin"), wxDIR_FILES );
		while ( flag )
		{
			fullFilePath = dirName + wxT("\\") + fileName;
			auto updateTime = ::wxFileModificationTime(fullFilePath);
			if(m_updateTime < updateTime)
			{
				*newTime = updateTime;
				fileInfo[updateTime] = fileName;
				if(fileInfo.size() >= 20)
				{
					break;
				}
			}

			flag = fileDir.GetNext(&fileName);
		}

	}

}

void CAlarmFileCheck::getAlarmInfo(const RADAR_ALARM_DATA& data)
{
	m_alarmInfo.push_back(data);
}

bool CAlarmFileCheck::parseAlarmInfo(UINT newTime)
{

	bool flag = false;
	for(auto item = m_alarmInfo.cbegin();item != m_alarmInfo.cend(); item++)
	{
		flag = writeToDataBase(*item);
		if(flag)
		{
			m_updateTime = newTime;
			GetPlug()->setAlarmTime(m_updateTime);
		}
		else
		{
			break;
		}

	}
	m_alarmInfo.clear();
	
	return flag;
}

UINT CAlarmFileCheck::getDateTime(const RADAR_ALARM_DATA& alarmInfo)
{
	if((std::strlen(alarmInfo.STATUS_DATA) == 0) || (std::strlen(alarmInfo.STATUS_TIME) == 0))
	{
		return -1;
	}
	struct tm ctime;
	
	char yearStr[8] = {'\0'};
	char monthStr[8] = {'\0'};
	char dayStr[8] = {'\0'};
	char hourStr[8] = {'\0'};
	char minStr[8] = {'\0'};
	char secStr[8] = {'\0'};

	std::memcpy(yearStr,alarmInfo.STATUS_DATA + 0, 4);
	std::memcpy(monthStr,alarmInfo.STATUS_DATA + 4, 2);
	std::memcpy(dayStr,alarmInfo.STATUS_DATA + 6, 2);
	std::memcpy(hourStr,alarmInfo.STATUS_TIME + 0, 2);
	std::memcpy(minStr,alarmInfo.STATUS_TIME + 2, 2);
	std::memcpy(secStr,alarmInfo.STATUS_TIME + 4, 2);

	ctime.tm_year = std::atoi(yearStr) - 1900;
	ctime.tm_mon = std::atoi(monthStr) - 1;
	ctime.tm_mday = std::atoi(dayStr);
	ctime.tm_hour = std::stoi(hourStr);
	ctime.tm_min = std::atoi(minStr);
	ctime.tm_sec = std::atoi(secStr);

	return static_cast<UINT>(std::mktime(&ctime));

}

bool CAlarmFileCheck::writeToDataBase(const RADAR_ALARM_DATA& alarmInfo)
{
	wxString sqlSentence;
	wxString sqlError;

	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);
	
	if (pDataBase)
	{
		try
		{
			UINT time = getDateTime(alarmInfo);
			if(time <= 0)
			{
				throw wxString(wxT("日期字符串为空"));
			}

			if(alarmInfo.MONITOR_ALARM_FLAG == ALARMON)
			{

				sqlSentence = wxT("INSERT INTO alarm_info (time, \
															endTime, \
															type, \
															alarmName, \
															level, \
															mode, \
															content, \
															origin, \
															radarcd) VALUES");

				sqlSentence.Append(wxString::Format(wxT("(%d, "),time));
				sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
				sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("radar_run")));
				sqlSentence.Append(wxString::Format(wxT("'%d', "), alarmInfo.ALARM_NO));

				sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
				sqlSentence.Append(wxString::Format(wxT("'%d',"), 0));
				sqlSentence.Append(wxString::Format(wxT("%s,"), wxT("NULL")));
				sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
				sqlSentence.Append(wxString::Format(wxT("'%s')"),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
			}
			else if(alarmInfo.MONITOR_ALARM_FLAG == ALARMOFF)
			{
				sqlSentence.Printf(wxT("UPDATE alarm_info SET endTime = %d WHERE endTime IS NULL AND alarmName = '%d' AND radarcd = '%s'"),time,
																																			alarmInfo.ALARM_NO,
																																			wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal));
			}
			else
			{
				return false;
			}

			if(!pDataBase->ExecuteNoQuery(sqlSentence,sqlError))
			{
				throw sqlError;
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
