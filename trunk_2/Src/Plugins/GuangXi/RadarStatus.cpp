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
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.SEL_PATT));              //��ɨģʽ 11��21��31
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.LOCALFLG));              //���Ʊ�־  LOCAL = 2��REMOTE = 4��EITHER = 8
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_PEAK_PWR));           //���߷�ֵ����
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_PEAK_PWR));          //�������ֵ����
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_AVG_PWR));            //����ƽ������
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_AVG_PWR));           //�����ƽ������
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.POWER_METER_RATIO_DB));   
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.ANT_PWR_MTR_ZERO));       //���߹��ʵ���
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_PWR_MTR_ZERO));      //��������ʵ���
				if(item->second.RNSPLN > 0)
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), std::log10(item->second.RNSPLN)));                 //������������ƽ
				}
				else
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), -99.0));                 //������������ƽ
				}
				
				if(item->second.RNLPLN > 0)
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), std::log10(item->second.RNLPLN)));                 //������������ƽ
				}
				else
				{
					sqlConSentence.Append(wxString::Format(wxT("%f, "), -99.0));                 //������������ƽ
				}
				
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.TE));                     //�����¶� 
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.IDU_TEST_DETECTIONS));    
				sqlConSentence.Append(wxString::Format(wxT("%d, "), item->second.SysStatus));  //1��ʾϵͳ������2��ʾϵͳ���ã�3��ʾϵͳ��Ҫά����4��ʾϵͳ���ϲ��ܹ�����5��ʾϵͳ�ػ�����STANDBY��
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_REFL_AMP[0])); //����������ֵ
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
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SYSSPLN));              //������ϵͳ�궨����
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SYSLPLN));              //������ϵͳ�궨����
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[0])); //�ٶ�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[1])); //�ٶ�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[2])); //�ٶ�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_VELOCITY[3])); //�ٶ�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[0])); //�ٶȲ���ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[1])); //�ٶȲ���ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[2])); //�ٶȲ���ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_VELOCITY[3])); //�ٶȲ���ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[0]));    //�׿�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[1]));    //�׿�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[2]));    //�׿�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_WIDTH[3]));    //�׿�����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[0]));    //�׿����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[1]));    //�׿����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[2]));    //�׿����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_WIDTH[3]));    //�׿����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.SHELTER_TEMP)); 	    //�����¶�        
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.XMTR_AIR_TEMP));        //����������¶�  
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.RADOME_TEMP));          //�������¶�      
		      	sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[0]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[1]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[2]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[3]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[4]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.EXPECTED_RF8_AMP[5]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[0]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[1]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[2]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[3]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[4]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.MEASURED_RF8_AMP[5]));  //KD�궨����ֵ
				sqlConSentence.Append(wxString::Format(wxT("%f, "), item->second.UNFILTERED_LIN_CHAN_PWR)); //�˲�ǰ����
				sqlConSentence.Append(wxString::Format(wxT("%f)"), item->second.FILTERED_LIN_CHAN_PWR));   //�˲�����

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
