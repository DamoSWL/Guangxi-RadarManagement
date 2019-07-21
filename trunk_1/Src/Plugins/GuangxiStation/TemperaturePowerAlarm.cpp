#include "TemperaturePowerAlarm.h"


CTemperaturePowerAlarm::CTemperaturePowerAlarm(void)
{
    m_envExpFlag["TemperatureRadar"] = false;
    m_envExpFlag["HumidityRadar"] = false;

    m_envContent["TemperatureRadar"] = "油机房房温度";
    m_envContent["HumidityRadar"] = "油机房房湿度";
}


CTemperaturePowerAlarm::~CTemperaturePowerAlarm(void)
{
}

void CTemperaturePowerAlarm::getAlarmType()
{
    m_alarmType["TemperatureRadar"] = "env";
    m_alarmType["HumidityRadar"] = "env";
}


void CTemperaturePowerAlarm::getThresholdValue()
{
    wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);

	if (pDataBase)
	{
		try
		{
			char* sqlQuery = "SELECT upperLimit, lowerLimit, eName FROM threshold_param WHERE tableName = 'radar_run_env'";
			int resultNum = pDataBase->ExecuteQuery(sqlQuery);
			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
                    auto sqlTmpStr = pDataBase->GetStringValue(index,"eName",length);
                    {
                        if(sqlTmpStr)
                        {
                            std::string eName(sqlTmpStr,strlen(sqlTmpStr));
                            std::pair<FLOAT,FLOAT> pair;
                            pair.first = pDataBase->GetFloatValue(index,"lowerLimit");
                            pair.second = pDataBase->GetFloatValue(index,"upperLimit");
                            m_ThreadholdValues[eName] = pair;
                        }
                    }
				}
				pDataBase->FreeResult(sqlError);

			}

		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}
	}
}


void CTemperaturePowerAlarm::getEnvValue(FLOAT temperature,UINT humidity)
{
    m_envValues["TemperatureRadar"] = temperature;
    m_envValues["HumidityRadar"] = humidity * 1.0;
}

void CTemperaturePowerAlarm::envCheck()
{
    for(auto item = m_envValues.cbegin();item != m_envValues.cend(); item++)
    {
        auto name = item->first;
        if(m_ThreadholdValues.find(name) != m_ThreadholdValues.cend())
        {
            auto value = m_envValues[name];
            auto lowerLimit = m_ThreadholdValues[name].first;
            auto upperLimit = m_ThreadholdValues[name].second;

            if(m_envExpFlag[name])
			{
				if((value <= upperLimit) || (value >= lowerLimit))
				{
					m_envExpFlag[name] = false;
					m_envEndureTime[name].second = ::wxGetUTCTime();
					writeToDataBasewithEndTime(name);
				}

			}
			else
			{
				if(value > upperLimit)
				{
					m_envExpFlag[name] = true;
					m_envEndureTime[name].first = ::wxGetUTCTime();
					m_envConcreteContent[name] = m_envContent[name];
					m_envConcreteContent[name].append("取样值过高");
					writeToDataBase(name);

				}
				else if(value < lowerLimit)
				{
					m_envExpFlag[name] = true;
					m_envEndureTime[name].first = ::wxGetUTCTime();
					m_envConcreteContent[name] = m_envContent[name];
					m_envConcreteContent[name].append("取样值过低");
					writeToDataBase(name);
				}
			}
            
        }
    }
}

bool CTemperaturePowerAlarm::writeToDataBase(const std::string name)
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

	try
	{
		if(pDataBase)
		{
			sqlSentence.append(wxString::Format(wxT("(%d, "), m_envEndureTime[name].first));
			sqlSentence.append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.append(wxString::Format(wxT("'%s', "),  wxString(m_alarmType[name].c_str(),wxConvLocal)));
			sqlSentence.append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.append(wxString::Format(wxT("%d, "), 1));
			sqlSentence.append(wxString::Format(wxT("'%s', "), wxT("0")));
			sqlSentence.append(wxString::Format(wxT("'%s', "), wxString(m_envConcreteContent[name].c_str(),wxConvLocal)));
            sqlSentence.append(wxString::Format(wxT("%d, "), 2));
            sqlSentence.Append(wxString::Format(wxT("'%s')"), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
		}
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

	return false;

}

bool CTemperaturePowerAlarm::writeToDataBasewithEndTime(const std::string name)
{
    wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);	
	if (pDataBase)
	{
		try
		{
			wxString sqlSentence; 
			sqlSentence.Printf(wxT("UPDATE alarm_info SET endTime=%d WHERE endTime IS NULL AND time=%d  AND content='%s'"), m_envEndureTime[name].second,
																										m_envEndureTime[name].first,
																										wxString(m_envConcreteContent[name].c_str(),wxConvLocal));
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


