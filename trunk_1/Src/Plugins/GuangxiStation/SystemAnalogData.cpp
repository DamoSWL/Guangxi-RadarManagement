#include "SystemAnalogData.h"

static const int CMDTIMERID = 214;
static const int WRITETIMERID = 216; 


BEGIN_EVENT_TABLE(CSystemAnalogData,wxEvtHandler)
    EVT_TIMER(WRITETIMERID,CSystemAnalogData::onwriteDataTimeout)
    EVT_TIMER(CMDTIMERID,CSystemAnalogData::onCmdTimeout)
END_EVENT_TABLE()

CSystemAnalogData::CSystemAnalogData(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{
}


CSystemAnalogData::~CSystemAnalogData(void)
{
}


void CSystemAnalogData::setID(const std::set<UINT>& id)
{
	m_IDSet = id;
	
	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newUPSDataFlag[*index] = false;
	}
}

void CSystemAnalogData::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

int CSystemAnalogData::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CSystemAnalogData::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CSystemAnalogData::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CSystemAnalogData::sendUPSCmd(UINT id)
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
		UPSCmd[7] = GetPlug()->HexToASCII(0x41).first;
		UPSCmd[8] = GetPlug()->HexToASCII(0x41).second;
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

		GetPlug()->sendCmd(UPSSystemAnalogData,(const char*)&UPSCmd,18,m_IPAddress.c_str(),UPSPORT);

		GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send ups system data");
	}
	

}

void CSystemAnalogData::onwriteDataTimeout(wxTimerEvent& event)
{
    m_cmdTimer.Stop();
	writeToDataBase();
	
	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newUPSDataFlag.begin(); item != m_newUPSDataFlag.end(); item++)
	{
		item->second = false;
	}
}

void CSystemAnalogData::onCmdTimeout(wxTimerEvent& event)
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

bool CSystemAnalogData::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO ups_info (radarcd, \
		updateTime,\
		equipcode, \
		m1_input_voltage_ab, \
		m1_input_voltage_bc, \
		m1_input_voltage_ca, \
		m1_output_voltage_a, \
		m1_output_voltage_b, \
		m1_output_voltage_c, \
		m1_output_electricity_a, \
		m1_output_electricity_b, \
		m1_output_electricity_c, \
		m1_direct_current_voltage, \
		m1_output_frequency) VALUES");
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
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "),wxT("E0")));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_input_voltage_ab));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_input_voltage_bc));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_input_voltage_ca));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_voltage_a));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_voltage_b));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_voltage_c));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_electricity_a));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_electricity_b));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_electricity_c));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_direct_current_voltage));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f)"),m_UPSParam[*id].m1_output_frequency));

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



bool CSystemAnalogData::isCheckSumValid(const char* pData,UINT Len)
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

void CSystemAnalogData::processCmd(const char* pData,unsigned int len)
{
	if(((UCHAR)pData[0] == UPSSOI) && ((UCHAR)pData[len -1] == UPSEOI) && (len >= 114))
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
	
							m_UPSParam[id].m1_input_voltage_ab = GetPlug()->UCHARToFloat((PUCHAR)(pData + 15));
							m_UPSParam[id].m1_input_voltage_bc = GetPlug()->UCHARToFloat((PUCHAR)(pData + 23));
							m_UPSParam[id].m1_input_voltage_ca = GetPlug()->UCHARToFloat((PUCHAR)(pData + 31));
							m_UPSParam[id].m1_output_voltage_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 39));
							m_UPSParam[id].m1_output_voltage_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 47));
							m_UPSParam[id].m1_output_voltage_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 55));
							m_UPSParam[id].m1_output_electricity_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 63));
							m_UPSParam[id].m1_output_electricity_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 71));
							m_UPSParam[id].m1_output_electricity_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 79));
							m_UPSParam[id].m1_direct_current_voltage = GetPlug()->UCHARToFloat((PUCHAR)(pData + 87));
							m_UPSParam[id].m1_output_frequency = GetPlug()->UCHARToFloat((PUCHAR)(pData + 95));
	
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

void CSystemAnalogData::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CSystemAnalogData::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}
