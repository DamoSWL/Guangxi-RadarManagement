#include "SwitchData.h"

static const int CMDTIMERID = 251;
static const int WRITETIMERID = 258; 


BEGIN_EVENT_TABLE(CSwitchData,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CSwitchData::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CSwitchData::onCmdTimeout)
END_EVENT_TABLE()

CSwitchData::CSwitchData(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{
}


CSwitchData::~CSwitchData(void)
{
}


void CSwitchData::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newUPSDataFlag[*index] = false;
	}
}

void CSwitchData::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

int CSwitchData::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CSwitchData::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CSwitchData::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CSwitchData::sendUPSCmd(UINT id)
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
		UPSCmd[7] = GetPlug()->HexToASCII(0x43).first;
		UPSCmd[8] = GetPlug()->HexToASCII(0x43).second;
		UPSCmd[9] = 0x30;
		UPSCmd[10] = 0x30;
		UPSCmd[11] = 0x30;
		UPSCmd[12] = 0x30;

		auto checkSum = 0;
		for(auto index = 1; index <= 12; index++)
		{
			checkSum += UPSCmd[index];
		}

		checkSum %= 0x10000;
		checkSum = ~checkSum + 1;

		UCHAR checkSumUpperPart = checkSum >> 8;
		UCHAR checkSumLowerPart = checkSum & 0x00FF;

		UPSCmd[13] = GetPlug()->HexToASCII(checkSumUpperPart).first;
		UPSCmd[14] = GetPlug()->HexToASCII(checkSumUpperPart).second;
		UPSCmd[15] = GetPlug()->HexToASCII(checkSumLowerPart).first;
		UPSCmd[16] = GetPlug()->HexToASCII(checkSumLowerPart).second;
		UPSCmd[17] = UPSEOI;

		GetPlug()->sendCmd(UPSSwitchState,(const char*)&UPSCmd,18,m_IPAddress.c_str(),UPSPORT);

		//GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send ups switch data");
	}
}

void CSwitchData::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();

	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newUPSDataFlag.begin(); item != m_newUPSDataFlag.end(); item++)
	{
		item->second = false;
	}
}

void CSwitchData::onCmdTimeout(wxTimerEvent& event)
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

bool CSwitchData::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO ups_info (radarcd, \
		updateTime,\
		equipcode, \
		m2_power_supply_mode, \
		m2_attery_self_checking, \
		m2_evenfloat_charge, \
		m2_shutdown, \
		m2_ups_supplytype, \
		m2_generator_access, \
		m2_entry_switch, \
		m2_maintain_bypass_switch, \
		m2_bypass_shutdown, \
		m2_output_switch, \
		m2_weaver_system_supply, \
		m2_rotary_switch, \
		m2_rejector) VALUES");
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
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "),wxT("E4")));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_power_supply_mode));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_attery_self_checking));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_evenfloat_charge));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_shutdown));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_ups_supplytype));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_generator_access));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_entry_switch));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_maintain_bypass_switch));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_bypass_shutdown));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_output_switch));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_weaver_system_supply));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_UPSParam[*id].m2_rotary_switch));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d)"),m_UPSParam[*id].m2_rejector));

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

bool CSwitchData::isCheckSumValid(const char* pData,UINT Len)
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

void CSwitchData::processCmd(const char* pData,unsigned int len)
{
	if(((UCHAR)pData[0] == UPSSOI) && ((UCHAR)pData[len -1] == UPSEOI) && (len >= 48))
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
							m_UPSParam[id].m2_power_supply_mode = GetPlug()->ASCIIToHex(std::make_pair(pData[15],pData[16]));
							m_UPSParam[id].m2_attery_self_checking = GetPlug()->ASCIIToHex(std::make_pair(pData[19],pData[20]));
							m_UPSParam[id].m2_evenfloat_charge = GetPlug()->ASCIIToHex(std::make_pair(pData[21],pData[22]));
							m_UPSParam[id].m2_shutdown = GetPlug()->ASCIIToHex(std::make_pair(pData[23],pData[24]));
							m_UPSParam[id].m2_ups_supplytype = GetPlug()->ASCIIToHex(std::make_pair(pData[25],pData[26]));
							m_UPSParam[id].m2_generator_access = GetPlug()->ASCIIToHex(std::make_pair(pData[27],pData[28]));
							m_UPSParam[id].m2_entry_switch = GetPlug()->ASCIIToHex(std::make_pair(pData[29],pData[30]));
							m_UPSParam[id].m2_maintain_bypass_switch = GetPlug()->ASCIIToHex(std::make_pair(pData[31],pData[32]));
							m_UPSParam[id].m2_bypass_shutdown = GetPlug()->ASCIIToHex(std::make_pair(pData[33],pData[34]));
							m_UPSParam[id].m2_output_switch = GetPlug()->ASCIIToHex(std::make_pair(pData[35],pData[36]));
							m_UPSParam[id].m2_weaver_system_supply = GetPlug()->ASCIIToHex(std::make_pair(pData[37],pData[38]));
							m_UPSParam[id].m2_rotary_switch = GetPlug()->ASCIIToHex(std::make_pair(pData[39],pData[40]));
							m_UPSParam[id].m2_rejector = GetPlug()->ASCIIToHex(std::make_pair(pData[41],pData[42]));

						}
						else
						{
							GetPlug()->AddLog(LOG_TYPE_ERROR,"ups switch data wrong RTN");
						}

					}

				}
			}

		}

	}
}

void CSwitchData::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CSwitchData::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}
