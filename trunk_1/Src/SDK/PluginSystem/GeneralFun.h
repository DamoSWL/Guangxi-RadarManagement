
#ifndef __GENERALFUN_
#define __GENERALFUN_

#include <time.h>
#include <wx/datetime.h>

class GeneralFun
{
public:
	static time_t GetTicks( wxDateTime &datetime)
	{
		struct tm tmptm;
		memset(&tmptm,0,sizeof(tmptm));
		tmptm.tm_year = datetime.GetYear()-1900;
		tmptm.tm_mon = datetime.GetMonth();
		tmptm.tm_mday = datetime.GetDay();
		tmptm.tm_hour = datetime.GetHour();
		tmptm.tm_min = datetime.GetMinute();
		tmptm.tm_sec = datetime.GetSecond();

		return mktime(&tmptm);
	}	
	//************************************
	// Method:    GetTicks
	// FullName:  GeneralFun::GetTicks
	// Access:    public static 
	// Returns:   time_t
	// Qualifier:
	// Parameter: long year 0~3000
	// Parameter: long month 1~12
	// Parameter: long day	1~31
	// Parameter: long hour	0~23
	// Parameter: long min 0~59
	// Parameter: long sec 0~59
	//************************************
	static time_t GetTicks( long year,long month,long day,long hour,long min,long sec)
	{
		struct tm tmptm;
		memset(&tmptm,0,sizeof(tmptm));
		tmptm.tm_year = year-1900;
		tmptm.tm_mon = month-1;
		tmptm.tm_mday = day;
		tmptm.tm_hour = hour;
		tmptm.tm_min = min;
		tmptm.tm_sec = sec;

		return mktime(&tmptm);
	}
};

#endif

