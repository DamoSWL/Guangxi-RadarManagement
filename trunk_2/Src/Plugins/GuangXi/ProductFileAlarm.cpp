#include "ProductFileAlarm.h"
#include <ctime>
#include <wx/dir.h>


CProductFileAlarm::CProductFileAlarm(void)

{


}


CProductFileAlarm::~CProductFileAlarm(void)
{
}

void CProductFileAlarm::setFileTraits()
{
    if(GetPlug()->getComputerType().find("PUP") != std::string::npos)
	{
		m_productTraits.insert(wxT("R_10_230_5"));
		m_productTraits.insert(wxT("R_10_230_15"));
		m_productTraits.insert(wxT("R_10_230_24"));
		m_productTraits.insert(wxT("R_10_230_34"));
		m_productTraits.insert(wxT("R_10_230_43"));
		m_productTraits.insert(wxT("R_10_230_60"));
		m_productTraits.insert(wxT("R_20_460_5"));
		m_productTraits.insert(wxT("R_20_460_15"));
		m_productTraits.insert(wxT("R_20_460_24"));

		m_productTraits.insert(wxT("V_5_115_5"));
		m_productTraits.insert(wxT("V_5_115_15"));
		m_productTraits.insert(wxT("V_5_115_24"));
		m_productTraits.insert(wxT("V_10_230_5"));
		m_productTraits.insert(wxT("V_10_230_15"));
		m_productTraits.insert(wxT("V_10_230_24"));
		m_productTraits.insert(wxT("V_10_230_34"));
		m_productTraits.insert(wxT("V_10_230_43"));
		m_productTraits.insert(wxT("V_10_230_60"));

		m_productTraits.insert(wxT("HSR_10_230_NUL"));
		m_productTraits.insert(wxT("CR_10X10_230_NUL"));
		m_productTraits.insert(wxT("CR_40X40_460_NUL"));
		m_productTraits.insert(wxT("ET_40_230X230_NUL"));
		m_productTraits.insert(wxT("VWP_20_NUL_NUL"));
		m_productTraits.insert(wxT("WER_10_50X50_NUL"));
		m_productTraits.insert(wxT("SRM_10_230_15"));
		m_productTraits.insert(wxT("VIL_40X40_230_NUL"));
		m_productTraits.insert(wxT("STI_NUL_345_NUL"));
		m_productTraits.insert(wxT("HI_NUL_230_NUL"));
		m_productTraits.insert(wxT("M_NUL_230_NUL"));
		m_productTraits.insert(wxT("TVS_NUL_230_NUL"));
		m_productTraits.insert(wxT("SS_NUL_345_NUL"));
		m_productTraits.insert(wxT("OHP_20_230_NUL"));
		m_productTraits.insert(wxT("THP_20_230_NUL"));
		m_productTraits.insert(wxT("STP_20_230_NUL"));
		m_productTraits.insert(wxT("CS_15_230x230_15_NUL"));
		m_productTraits.insert(wxT("CAR_10_230_NUL"));
	}
}




void CProductFileAlarm::insertFileName(const wxString& fileName)
{
	auto TimeStr = getTimeStr(fileName);
	if(TimeStr.IsEmpty())
	{
		return;
	}
	auto fileSpec = wxString::Format(wxT("*_%s_%s*"),TimeStr,wxT("P_DOR_"));

	wxString productFileName;
	wxDir dir(GetPlug()->getProductDir());
	auto flag = dir.GetFirst(&productFileName,fileSpec,wxDIR_FILES);
	
	while(flag)
	{
		if(m_productFileNames[TimeStr].find(productFileName) == m_productFileNames[TimeStr].end())
		{
			m_productFileNames[TimeStr].insert(productFileName);
		}
		flag = dir.GetNext(&productFileName);
	}
	
}

void CProductFileAlarm::productFileCheck()
{
    if(m_productFileNames.size() < 2)
    {
        return;
    }

    auto TimeStr = m_productFileNames.cbegin()->first;
	bool flag = false;

    for(auto trait = m_productTraits.cbegin(); trait != m_productTraits.cend(); trait++)
    {
		auto item = m_productFileNames[TimeStr].cbegin();
        for(; item != m_productFileNames[TimeStr].cend(); item++)
        {
            if(item->Find(*trait) != wxNOT_FOUND)
			{
				break;
			}
        }

        if(item == m_productFileNames[TimeStr].cend())
        {
			wxString missingFileName;
			
            missingFileName.Append(wxT("Z_RADR_I_"));
			missingFileName.Append(wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal).Upper());
			missingFileName.Append(wxT("_"));
			missingFileName.Append(TimeStr);
			missingFileName.Append(wxT("_P_DOR_"));
			missingFileName.Append(wxString(GetPlug()->getRadarTypeStr().c_str(),wxConvLocal));
			missingFileName.Append(wxT("_"));
			missingFileName.Append(*trait);
			missingFileName.Append(wxT(".771.bin"));
			//m_missingFiles.insert(missingFileName);

            flag = writeToDataBase(missingFileName,getTime(TimeStr));
        }
    }

	if(flag)
	{
		m_productFileNames.erase(TimeStr);
	}
}

wxString CProductFileAlarm::getTimeStr(const wxString& fileName)
{
    if(fileName.Length() > 30)
	{
		return fileName.Mid(15,14);
			
	}

	return wxString();

}

UINT CProductFileAlarm::getTime(const wxString& TimeStr)
{
    if(!TimeStr.IsEmpty())
	{
		ULONG year = 0;
		ULONG month = 0;
		ULONG day = 0;
		ULONG hour= 0;
		ULONG minute = 0;
		ULONG second = 0;

		TimeStr.Left(4).ToULong(&year);
		TimeStr.Mid(4,2).ToULong(&month);
		TimeStr.Mid(6,2).ToULong(&day);
		TimeStr.Mid(8,2).ToULong(&hour);
		TimeStr.Mid(10,2).ToULong(&minute);
		TimeStr.Right(2).ToULong(&second);

        struct tm generationTime;
        generationTime.tm_year = year - 1900;
        generationTime.tm_mon = month - 1;
        generationTime.tm_mday = day - 1;
        generationTime.tm_hour = hour;
        generationTime.tm_min = minute;
        generationTime.tm_sec = second;

		return std::mktime(&generationTime);
		
	}

	return -1;
}

bool CProductFileAlarm::writeToDataBase(const wxString& fileName,UINT time)
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
			sqlSentence.Append(wxString::Format(wxT("(%d, "),time));
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

