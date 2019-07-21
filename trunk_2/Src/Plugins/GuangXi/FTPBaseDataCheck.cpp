
#include "FTPBaseDataCheck.h"
#include <wx/dir.h>
#include <sstream>
#include <ctime>
#include "mysqldatabase.h"



INT CFTPBaseDataCheck::getDateTime(const std::string& date,const std::string time)
{
	struct tm ctime;

	if(date.empty() || time.empty())
	{
		return -1;
	}

	std::string::size_type pos = date.find_first_of('-');
	std::string yearStr = date.substr(0, pos);
	ctime.tm_year = std::stoi(yearStr) - 1900;

	pos++;
	std::string::size_type previousPos = pos;
	pos = date.find_first_of('-', previousPos);
	std::string monthStr = date.substr(previousPos, pos - previousPos);
	ctime.tm_mon = std::stoi(monthStr) - 1;

	pos++;
	previousPos = pos;
	std::string dayStr = date.substr(previousPos, date.length() - previousPos);
	ctime.tm_mday = std::stoi(dayStr);

	 pos = time.find_first_of(':');
	std::string hourStr = time.substr(0, pos);
	ctime.tm_hour = std::stoi(hourStr);

	pos++;
	previousPos = pos;
	pos = time.find_first_of(':', previousPos);
	std::string minStr = time.substr(previousPos, pos - previousPos);
	ctime.tm_min = std::stoi(minStr);

	pos++;
	previousPos = pos;
	std::string secStr = time.substr(previousPos, time.length() - previousPos);
	ctime.tm_sec = std::stoi(secStr);


	return static_cast<UINT>(std::mktime(&ctime));
}



bool CFTPBaseDataCheck::writeToDataBase(const std::tuple<UINT,std::string,BOOL>& baseDataInfo)
{
	wxString sqlSentence = wxT("INSERT INTO file_transfer (updateTime, \
													type, \
													file_name, \
													status, \
													route, \
													radarcd) VALUES");
	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);		
	
	if(pDataBase)
	{
		try
		{
			sqlSentence.Append(wxString::Format(wxT("(%d, "), std::get<0>(baseDataInfo)));
			sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
			std::string fileName = std::get<1>(baseDataInfo);
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxString(fileName.c_str(),wxConvLocal)));
			sqlSentence.Append(wxString::Format(wxT("%d, "), std::get<2>(baseDataInfo)));
			sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.Append(wxString::Format(wxT("'%s')"),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
	
			if(!pDataBase->ExecuteNoQuery(sqlSentence,sqlError))
			{
				throw sqlError;
			}

			return true;
		}
		catch(wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}

	}

	return false;
}

CFTPBaseDataCheck::CFTPBaseDataCheck(void):m_updateTime(0),m_currentDay(0),m_Lines(-1)
{
}


CFTPBaseDataCheck::~CFTPBaseDataCheck(void)
{
}



DWORD WINAPI CFTPBaseDataCheck::fileMonitor(LPVOID para)
{
	CFTPBaseDataCheck* self = static_cast<CFTPBaseDataCheck*>(para);
	if(self)
	{
		UINT newTime = 0;
		auto CriticalSection = GetPlug()->getBaseDataFTPCritical();
		auto fileSpec = self->getFileSpec();

		while(!GetPlug()->GetExit())
		{
			
			for(auto item = fileSpec.cbegin(); item != fileSpec.cend();item++)
			{
				EnterCriticalSection(CriticalSection);
				auto fileName = self->getLatestFileName(&newTime,*item);
				if(!fileName.IsEmpty())
				{
					GetPlug()->AddLog(LOG_TYPE_MESSAGE,fileName.mb_str(wxConvLocal));
					wxFile file(fileName);
					if(file.IsOpened())
					{
						auto Length = file.Length();
						char* content = new char[Length+1];
						std::memset(content,'\0',Length+1);
						file.Read(content,Length);
						self->getFileContent(content);
						delete[] content;
						self->parseFile(newTime);
					}
					file.Close();
				}
				LeaveCriticalSection(CriticalSection);
			}

			::wxSleep(1);			
		}
	}

	return TRUE;
}

wxString CFTPBaseDataCheck::getLatestFileName(UINT* newTime,const wxString& fileSpec)
{

	auto dirName = GetPlug()->getFTPBaseDataDir();

	if(dirName.IsEmpty())
	{
		return wxString();
	}

	 if(!::wxDirExists(dirName))
	 {
		 GetPlug()->AddLog(LOG_TYPE_ERROR,"基数据FTP目录不存在，请重新选择目录");
		 ::wxSleep(5);
		 return wxString();
	 }

	wxDir fileDir(dirName);
	wxString fullFilePath;

	if ( !fileDir.IsOpened() )
	{
		GetPlug()->AddLog(LOG_TYPE_ERROR,"FTP文件监控目录异常");
	
	}
	else
	{
		wxString fileName;
		bool newFileFlag = false;

		bool flag = fileDir.GetFirst(&fileName, fileSpec, wxDIR_FILES );
		while ( flag )
		{
			fullFilePath = dirName + wxT("\\") + fileName;
			auto updateTime = ::wxFileModificationTime(fullFilePath);
			if(m_updateTime < updateTime)
			{
				*newTime = updateTime;
				newFileFlag = true;

				if(m_currentDay != wxDateTime((time_t)*newTime).GetDay())
				{
					m_currentDay = wxDateTime((time_t)*newTime).GetDay();
					//m_upLoadFileInfo.clear();
					m_Lines = -1;
				}
				break;
			}

			flag = fileDir.GetNext(&fileName);
		}

		if(!newFileFlag)
		{
			fullFilePath.clear();
		}

	}


	

	return fullFilePath;
}

void CFTPBaseDataCheck::getFileContent(const char* content)
{
	if(content)
	{
		m_Contents.assign(content);
	}
	
}

void CFTPBaseDataCheck::parseFile(UINT newTime)
{
	if(m_Contents.empty())
	{
		return;
	}

	std::istringstream istrStream(m_Contents);
	std::string DateStr;
	std::string TimeStr;
	std::string operationStr;
	std::string fileStr;
	std::string fileNameStr;
	std::string resultStr;
	BOOL flag = FALSE;
	INT Lines = -1;

	while(istrStream >> DateStr >> TimeStr >> operationStr >> fileStr >> fileNameStr >> resultStr)
	{
		Lines++;
		if(Lines > m_Lines)
		{
			std::tuple<UINT,std::string,BOOL> singleFileInfo;
	
			auto time = getDateTime(DateStr,TimeStr);
			if(time > 0)
			{
				if(!fileNameStr.empty() && (fileNameStr.find("O_DOR") != std::string::npos))
				{
					if(resultStr == "successed!")
					{
						flag = TRUE;
						singleFileInfo = std::make_tuple(time,fileNameStr,TRUE);
					}
					else
					{
						flag = FALSE;
						singleFileInfo = std::make_tuple(time,fileNameStr,FALSE);
					}
				
					//if(m_upLoadFileInfo.find(singleFileInfo) == m_upLoadFileInfo.end())
	
					if(writeToDataBase(singleFileInfo) && m_FTPFileAlarm.writeToDataBase(time,fileNameStr,flag))
					{
						m_updateTime = newTime;
						//m_upLoadFileInfo.insert(singleFileInfo);
						m_Lines = Lines;
						GetPlug()->setFtpBaseDataTime(m_updateTime);
					}
				}
				
			}
		}
	}

}
