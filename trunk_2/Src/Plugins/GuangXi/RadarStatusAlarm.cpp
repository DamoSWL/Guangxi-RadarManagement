#include "RadarStatusAlarm.h"


CRadarStatusAlarm::CRadarStatusAlarm(void)
{
	m_RadarStatusExpFlag["ant_peak_pwr"] = false;
	m_RadarStatusExpFlag["xmtr_peak_pwr"] = false;
	m_RadarStatusExpFlag["ant_avg_pwr"] = false;           
	m_RadarStatusExpFlag["xmtr_avg_pwr"] = false;          
	m_RadarStatusExpFlag["power_meter_ratio_db"] = false;  
	m_RadarStatusExpFlag["ant_pwr_mtr_zero"] = false;      
	m_RadarStatusExpFlag["xmtr_pwr_mtr_zero"] = false;                     
	m_RadarStatusExpFlag["rnspln"] = false;
	m_RadarStatusExpFlag["rnlpln"] = false;                
	m_RadarStatusExpFlag["te"] =false;                      
	m_RadarStatusExpFlag["expected_refl_amp_1"] = false; 
	m_RadarStatusExpFlag["expected_refl_amp_2"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_3"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_4"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_5"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_6"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_7"] = false;
	m_RadarStatusExpFlag["expected_refl_amp_8"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_1"] = false; 
	m_RadarStatusExpFlag["measured_refl_amp_2"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_3"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_4"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_5"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_6"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_7"] = false;
	m_RadarStatusExpFlag["measured_refl_amp_8"] = false;
	m_RadarStatusExpFlag["sysspln"] = false;              
	m_RadarStatusExpFlag["syslpln"] = false;              
	m_RadarStatusExpFlag["expected_velocity_1"] = false; 
	m_RadarStatusExpFlag["expected_velocity_2"] = false;
	m_RadarStatusExpFlag["expected_velocity_3"] = false;
	m_RadarStatusExpFlag["expected_velocity_4"] = false;
	m_RadarStatusExpFlag["measured_velocity_1"] = false; 
	m_RadarStatusExpFlag["measured_velocity_2"] = false;
	m_RadarStatusExpFlag["measured_velocity_3"] = false;
	m_RadarStatusExpFlag["measured_velocity_4"] = false;
	m_RadarStatusExpFlag["expected_velocity_1"] = false; 
	m_RadarStatusExpFlag["expected_velocity_2"] = false;
	m_RadarStatusExpFlag["expected_velocity_3"] = false;
	m_RadarStatusExpFlag["expected_velocity_4"] = false;
	m_RadarStatusExpFlag["measured_velocity_1"] = false;    
	m_RadarStatusExpFlag["measured_velocity_2"] = false;
	m_RadarStatusExpFlag["measured_velocity_3"] = false;
	m_RadarStatusExpFlag["measured_velocity_4"] = false;
	m_RadarStatusExpFlag["shelter_temp"] = false; 	    
	m_RadarStatusExpFlag["xmtr_air_temp"] = false;        
	m_RadarStatusExpFlag["radome_temp"] = false;          
	m_RadarStatusExpFlag["expected_rf8_amp_1"] = false;  
	m_RadarStatusExpFlag["expected_rf8_amp_2"] = false;
	m_RadarStatusExpFlag["expected_rf8_amp_3"] = false;
	m_RadarStatusExpFlag["expected_rf8_amp_4"] = false;
	m_RadarStatusExpFlag["expected_rf8_amp_5"] = false;
	m_RadarStatusExpFlag["expected_rf8_amp_6"] = false;
	m_RadarStatusExpFlag["measured_rf8_amp_1"] = false;  
	m_RadarStatusExpFlag["measured_rf8_amp_2"] = false;
	m_RadarStatusExpFlag["measured_rf8_amp_3"] = false;
	m_RadarStatusExpFlag["measured_rf8_amp_4"] = false;
	m_RadarStatusExpFlag["measured_rf8_amp_5"] = false;
	m_RadarStatusExpFlag["measured_rf8_amp_6"] = false;
	m_RadarStatusExpFlag["unfiltered_lin_char_pwr"] = false;
	m_RadarStatusExpFlag["filtered_lin_char_pwr"] = false; 

	m_RadarStatusContent["ant_peak_pwr"] = "天线峰值功率";
	m_RadarStatusContent["xmtr_peak_pwr"] = "发射机峰值功率";
	m_RadarStatusContent["ant_avg_pwr"] = "天线平均功率";           
	m_RadarStatusContent["xmtr_avg_pwr"] = "发射机平均功率";          
	m_RadarStatusContent["power_meter_ratio_db"] = "发射机和天线功率比";  
	m_RadarStatusContent["ant_pwr_mtr_zero"] = "天线功率调零";      
	m_RadarStatusContent["xmtr_pwr_mtr_zero"] = "发射机功率调零";                     
	m_RadarStatusContent["rnspln"] = "短脉冲噪声电平";
	m_RadarStatusContent["rnlpln"] = "长脉冲噪声电平";                
	m_RadarStatusContent["te"] ="噪声温度 ";                      
	m_RadarStatusContent["expected_refl_amp_1"] = "第一路反射率期望值"; 
	m_RadarStatusContent["expected_refl_amp_2"] = "第二路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_3"] = "第三路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_4"] = "第四路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_5"] = "第五路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_6"] = "第六路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_7"] = "第七路反射率期望值";
	m_RadarStatusContent["expected_refl_amp_8"] = "第八路反射率期望值";
	m_RadarStatusContent["measured_refl_amp_1"] = "第一路反射率测量值"; 
	m_RadarStatusContent["measured_refl_amp_2"] = "第二路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_3"] = "第三路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_4"] = "第四路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_5"] = "第五路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_6"] = "第六路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_7"] = "第七路反射率测量值";
	m_RadarStatusContent["measured_refl_amp_8"] = "第八路反射率测量值";
	m_RadarStatusContent["sysspln"] = "短脉冲系统标定常数";              
	m_RadarStatusContent["syslpln"] = "长脉冲系统标定常数";              
	m_RadarStatusContent["expected_velocity_1"] = "第一路速度期望值"; 
	m_RadarStatusContent["expected_velocity_2"] = "第二路速度期望值";
	m_RadarStatusContent["expected_velocity_3"] = "第三路速度期望值";
	m_RadarStatusContent["expected_velocity_4"] = "第四路速度期望值";
	m_RadarStatusContent["measured_velocity_1"] = "第一路速度测量值"; 
	m_RadarStatusContent["measured_velocity_2"] = "第二路速度测量值";
	m_RadarStatusContent["measured_velocity_3"] = "第三路速度测量值";
	m_RadarStatusContent["measured_velocity_4"] = "第四路速度测量值";
	m_RadarStatusContent["expected_velocity_1"] = "第一路谱宽期望值"; 
	m_RadarStatusContent["expected_velocity_2"] = "第二路谱宽期望值";
	m_RadarStatusContent["expected_velocity_3"] = "第三路谱宽期望值";
	m_RadarStatusContent["expected_velocity_4"] = "第四路谱宽期望值";
	m_RadarStatusContent["measured_velocity_1"] = "第一路谱宽测量值";    
	m_RadarStatusContent["measured_velocity_2"] = "第二路谱宽测量值";
	m_RadarStatusContent["measured_velocity_3"] = "第三路谱宽测量值";
	m_RadarStatusContent["measured_velocity_4"] = "第四路谱宽测量值";
	m_RadarStatusContent["shelter_temp"] = "机房温度"; 	    
	m_RadarStatusContent["xmtr_air_temp"] = "发射机空气温度 ";        
	m_RadarStatusContent["radome_temp"] = "天线罩温度";          
	m_RadarStatusContent["expected_rf8_amp_1"] = "第一路KD标定期望值";  
	m_RadarStatusContent["expected_rf8_amp_2"] = "第二路KD标定期望值";
	m_RadarStatusContent["expected_rf8_amp_3"] = "第三路KD标定期望值";
	m_RadarStatusContent["expected_rf8_amp_4"] = "第四路KD标定期望值";
	m_RadarStatusContent["expected_rf8_amp_5"] = "第五路KD标定期望值";
	m_RadarStatusContent["expected_rf8_amp_6"] = "第六路KD标定期望值";
	m_RadarStatusContent["measured_rf8_amp_1"] = "第一路KD标定测量值";  
	m_RadarStatusContent["measured_rf8_amp_2"] = "第二路KD标定测量值";
	m_RadarStatusContent["measured_rf8_amp_3"] = "第三路KD标定测量值";
	m_RadarStatusContent["measured_rf8_amp_4"] = "第四路KD标定测量值";
	m_RadarStatusContent["measured_rf8_amp_5"] = "第五路KD标定测量值";
	m_RadarStatusContent["measured_rf8_amp_6"] = "第六路KD标定测量值";
	m_RadarStatusContent["unfiltered_lin_char_pwr"] = "滤波前功率";
	m_RadarStatusContent["filtered_lin_char_pwr"] = "滤波后功率";


}


CRadarStatusAlarm::~CRadarStatusAlarm(void)
{
}


void CRadarStatusAlarm::getAlarmType()
{
    wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);	
	if (pDataBase)
	{
		try
		{

			char* sqlQuery = "SELECT eName, type FROM threshold_param WHERE tableName = 'radar_calibration_param'";
			int resultNum = pDataBase->ExecuteQuery(sqlQuery);
			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = pDataBase->GetStringValue(index,"eName",length);
					if(sqlTmpStr)
					{
						std::string eName(sqlTmpStr,strlen(sqlTmpStr));
						sqlTmpStr = pDataBase->GetStringValue(index,"type",length);
						if(sqlTmpStr)
						{
							std::string type(sqlTmpStr,strlen(sqlTmpStr));
							m_alarmType[eName] = type;
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

void CRadarStatusAlarm::getThresholdValue()
{
    wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);	
	if (pDataBase)
	{
		try
		{
			char* sqlQuery = "SELECT upperLimit, lowerLimit, eName FROM threshold_param WHERE tableName = 'radar_calibration_param'";
			int resultNum = pDataBase->ExecuteQuery(sqlQuery);
			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = pDataBase->GetStringValue(index,"eName",length);
					{
						if(sqlTmpStr)
						{
							std::string eName(sqlTmpStr,strlen(sqlTmpStr));
							std::pair<FLOAT,FLOAT> pair;
							pair.first = pDataBase->GetFloatValue(index,"lowerLimit");
							pair.second = pDataBase->GetFloatValue(index,"upperLimit");
							if((pair.first == 0) && (pair.second == 0))
							{
								continue;
							}
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

void CRadarStatusAlarm::getRadarStatusData(const RADAR_STATUS_DATA& radarData)
{
	m_RadarStatusValues["ant_peak_pwr"] = radarData.ANT_PEAK_PWR;
	m_RadarStatusValues["xmtr_peak_pwr"] = radarData.XMTR_PEAK_PWR;
	m_RadarStatusValues["ant_avg_pwr"] = radarData.ANT_AVG_PWR;           
	m_RadarStatusValues["xmtr_avg_pwr"] = radarData.XMTR_AVG_PWR;          
	m_RadarStatusValues["power_meter_ratio_db"] = radarData.POWER_METER_RATIO_DB;  
	m_RadarStatusValues["ant_pwr_mtr_zero"] = radarData.ANT_PWR_MTR_ZERO;      
	m_RadarStatusValues["xmtr_pwr_mtr_zero"] = radarData.XMTR_PWR_MTR_ZERO;                     
	m_RadarStatusValues["rnspln"] = radarData.RNSPLN;
	m_RadarStatusValues["rnlpln"] = radarData.RNLPLN;                
	m_RadarStatusValues["te"] = radarData.TE;                      
	m_RadarStatusValues["expected_refl_amp_1"] = radarData.EXPECTED_REFL_AMP[0]; 
	m_RadarStatusValues["expected_refl_amp_2"] = radarData.EXPECTED_REFL_AMP[1];
	m_RadarStatusValues["expected_refl_amp_3"] = radarData.EXPECTED_REFL_AMP[2];
	m_RadarStatusValues["expected_refl_amp_4"] = radarData.EXPECTED_REFL_AMP[3];
	m_RadarStatusValues["expected_refl_amp_5"] = radarData.EXPECTED_REFL_AMP[4];
	m_RadarStatusValues["expected_refl_amp_6"] = radarData.EXPECTED_REFL_AMP[5];
	m_RadarStatusValues["expected_refl_amp_7"] = radarData.EXPECTED_REFL_AMP[6];
	m_RadarStatusValues["expected_refl_amp_8"] = radarData.EXPECTED_REFL_AMP[7];
	m_RadarStatusValues["measured_refl_amp_1"] = radarData.MEASURED_REFL_AMP[0]; 
	m_RadarStatusValues["measured_refl_amp_2"] = radarData.MEASURED_REFL_AMP[1];
	m_RadarStatusValues["measured_refl_amp_3"] = radarData.MEASURED_REFL_AMP[2];
	m_RadarStatusValues["measured_refl_amp_4"] = radarData.MEASURED_REFL_AMP[3];
	m_RadarStatusValues["measured_refl_amp_5"] = radarData.MEASURED_REFL_AMP[4];
	m_RadarStatusValues["measured_refl_amp_6"] = radarData.MEASURED_REFL_AMP[5];
	m_RadarStatusValues["measured_refl_amp_7"] = radarData.MEASURED_REFL_AMP[6];
	m_RadarStatusValues["measured_refl_amp_8"] = radarData.MEASURED_REFL_AMP[7];
	m_RadarStatusValues["sysspln"] = radarData.SYSSPLN;              
	m_RadarStatusValues["syslpln"] = radarData.SYSLPLN;              
	m_RadarStatusValues["expected_velocity_1"] = radarData.EXPECTED_VELOCITY[0]; 
	m_RadarStatusValues["expected_velocity_2"] = radarData.EXPECTED_VELOCITY[1];
	m_RadarStatusValues["expected_velocity_3"] = radarData.EXPECTED_VELOCITY[2];
	m_RadarStatusValues["expected_velocity_4"] = radarData.EXPECTED_VELOCITY[3];
	m_RadarStatusValues["measured_velocity_1"] = radarData.MEASURED_VELOCITY[0]; 
	m_RadarStatusValues["measured_velocity_2"] = radarData.MEASURED_VELOCITY[1];
	m_RadarStatusValues["measured_velocity_3"] = radarData.MEASURED_VELOCITY[2];
	m_RadarStatusValues["measured_velocity_4"] = radarData.MEASURED_VELOCITY[3];
	m_RadarStatusValues["expected_velocity_1"] = radarData.EXPECTED_WIDTH[0]; 
	m_RadarStatusValues["expected_velocity_2"] = radarData.EXPECTED_WIDTH[1];
	m_RadarStatusValues["expected_velocity_3"] = radarData.EXPECTED_WIDTH[2];
	m_RadarStatusValues["expected_velocity_4"] = radarData.EXPECTED_WIDTH[3];
	m_RadarStatusValues["measured_velocity_1"] = radarData.MEASURED_WIDTH[0];    
	m_RadarStatusValues["measured_velocity_2"] = radarData.MEASURED_WIDTH[1];
	m_RadarStatusValues["measured_velocity_3"] = radarData.MEASURED_WIDTH[2];
	m_RadarStatusValues["measured_velocity_4"] = radarData.MEASURED_WIDTH[3];
	m_RadarStatusValues["shelter_temp"] = radarData.SHELTER_TEMP; 	    
	m_RadarStatusValues["xmtr_air_temp"] = radarData.XMTR_AIR_TEMP;        
	m_RadarStatusValues["radome_temp"] = radarData.RADOME_TEMP;          
	m_RadarStatusValues["expected_rf8_amp_1"] = radarData.EXPECTED_RF8_AMP[0];  
	m_RadarStatusValues["expected_rf8_amp_2"] = radarData.EXPECTED_RF8_AMP[1];
	m_RadarStatusValues["expected_rf8_amp_3"] = radarData.EXPECTED_RF8_AMP[2];
	m_RadarStatusValues["expected_rf8_amp_4"] = radarData.EXPECTED_RF8_AMP[3];
	m_RadarStatusValues["expected_rf8_amp_5"] = radarData.EXPECTED_RF8_AMP[4];
	m_RadarStatusValues["expected_rf8_amp_6"] = radarData.EXPECTED_RF8_AMP[5];
	m_RadarStatusValues["measured_rf8_amp_1"] = radarData.MEASURED_RF8_AMP[0];  
	m_RadarStatusValues["measured_rf8_amp_2"] = radarData.MEASURED_RF8_AMP[1];
	m_RadarStatusValues["measured_rf8_amp_3"] = radarData.MEASURED_RF8_AMP[2];
	m_RadarStatusValues["measured_rf8_amp_4"] = radarData.MEASURED_RF8_AMP[3];
	m_RadarStatusValues["measured_rf8_amp_5"] = radarData.MEASURED_RF8_AMP[4];
	m_RadarStatusValues["measured_rf8_amp_6"] = radarData.MEASURED_RF8_AMP[5];
	m_RadarStatusValues["unfiltered_lin_char_pwr"] = radarData.UNFILTERED_LIN_CHAN_PWR;
	m_RadarStatusValues["filtered_lin_char_pwr"] = radarData.FILTERED_LIN_CHAN_PWR; 
}

void CRadarStatusAlarm::RadarStatusCheck()
{
	for(auto item = m_RadarStatusValues.cbegin(); item != m_RadarStatusValues.cend(); item++)
	{
		auto name = item->first;
		
		if(m_ThreadholdValues.find(name) != m_ThreadholdValues.cend())
		{
			auto value = m_RadarStatusValues[name];
			auto lowerLimit = m_ThreadholdValues[name].first;
			auto upperLimit = m_ThreadholdValues[name].second;
			

			if(m_RadarStatusExpFlag[name])
			{
				if((value <= upperLimit) && (value >= lowerLimit))
				{
					m_RadarStatusExpFlag[name] = false;
					m_RadarStatusEndureTime[name].second = ::wxGetUTCTime();
					writeToDataBasewithEndTime(name);
				}

			}
			else
			{
				if(value > upperLimit)
				{
					m_RadarStatusExpFlag[name] = true;
					m_RadarStatusEndureTime[name].first = ::wxGetUTCTime();
					m_RadarStatusConcreteContent[name] = m_RadarStatusContent[name];
					m_RadarStatusConcreteContent[name].append("取样值过高");
					writeToDataBase(name);

				}
				else if(value < lowerLimit)
				{
					m_RadarStatusExpFlag[name] = true;
					m_RadarStatusEndureTime[name].first = ::wxGetUTCTime();
					m_RadarStatusConcreteContent[name] = m_RadarStatusContent[name];
					m_RadarStatusConcreteContent[name].append("取样值过低");
					writeToDataBase(name);
				}
			}

		}

	}

	for(auto item = m_RadarStatusExpFlag.cbegin(); item != m_RadarStatusExpFlag.cend(); item++)
	{
		if(item->second)
		{
			GetPlug()->setExcessiveFlag(true);
			return;
		}
	}

	GetPlug()->setExcessiveFlag(false);
}

bool CRadarStatusAlarm::writeToDataBase(const std::string& name)
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
	if (pDataBase)
	{
		try
		{
	
			sqlSentence.Append(wxString::Format(wxT("(%d, "), m_RadarStatusEndureTime[name].first));
			sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("radar_run")));
			sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
			sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("0")));
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxString(m_RadarStatusConcreteContent[name].c_str(),wxConvLocal)));
			sqlSentence.Append(wxString::Format(wxT("%d, "), 2));
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

bool CRadarStatusAlarm::writeToDataBasewithEndTime(const std::string& name)
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
			sqlSentence.Printf(wxT("UPDATE alarm_info SET endTime=%d WHERE time=%d  AND endTime is NULL AND content='%s'"), m_RadarStatusEndureTime[name].second,
																										m_RadarStatusEndureTime[name].first,
																										wxString(m_RadarStatusConcreteContent[name].c_str(),wxConvLocal));
		
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
