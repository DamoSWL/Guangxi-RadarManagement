#include "FTPFileAlarm.h"
#include "mysqldatabase.h"

CFTPFileAlarm::CFTPFileAlarm(void)
{
}


CFTPFileAlarm::~CFTPFileAlarm(void)
{
}

bool CFTPFileAlarm::writeToDataBase(UINT time,const std::string& fileName,BOOL flag)
{
    if(flag == TRUE)
    {
        return true;
    }

	wxString sqlSentence = wxT("INSERT INTO alarm_info (time, \
                                                            endTime, \
                                                            type, \
                                                            alarmName, \
                                                            level, \
                                                            mode, \
                                                            content, \
                                                            origin, \
                                                            radarcd) VALUES"); 
    wxString sqlError;

    MySqlDatabase* pDataBase = GetPlug()->getDatabase();
    wxMutex* sqlLocker = GetPlug()->getSqlMutex();
    wxMutexLocker locker(*sqlLocker);

    if(pDataBase)
    {
        try
        {
            sqlSentence.Append(wxString::Format(wxT("(%d, "),time));
			sqlSentence.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
            sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("radar_run")));
			sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
			sqlSentence.Append(wxString::Format(wxT("'%d',"), 0));

            wxString content;
            content = wxString(fileName.c_str(),wxConvLocal);
			content.Append(wxT("FTPÉÏ´«Ê§°Ü"));

			sqlSentence.Append(wxString::Format(wxT("'%s',"),content));
			sqlSentence.Append(wxString::Format(wxT("%d,"), 2));
			sqlSentence.Append(wxString::Format(wxT("'%s')"),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));

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
