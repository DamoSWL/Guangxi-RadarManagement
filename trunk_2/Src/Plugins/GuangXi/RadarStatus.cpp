#include "RadarStatus.h"


CRadarStatus::CRadarStatus(void)
{
}


CRadarStatus::~CRadarStatus(void)
{
}

void CRadarStatus::getFileContent(std::map<wxString,UINT>& filesInfo)
{
	for(auto item = filesInfo.cbegin(); item != filesInfo.cend(); item++)
	{
		RADAR_STATUS_DATA radarStatus;
		auto fullPath = GetPlug()->getZhuangtaiDir();
		fullPath.Append(wxT("\\"));
		fullPath.Append(item->first);
		wxFile file(fullPath);
		
		if(file.IsOpened())
		{
			file.Read((void*)&radarStatus,sizeof(RADAR_STATUS_DATA));
			m_radarStatus[item->second] = radarStatus;

		}
		file.Close();

	}
}

bool CRadarStatus::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO radar_calibration_param (updateTime, \
																				radarcd, \
																				sel_patt, \
																				localFlg, \
																				ant_peak_pwr, \
																				xmtr_peak_pwr, \
																				ant_avg_pwr, \
																				xmtr_avg_pwr, \
																				power_meter_ratio_db, \
																				ant_pwr_mtr_zero, \
																				xmtr_pwr_mtr_zero, \
																				rnspln, \
																				rnlpln, \
																				te, \
																				idu_test_detections, \
																				sysstatus, \
																				expected_refl_amp_1, \
																				expected_refl_amp_2, \
																				expected_refl_amp_3, \
																				expected_refl_amp_4, \
																				expected_refl_amp_5, \
																				expected_refl_amp_6, \
																				expected_refl_amp_7, \
																				expected_refl_amp_8, \
																				measured_refl_amp_1, \
																				measured_refl_amp_2, \
																				measured_refl_amp_3, \
																				measured_refl_amp_4, \
																				measured_refl_amp_5, \
																				measured_refl_amp_6, \
																				measured_refl_amp_7, \
																				measured_refl_amp_8, \
																				sysspln, \
																				syslpln, \
																				expected_velocity_1, \
																				expected_velocity_2, \
																				expected_velocity_3, \
																				expected_velocity_4, \
																				measured_velocity_1, \
																				measured_velocity_2, \
																				measured_velocity_3, \
																				measured_velocity_4, \
																				expected_width_1, \
																				expected_width_2, \
																				expected_width_3, \
																				expected_width_4, \
																				measured_width_1, \
																				measured_width_2, \
																				measured_width_3, \
																				measured_width_4, \
																				shelter_temp, \
																				xmtr_air_temp, \
																				radome_temp, \
																				expected_rf8_amp_1, \
																				expected_rf8_amp_2, \
																				expected_rf8_amp_3, \
																				expected_rf8_amp_4, \
																				expected_rf8_amp_5, \
																				expected_rf8_amp_6, \
																				measured_rf8_amp_1, \
																				measured_rf8_amp_2, \
																				measured_rf8_amp_3, \
																				measured_rf8_amp_4, \
																				measured_rf8_amp_5, \
																				measured_rf8_amp_6, \
																				unfiltered_lin_char_pwr, \
																				filtered_lin_char_pwr) VALUES");

	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);

	if(pDataBase)
	{
		try
		{
			for(auto item = m_radarStatus.cbegin(); item != m_radarStatus.cend(); item++)
			{
				auto sqlConSentence = sqlSentence;
				sqlConSentence.Append(wxString::Format(wxT("(%d, "),item->first));
				sqlConSentence.Append(wxString::Format(wxT("'%s', "),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.SEL_PATT));              //体扫模式 11，21，31
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.LOCALFLG));              //控制标志  LOCAL = 2；REMOTE = 4；EITHER = 8
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_PEAK_PWR));           //天线峰值功率
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_PEAK_PWR));          //发射机峰值功率
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_AVG_PWR));            //天线平均功率
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_AVG_PWR));           //发射机平均功率
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.POWER_METER_RATIO_DB));   
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_PWR_MTR_ZERO));       //天线功率调零
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_PWR_MTR_ZERO));      //发射机功率调零
				if(item->second.RNSPLN > 0)
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), std::log10(item->second.RNSPLN)));                 //短脉冲噪声电平
				}
				else
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), -99.0));                 //短脉冲噪声电平
				}
				
				if(item->second.RNLPLN > 0)
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), std::log10(item->second.RNLPLN)));                 //长脉冲噪声电平
				}
				else
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), -99.0));                 //短脉冲噪声电平
				}
				
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.TE));                     //噪声温度 
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.IDU_TEST_DETECTIONS));    
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.SysStatus));  //1表示系统正常，2表示系统可用，3表示系统需要维护，4表示系统故障不能工作，5表示系统关机或者STANDBY。
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[0])); //反射率期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[1]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[2]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[3]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[4]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[5]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[6]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[7]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[0])); 
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[1]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[2]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[3]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[4]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[5]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[6]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_REFL_AMP[7]));
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SYSSPLN));              //短脉冲系统标定常数
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SYSLPLN));              //长脉冲系统标定常数
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[0])); //速度期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[1])); //速度期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[2])); //速度期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[3])); //速度期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[0])); //速度测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[1])); //速度测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[2])); //速度测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[3])); //速度测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[0]));    //谱宽期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[1]));    //谱宽期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[2]));    //谱宽期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[3]));    //谱宽期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[0]));    //谱宽测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[1]));    //谱宽测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[2]));    //谱宽测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[3]));    //谱宽测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SHELTER_TEMP)); 	    //机房温度        
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_AIR_TEMP));        //发射机空气温度  
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.RADOME_TEMP));          //天线罩温度      
		      	sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[0]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[1]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[2]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[3]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[4]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[5]));  //KD标定期望值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[0]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[1]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[2]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[3]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[4]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[5]));  //KD标定测量值
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.UNFILTERED_LIN_CHAN_PWR)); //滤波前功率
				sqlConSentence.Append(wxString::Format(wxT("%f)"), item->second.FILTERED_LIN_CHAN_PWR));   //滤波后功率

				if(!pDataBase->ExecuteNoQuery(sqlConSentence,sqlError))
				{
					throw sqlError;
				}

			}

			return true;	


		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}

	}

	m_radarStatus.clear();

	return false;
}

void CRadarStatus::radarStatusCheck()
{
	m_radarStatusAlarm.getAlarmType();
	m_radarStatusAlarm.getThresholdValue();
	for(auto item = m_radarStatus.cbegin(); item != m_radarStatus.cend(); item++)
	{
		m_radarStatusAlarm.getRadarStatusData(item->second);
		m_radarStatusAlarm.RadarStatusCheck();
	}
	m_radarStatus.clear();
	
}
