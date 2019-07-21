#pragma once

#include <map>


class CProductCheck
{
public:
	CProductCheck(void);
	~CProductCheck(void);

public:
	static DWORD WINAPI fileMonitor(LPVOID para);
	void getLatestFileName(std::map<wxString,UINT>& filesInfo,UINT* newTime);
	void parseFile(std::map<wxString,UINT>& filesInfo,UINT newTime);
	void setUpdateTime(UINT time){m_updateTime = time;}

private:
	bool writeToDataBase(const std::map<wxString,UINT>& filesInfo);

private:
	static const wxString FILESPEC;

	UINT m_updateTime;
	
};

