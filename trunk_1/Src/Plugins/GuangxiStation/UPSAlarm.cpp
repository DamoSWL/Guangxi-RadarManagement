#include "UPSAlarm.h"

static const int CMDTIMERID = 263;
static const int WRITETIMERID = 267; 


BEGIN_EVENT_TABLE(CUPSAlarm,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CUPSAlarm::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CUPSAlarm::onCmdTimeout)
END_EVENT_TABLE()

CUPSAlarm::CUPSAlarm(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{
}


CUPSAlarm::~CUPSAlarm(void)
{
}


void CUPSAlarm::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newUPSDataFlag[*index] = false;
	}
}

void CUPSAlarm::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

int CUPSAlarm::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CUPSAlarm::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CUPSAlarm::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CUPSAlarm::sendUPSCmd(UINT id)
{
	UCHAR UPSCmd[18] = {'\0'};
	UPSCmd[0] = UPSSOI;
	UPSCmd[1] = GetPlug()->HexToASCII(0x11).first;
	UPSCmd[2] = GetPlug()->HexToASCII(0x11).second;

	for(auto id = m_IDSet.cbegin(); id != m_IDSet.cend(); id++)
	{
		UPSCmd[3] = GetPlug()->HexToASCII(*id).first;
		UPSCmd[4] = GetPlug()->HexToASCII(*id).second;
		UPSCmd[5] = GetPlug()->HexToASCII(0x2A).first;
		UPSCmd[6] = GetPlug()->HexToASCII(0x2A).second;
		UPSCmd[7] = GetPlug()->HexToASCII(0x44).first;
		UPSCmd[8] = GetPlug()->HexToASCII(0x44).second;
		UPSCmd[9] = 0x30;
		UPSCmd[10 ]= 0x30;
		UPSCmd[11 ]= 0x30;
		UPSCmd[12 ]= 0x30;

		auto checkSum = 0;
		for(auto index = 1; index <= 12; index++)
		{
			checkSum += UPSCmd[index];
		}

		checkSum %= 0x10000;
		checkSum = ~checkSum + 1;

		UCHAR checkSumUpperPart = checkSum >> 8;
		UCHAR checkSumLowerPart = checkSum & 0x00FF;

		UPSCmd[13 ]= GetPlug()->HexToASCII(checkSumUpperPart).first;
		UPSCmd[14 ]= GetPlug()->HexToASCII(checkSumUpperPart).second;
		UPSCmd[15] = GetPlug()->HexToASCII(checkSumLowerPart).first;
		UPSCmd[16] = GetPlug()->HexToASCII(checkSumLowerPart).second;
		UPSCmd[17] = UPSEOI;

		GetPlug()->sendCmd(UPSAlarmState,(const char*)&UPSCmd,18,m_IPAddress.c_str(),UPSPORT);

		//GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send ups switch data");
	}
}

void CUPSAlarm::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();

	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newUPSDataFlag.begin(); item != m_newUPSDataFlag.end(); item++)
	{
		item->second = false;
	}
}

void CUPSAlarm::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();

	for(auto item = m_newUPSDataFlag.cbegin(); item != m_newUPSDataFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendUPSCmd(item->first);
		}
		::wxMilliSleep(200);
	}

	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CUPSAlarm::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO ups_info (radarcd, \
		updateTime,\
		equipcode, \
		m3_inverter_sync, \
		m3_main_road_voltage_abnormal, \
		m3_rectifier_blockade, \
		m3_abnormal_output_voltage, \
		m3_bypass_situation, \
		m3_total_battery_voltage_state, \
		m3_main_road_frequency_abnormal, \
		m3_main_road_fuse_broken, \
		m3_main_path_reversed, \
		m3_main_phase_alarm, \
		m3_auxiliary_power1_poweroff, \
		m3_auxiliary_power2_poweroff, \
		m3_rectify_current_limiting, \
		m3_soft_start_failure, \
		m3_rectify_over_temp, \
		m3_input_filter_alarm, \
		m3_filter_flow, \
		m3_filter_contactor_alarm, \
		m3_rectify_drives_cable_alarm, \
		m3_rectify_commun_alarm, \
		m3_inverter_overheats, \
		m3_fan_alarm, \
		m3_inverter_thyristor_alarm, \
		m3_bypass_thyristor_alarm, \
		m3_user_opre_error, \
		m3_single_output_overload, \
		m3_parallel_overloaded, \
		m3_single_overload_timeout, \
		m3_bypass_abnormal_shutdown, \
		m3_output_over_voltage, \
		m3_inverter_overflows, \
		m3_bypass_phase_reversed, \
		m3_load_shock_bypass, \
		m3_bypass_switch_times_limit, \
		m3_weaver_flow_alarm, \
		m3_bus_abnormal_shutdown, \
		m3_neighbor_machine_request_bypass, \
		m3_weaver_plate__alarm, \
		m3_parallel_connection_alarm, \
		m3_parallel_machine_communi_alarm, \
		m3_bypass_out_alarm, \
		m3_lbs_activate_alarm, \
		m3_bypass_induces_over_temperature, \
		m3_static_switch_over_temperature, \
		m3_bypass_reverse_irrigation_alarm, \
		m3_inverter_drives_cable_alarm, \
		m3_contravariant_communi_alarm, \
		m3_parallel_sys_battery_alarm, \
		m3_emergency_switch, \
		m3_env_excess_temperature, \
		m3_storage_battery_life, \
		m3_battery_excess_temperature, \
		m3_battery_ground_alarm, \
		m3_battery_fuse, \
		m3_bcb_access_situation, \
		m3_output_fuse_broken, \
		m3_capacitance_over_voltage, \
		m3_bus_over_voltage, \
		m3_busbar_short_circuit_alarm, \
		m3_input_electricity_unevenness, \
		m3_output_capacitance_maintain, \
		m3_filter_frequency) VALUES");
	wxString sqlError;
	MySqlDatabase* pDataBase = GetPlug()->getDatabase();
	wxMutex* sqlLocker = GetPlug()->getSqlMutex();
	wxMutexLocker locker(*sqlLocker);

	if(pDataBase)
	{
		try
		{

			for(auto id = m_IDSet.cbegin(); id != m_IDSet.cend(); id++)
			{
				if(m_newUPSDataFlag[*id])
				{
					auto sqlSentenceSingle = sqlSentence;
					sqlSentenceSingle.Append(wxString::Format(wxT("('%s', "),wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),::wxGetUTCTime()));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "),wxT("E5")));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_inverter_sync));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_main_road_voltage_abnormal));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_rectifier_blockade));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_abnormal_output_voltage));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_situation));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_total_battery_voltage_state));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_main_road_frequency_abnormal));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_main_road_fuse_broken));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_main_path_reversed));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_main_phase_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_auxiliary_power1_poweroff));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_auxiliary_power2_poweroff));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_rectify_current_limiting));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_soft_start_failure));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_rectify_over_temp));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_input_filter_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_filter_flow));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_filter_contactor_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_rectify_drives_cable_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_rectify_commun_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_inverter_overheats));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_fan_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_inverter_thyristor_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_thyristor_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_user_opre_error));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_single_output_overload));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_parallel_overloaded));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_single_overload_timeout));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_abnormal_shutdown));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_output_over_voltage));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_inverter_overflows));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_phase_reversed));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_load_shock_bypass));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_switch_times_limit));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_weaver_flow_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bus_abnormal_shutdown));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_neighbor_machine_request_bypass));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_weaver_plate__alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_parallel_connection_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_parallel_machine_communi_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_out_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_lbs_activate_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_induces_over_temperature));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_static_switch_over_temperature));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bypass_reverse_irrigation_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_inverter_drives_cable_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_contravariant_communi_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_parallel_sys_battery_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_emergency_switch));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_env_excess_temperature));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_storage_battery_life));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_battery_excess_temperature));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_battery_ground_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_battery_fuse));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bcb_access_situation));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_output_fuse_broken));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_capacitance_over_voltage));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_bus_over_voltage));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_busbar_short_circuit_alarm));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_input_electricity_unevenness));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m3_output_capacitance_maintain));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d)"),m_UPSParam[*id].m3_filter_frequency));


					if(!pDataBase->ExecuteNoQuery(sqlSentenceSingle,sqlError))
					{
						throw sqlError;
					}

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

	return false;
}

bool CUPSAlarm::isCheckSumValid(const char* pData,UINT Len)
{
		if(pData)
		{
			auto Checksum = 0;
			for(UINT index = 1; index <= Len - 1 - 5; index++)
			{
				Checksum += (UCHAR)pData[index];
			}

			Checksum %= 0x10000;
			Checksum = ~Checksum + 1;
			UCHAR checkSumUpperPart = Checksum >> 8;
			UCHAR checkSumLowerPart = Checksum & 0x00FF;

			UCHAR checkSumUpperPartInData = GetPlug()->ASCIIToHex(std::make_pair(pData[Len - 1 - 4], pData[Len - 1 - 3]));
			UCHAR checkSumLowerPartInData = GetPlug()->ASCIIToHex(std::make_pair(pData[Len -1 - 2],pData[Len - 1 - 1]));
			if((checkSumUpperPart == checkSumUpperPartInData) && (checkSumLowerPart == checkSumLowerPartInData))
			{
				return true;
			}

		}

		return false;
}

void CUPSAlarm::processCmd(const char* pData,unsigned int len)
{
	if(((UCHAR)pData[0] == UPSSOI) && ((UCHAR)pData[len -1] == UPSEOI) && (len >= 148))
	{
		if(isCheckSumValid(pData,len))
		{
			if(GetPlug()->ASCIIToHex(std::make_pair(pData[1],pData[2])) == 0x11)
			{
				auto id = GetPlug()->ASCIIToHex(std::make_pair(pData[3],pData[4]));
				if(m_IDSet.find(id) != m_IDSet.end())
				{
					m_newUPSDataFlag[id] = true;
					if(GetPlug()->ASCIIToHex(std::make_pair(pData[5],pData[6])) == 0x2A)
					{
						if(GetPlug()->ASCIIToHex(std::make_pair(pData[7],pData[8])) == 0x00)
						{
							m_UPSParam[id].m3_inverter_sync = GetPlug()->ASCIIToHex(std::make_pair(pData[15],pData[16]));
							m_UPSParam[id].m3_main_road_voltage_abnormal = GetPlug()->ASCIIToHex(std::make_pair(pData[17],pData[18]));
							m_UPSParam[id].m3_rectifier_blockade = GetPlug()->ASCIIToHex(std::make_pair(pData[19], pData[20]));
							m_UPSParam[id].m3_abnormal_output_voltage = GetPlug()->ASCIIToHex(std::make_pair(pData[21], pData[22]));
							m_UPSParam[id].m3_bypass_situation = GetPlug()->ASCIIToHex(std::make_pair(pData[23], pData[24]));
							m_UPSParam[id].m3_total_battery_voltage_state = GetPlug()->ASCIIToHex(std::make_pair(pData[25],pData[26]));
							m_UPSParam[id].m3_main_road_frequency_abnormal = GetPlug()->ASCIIToHex(std::make_pair(pData[31],pData[32]));
							m_UPSParam[id].m3_main_road_fuse_broken = GetPlug()->ASCIIToHex(std::make_pair(pData[33],pData[34]));
							m_UPSParam[id].m3_main_path_reversed = GetPlug()->ASCIIToHex(std::make_pair(pData[35],pData[36]));
							m_UPSParam[id].m3_main_phase_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[37],pData[38]));
							m_UPSParam[id].m3_auxiliary_power1_poweroff = GetPlug()->ASCIIToHex(std::make_pair(pData[39],pData[40]));
							m_UPSParam[id].m3_auxiliary_power2_poweroff = GetPlug()->ASCIIToHex(std::make_pair(pData[41],pData[42]));
							m_UPSParam[id].m3_rectify_current_limiting = GetPlug()->ASCIIToHex(std::make_pair(pData[43],pData[44]));

							m_UPSParam[id].m3_soft_start_failure = GetPlug()->ASCIIToHex(std::make_pair(pData[45],pData[46]));
							m_UPSParam[id].m3_rectify_over_temp = GetPlug()->ASCIIToHex(std::make_pair(pData[47],pData[48]));
							m_UPSParam[id].m3_input_filter_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[49],pData[50]));
							m_UPSParam[id].m3_filter_flow = GetPlug()->ASCIIToHex(std::make_pair(pData[51],pData[52]));
							m_UPSParam[id].m3_filter_contactor_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[53],pData[54]));
							m_UPSParam[id].m3_rectify_drives_cable_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[55],pData[56]));
							m_UPSParam[id].m3_rectify_commun_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[57],pData[58]));
							m_UPSParam[id].m3_inverter_overheats = GetPlug()->ASCIIToHex(std::make_pair(pData[59],pData[60]));
							m_UPSParam[id].m3_fan_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[61],pData[62]));
							m_UPSParam[id].m3_inverter_thyristor_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[63],pData[64]));
							m_UPSParam[id].m3_bypass_thyristor_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[65],pData[66]));
							m_UPSParam[id].m3_user_opre_error = GetPlug()->ASCIIToHex(std::make_pair(pData[67],pData[68]));
							m_UPSParam[id].m3_single_output_overload = GetPlug()->ASCIIToHex(std::make_pair(pData[69],pData[70]));
							m_UPSParam[id].m3_parallel_overloaded = GetPlug()->ASCIIToHex(std::make_pair(pData[71],pData[72]));
							m_UPSParam[id].m3_single_overload_timeout = GetPlug()->ASCIIToHex(std::make_pair(pData[73],pData[74]));
							m_UPSParam[id].m3_bypass_abnormal_shutdown = GetPlug()->ASCIIToHex(std::make_pair(pData[75],pData[76]));
							m_UPSParam[id].m3_output_over_voltage = GetPlug()->ASCIIToHex(std::make_pair(pData[77],pData[78]));
							m_UPSParam[id].m3_inverter_overflows = GetPlug()->ASCIIToHex(std::make_pair(pData[79],pData[80]));

							m_UPSParam[id].m3_bypass_phase_reversed = GetPlug()->ASCIIToHex(std::make_pair(pData[81],pData[82]));
							m_UPSParam[id].m3_load_shock_bypass = GetPlug()->ASCIIToHex(std::make_pair(pData[83],pData[84]));
							m_UPSParam[id].m3_bypass_switch_times_limit = GetPlug()->ASCIIToHex(std::make_pair(pData[85],pData[86]));
							m_UPSParam[id].m3_weaver_flow_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[87],pData[88]));
							m_UPSParam[id].m3_bus_abnormal_shutdown = GetPlug()->ASCIIToHex(std::make_pair(pData[89],pData[90]));
							m_UPSParam[id].m3_neighbor_machine_request_bypass = GetPlug()->ASCIIToHex(std::make_pair(pData[91],pData[92]));
							m_UPSParam[id].m3_weaver_plate__alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[93],pData[94]));
							m_UPSParam[id].m3_parallel_connection_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[95],pData[96]));
							m_UPSParam[id].m3_parallel_machine_communi_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[97],pData[98]));
							m_UPSParam[id].m3_bypass_out_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[99],pData[100]));
							m_UPSParam[id].m3_lbs_activate_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[101],pData[102]));
							m_UPSParam[id].m3_bypass_induces_over_temperature = GetPlug()->ASCIIToHex(std::make_pair(pData[103],pData[104]));
							m_UPSParam[id].m3_static_switch_over_temperature = GetPlug()->ASCIIToHex(std::make_pair(pData[105],pData[106]));
							m_UPSParam[id].m3_bypass_reverse_irrigation_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[107],pData[108]));
							m_UPSParam[id].m3_inverter_drives_cable_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[109],pData[110]));
							m_UPSParam[id].m3_contravariant_communi_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[111],pData[112]));
							m_UPSParam[id].m3_parallel_sys_battery_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[113],pData[114]));
							m_UPSParam[id].m3_emergency_switch = GetPlug()->ASCIIToHex(std::make_pair(pData[115],pData[116]));
							m_UPSParam[id].m3_env_excess_temperature = GetPlug()->ASCIIToHex(std::make_pair(pData[117],pData[118]));
							m_UPSParam[id].m3_storage_battery_life = GetPlug()->ASCIIToHex(std::make_pair(pData[119],pData[120]));
							m_UPSParam[id].m3_battery_excess_temperature = GetPlug()->ASCIIToHex(std::make_pair(pData[121],pData[122]));
							m_UPSParam[id].m3_battery_ground_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[123],pData[124]));
							m_UPSParam[id].m3_battery_fuse = GetPlug()->ASCIIToHex(std::make_pair(pData[125],pData[126]));
							m_UPSParam[id].m3_bcb_access_situation = GetPlug()->ASCIIToHex(std::make_pair(pData[127],pData[128]));
							m_UPSParam[id].m3_output_fuse_broken = GetPlug()->ASCIIToHex(std::make_pair(pData[129],pData[130]));
							m_UPSParam[id].m3_capacitance_over_voltage = GetPlug()->ASCIIToHex(std::make_pair(pData[131],pData[132]));
							m_UPSParam[id].m3_bus_over_voltage = GetPlug()->ASCIIToHex(std::make_pair(pData[133],pData[134]));
							m_UPSParam[id].m3_busbar_short_circuit_alarm = GetPlug()->ASCIIToHex(std::make_pair(pData[135],pData[136]));
							m_UPSParam[id].m3_input_electricity_unevenness = GetPlug()->ASCIIToHex(std::make_pair(pData[137],pData[138]));
							m_UPSParam[id].m3_output_capacitance_maintain = GetPlug()->ASCIIToHex(std::make_pair(pData[139],pData[140]));
							m_UPSParam[id].m3_filter_frequency = GetPlug()->ASCIIToHex(std::make_pair(pData[141],pData[142]));


						}
						else
						{
						 	GetPlug()->AddLog(LOG_TYPE_ERROR,"ups system data wrong RTN");
						}

					}

				}
			}

		}

	}
}

void CUPSAlarm::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CUPSAlarm::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}
