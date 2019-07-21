#pragma once



class CProductFileAlarm
{
public:
	CProductFileAlarm(void);
	~CProductFileAlarm(void);

public:
	void setFileTraits();
	void insertFileName(const wxString& fileName);
	void productFileCheck();

private:
	wxString getTimeStr(const wxString& fileName);
	UINT getTime(const wxString& TimeStr);
	bool writeToDataBase(const wxString& fileName,UINT time);

private:
	std::map<wxString, std::set<wxString> > m_productFileNames;
	std::set<wxString> m_productTraits;
	std::set<wxString> m_missingFiles;


};

