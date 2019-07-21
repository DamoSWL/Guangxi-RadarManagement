#include "PintuFileAlarm.h"
#include <wx/dir.h>


CPintuFileAlarm::CPintuFileAlarm(void)
{
}


CPintuFileAlarm::~CPintuFileAlarm(void)
{
}

void CPintuFileAlarm::setFileTraits()
{
    if(GetPlug()->getComputerType().find("PUP") != std::string::npos)
    {
        m_pintuTraits.insert(wxT(".OP4"));  
        m_pintuTraits.insert(wxT(".VP4"));  
        m_pintuTraits.insert(wxT(".WP4"));  
        m_pintuTraits.insert(wxT(".XP4"));  
        m_pintuTraits.insert(wxT(".ZP4"));  
    }
}


void CPintuFileAlarm::insertFileName(const wxString& fileName)
{
	auto TimeStr = getTimeStr(fileName);
	if(TimeStr.IsEmpty())
	{
		return;
	}
	auto fileSpec = wxString::Format(wxT("%s*"),TimeStr);

	wxString pintuFileName;
	wxDir dir(GetPlug()->getPintuDir());
	auto flag = dir.GetFirst(&pintuFileName,fileSpec,wxDIR_FILES);
	
	while(flag)
	{
		if(m_pintuFileNames[TimeStr].find(pintuFileName) == m_pintuFileNames[TimeStr].end())
		{
			m_pintuFileNames[TimeStr].insert(pintuFileName);
		}
		flag = dir.GetNext(&pintuFileName);
	}
    
}

void CPintuFileAlarm::pintuFileCheck()
{
    if(m_pintuFileNames.size() < 2)
    {
        return;
    }

    auto TimeStr = m_pintuFileNames.cbegin()->first;
    bool flag = false;

    for(auto trait = m_pintuTraits.cbegin(); trait != m_pintuTraits.cend(); trait++)
    {
        auto item = m_pintuFileNames[TimeStr].cbegin();
        for(; item != m_pintuFileNames[TimeStr].cend(); item++)
        {
            if(item->Find(*trait) != wxNOT_FOUND)
            {
                break;
            }
        }

        if(item == m_pintuFileNames[TimeStr].cend())
        {
            wxString missingFileName;

            missingFileName.Append(TimeStr);
            missingFileName.Append(*trait);

            //m_missingFiles.insert(missingFileName);

			flag = writeToDataBase(missingFileName);
        }
    }
	if(flag)
	{
		m_pintuFileNames.erase(TimeStr);
	}

}

wxString CPintuFileAlarm::getTimeStr(const wxString& fileName)
{
    auto index  = fileName.Find(wxT("."));
	if(index != wxNOT_FOUND)
	{
		return fileName.Left(index);
	}
	return wxString();
}

bool CPintuFileAlarm::writeToDataBase(const wxString& fileName)
{
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
            sqlSentence.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
            sqlSentence.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
            sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("radar_run")));
            sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
            sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
            sqlSentence.Append(wxString::Format(wxT("'%d',"), 0));

            wxString content;
            content.Append(wxString(GetPlug()->getComputerType().c_str(),wxConvLocal));
            content.Append(wxT("没有生成"));
            content.Append(fileName);

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
