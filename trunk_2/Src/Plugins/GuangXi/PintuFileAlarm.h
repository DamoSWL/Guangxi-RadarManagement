#pragma once

class CPintuFileAlarm
{
public:
	CPintuFileAlarm(void);
	~CPintuFileAlarm(void);

public:
	void setFileTraits();
	void insertFileName(const wxString& fileName);
	void pintuFileCheck();

private:
	wxString getTimeStr(const wxString& fileName);
	bool writeToDataBase(const wxString& fileName);

private:
	std::map<wxString, std::set<wxString> > m_pintuFileNames;
	std::set<wxString> m_pintuTraits;
	std::set<wxString> m_missingFiles;
};

