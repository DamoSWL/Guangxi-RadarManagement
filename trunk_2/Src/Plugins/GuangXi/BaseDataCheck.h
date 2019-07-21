#pragma once


class CBaseDataCheck
{
public:
	CBaseDataCheck(void);
	~CBaseDataCheck(void);

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

