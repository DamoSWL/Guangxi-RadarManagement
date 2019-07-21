#include "ThreePhaseAlarm.h"


CThreePhaseAlarm::CThreePhaseAlarm(void)
{
    m_ThreePhaseExpFlag["ThreePhaseAVoltage"] = false;
    m_ThreePhaseExpFlag["ThreePhaseACurrent"] = false;
    m_ThreePhaseExpFlag["ThreePhaseAPower"] = false;
    m_ThreePhaseExpFlag["ThreePhaseBVoltage"] = false;
    m_ThreePhaseExpFlag["ThreePhaseBCurrent"] = false;
    m_ThreePhaseExpFlag["ThreePhaseBPower"] = false;
    m_ThreePhaseExpFlag["ThreePhaseCVoltage"] = false;
    m_ThreePhaseExpFlag["ThreePhaseCCurrent"] = false;
    m_ThreePhaseExpFlag["ThreePhaseCPower"] = false;
    m_ThreePhaseExpFlag["ThreePhaseFrequency"] = false;

    m_ThreePhaseContent["ThreePhaseAVoltage"] = "三相电A项电压";
    m_ThreePhaseContent["ThreePhaseACurrent"] = "三相电A项电流";
    m_ThreePhaseContent["ThreePhaseAPower"] = "三相电A项功率";
    m_ThreePhaseContent["ThreePhaseBVoltage"] = "三相电B项电压";
    m_ThreePhaseContent["ThreePhaseBCurrent"] = "三相电B项电流";
    m_ThreePhaseContent["ThreePhaseBPower"] = "三相电B项功率";
    m_ThreePhaseContent["ThreePhaseCVoltage"] = "三相电C项电压";
    m_ThreePhaseContent["ThreePhaseCCurrent"] = "三相电C项电流";
    m_ThreePhaseContent["ThreePhaseCPower"] = "三相电C项功率";
    m_ThreePhaseContent["ThreePhaseFrequency"] = "三相电频率";
}


CThreePhaseAlarm::~CThreePhaseAlarm(void)
{
}

void CThreePhaseAlarm::getAlarmType()
{
    m_alarmType["ThreePhaseAVoltage"] = "power";
    m_alarmType["ThreePhaseACurrent"] = "power";
    m_alarmType["ThreePhaseAPower"] = "power";
    m_alarmType["ThreePhaseBVoltage"] = "power";
    m_alarmType["ThreePhaseBCurrent"] = "power";
    m_alarmType["ThreePhaseBPower"] = "power";
    m_alarmType["ThreePhaseCVoltage"] = "power";
    m_alarmType["ThreePhaseCCurrent"] = "power";
    m_alarmType["ThreePhaseCPower"] = "power";
    m_alarmType["ThreePhaseFrequency"] = "power";
}

void CThreePhaseAlarm::getThresholdValue()
{
    wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);

	if (pDataBase)
	{
		try
		{
			char* sqlQuery = "SELECT upperLimit, lowerLimit, eName FROM threshold_param WHERE tableName = 'radar_run_power'";
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

void CThreePhaseAlarm::getThreePhaseValue(const ThreePhaseParam& param)
{
    m_ThreePhaseValues["ThreePhaseAVoltage"] = param.APhasePower / 100.0;
    m_ThreePhaseValues["ThreePhaseACurrent"] = param.APhaseCurrent / 1000.0;
    m_ThreePhaseValues["ThreePhaseAPower"] = param.APhasePowerRatio / 1.0;
    m_ThreePhaseValues["ThreePhaseBVoltage"] = param.BPhasePower / 100.0;
    m_ThreePhaseValues["ThreePhaseBCurrent"] = param.BPhaseCurrent / 1000.0;
    m_ThreePhaseValues["ThreePhaseBPower"] = param.BPhasePowerRatio / 1.0;
    m_ThreePhaseValues["ThreePhaseCVoltage"] = param.CPhasePower / 100.0;
    m_ThreePhaseValues["ThreePhaseCCurrent"] = param.CPhaseCurrent / 1000.0;
    m_ThreePhaseValues["ThreePhaseCPower"] = param.CPhasePowerRatio / 1.0;
    m_ThreePhaseValues["ThreePhaseFrequency"] = param.frequency / 100.0;
}

void CThreePhaseAlarm::ThreePhaseCheck()
{
    for(auto item = m_ThreePhaseValues.cbegin();item != m_ThreePhaseValues.cend(); item++)
    {
        auto name = item->first;
        if(m_ThreadholdValues.find(name) != m_ThreadholdValues.cend())
        {
            auto value = m_ThreePhaseValues[name];
            auto lowerLimit = m_ThreadholdValues[name].first;
            auto upperLimit = m_ThreadholdValues[name].second;

            if(m_ThreePhaseExpFlag[name])
			{
				if((value <= upperLimit) || (value >= lowerLimit))
				{
					m_ThreePhaseExpFlag[name] = false;
					m_ThreePhaseEndureTime[name].second = ::wxGetUTCTime();
					writeToDataBasewithEndTime(name);
				}

			}
			else
			{
				if(value > upperLimit)
				{
					m_ThreePhaseExpFlag[name] = true;
					m_ThreePhaseEndureTime[name].first = ::wxGetUTCTime();
					m_ThreePhaseConcreteContent[name] = m_ThreePhaseContent[name];
					m_ThreePhaseConcreteContent[name].append("取样值过高");
					writeToDataBase(name);

				}
				else if(value < lowerLimit)
				{
					m_ThreePhaseExpFlag[name] = true;
					m_ThreePhaseEndureTime[name].first = ::wxGetUTCTime();
					m_ThreePhaseConcreteContent[name] = m_ThreePhaseContent[name];
					m_ThreePhaseConcreteContent[name].append("取样值过低");
					writeToDataBase(name);
				}
			}
            
        }
    }
}

bool CThreePhaseAlarm::writeToDataBase(const std::string name)
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
			sqlSentence.append(wxString::Format(wxT("(%d, "), m_ThreePhaseEndureTime[name].first));
			sqlSentence.append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.append(wxString::Format(wxT("'%s', "),  wxString(m_alarmType[name].c_str(),wxConvLocal)));
			sqlSentence.append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.append(wxString::Format(wxT("%d, "), 1));
			sqlSentence.append(wxString::Format(wxT("'%s', "), wxT("0")));
			sqlSentence.append(wxString::Format(wxT("'%s', "), wxString(m_ThreePhaseConcreteContent[name].c_str(),wxConvLocal)));
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

bool CThreePhaseAlarm::writeToDataBasewithEndTime(const std::string name)
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
			sqlSentence.Printf(wxT("UPDATE alarm_info SET endTime=%d WHERE endTime IS NULL AND time=%d AND content='%s'"), m_ThreePhaseEndureTime[name].second,
																										m_ThreePhaseEndureTime[name].first,
																										wxString(m_ThreePhaseConcreteContent[name].c_str(),wxConvLocal));
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
