#pragma once

#include <set>
#include <map>

class CSoftwareCheck : public wxEvtHandler
{
public:
	CSoftwareCheck(void);
	~CSoftwareCheck(void);

public:

	void startTimer();
	void stopTimer() {m_Timer.Stop();}
	
private:
	void getSoftwareName();
	void onTimeout(wxTimerEvent& event);
	bool writeToDataBase(const std::map<wxString,UINT>& status);


private:
	enum 
	{
		ABNORMAL = 0,
		NORMAL
	};

private:
	std::set<wxString> m_softwareNames;
	wxTimer m_Timer;
	

	DECLARE_EVENT_TABLE()


};

