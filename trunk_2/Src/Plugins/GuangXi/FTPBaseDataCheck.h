#pragma once

#include <tuple>
#include "IFTPCheck.h"
#include "FTPFileAlarm.h"

class CFTPBaseDataCheck : public IFTPCheck
{ 
public:
	CFTPBaseDataCheck(void);
	~CFTPBaseDataCheck(void);

public:
	static DWORD WINAPI fileMonitor(LPVOID para);
	virtual wxString getLatestFileName(UINT* newTime,const wxString& fileSpec) override;
	virtual void getFileContent(const char* content) override;
	virtual void parseFile(UINT newTime) override;
	virtual void setUpdateTime(UINT time) {m_updateTime = time;}

public:
	void insertFileSpec(const wxString& fileSpec) {FILESPEC.insert(fileSpec);}
	const std::set<wxString>& getFileSpec() {return FILESPEC;}

private:
	INT getDateTime(const std::string& date,const std::string time);
	bool writeToDataBase(const std::tuple<UINT,std::string,BOOL>& baseDataInfo);

private:
	std::set<wxString> FILESPEC;

	UINT m_updateTime;
	UINT m_currentDay;
	std::string m_Contents;
	std::set<std::tuple<UINT,std::string,BOOL> > m_upLoadFileInfo;
	CFTPFileAlarm m_FTPFileAlarm;
	INT m_Lines;

};





