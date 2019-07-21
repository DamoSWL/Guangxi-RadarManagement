#include "SelfDefAnalogDataThree.h"



static const int CMDTIMERID = 246;
static const int WRITETIMERID = 247; 


BEGIN_EVENT_TABLE(CSelfDefAnalogDataThree,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CSelfDefAnalogDataThree::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CSelfDefAnalogDataThree::onCmdTimeout)
END_EVENT_TABLE()

CSelfDefAnalogDataThree::CSelfDefAnalogDataThree(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{
}


CSelfDefAnalogDataThree::~CSelfDefAnalogDataThree(void)
{
}


void CSelfDefAnalogDataThree::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newUPSDataFlag[*index] = false;
	}
}

void CSelfDefAnalogDataThree::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

int CSelfDefAnalogDataThree::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CSelfDefAnalogDataThree::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CSelfDefAnalogDataThree::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CSelfDefAnalogDataThree::sendUPSCmd(UINT id)
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
		UPSCmd[7] = GetPlug()->HexToASCII(0xE3).first;
		UPSCmd[8] = GetPlug()->HexToASCII(0xE3).second;
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

		GetPlug()->sendCmd(UPSExtendAnalogDataThree,(const char*)&UPSCmd,18,m_IPAddress.c_str(),UPSPORT);

		//GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send ups selfdef data three");
	}
}

void CSelfDefAnalogDataThree::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();

	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newUPSDataFlag.begin(); item != m_newUPSDataFlag.end(); item++)
	{
		item->second = false;
	}
}

void CSelfDefAnalogDataThree::onCmdTimeout(wxTimerEvent& event)
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

bool CSelfDefAnalogDataThree::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO ups_info (radarcd, \
		updateTime,\
		equipcode, \
        m1_output_reactive_power_a, \
        m1_output_reactive_power_b, \
        m1_output_reactive_power_c, \
        m1_sys_output_active_power_a, \
        m1_sys_output_active_power_b, \
        m1_sys_output_active_power_c, \
        m1_sys_output_apparent_power_a, \
        m1_sys_output_apparent_power_b, \
        m1_sys_output_apparent_power_c, \
        m1_sys_output_reactive_power_a, \
        m1_sys_output_reactive_power_b, \
        m1_sys_output_reactive_power_c, \
        m1_attery_backup_time, \
        m1_attery_tempereture, \
        m1_enviro_tempereture, \
        m1_attery_old_ratio) VALUES");
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
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "),wxT("E3")));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_reactive_power_a));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_reactive_power_b));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_output_reactive_power_c));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_active_power_a));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_active_power_b));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_active_power_c));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_apparent_power_a));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_apparent_power_b));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_apparent_power_c));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_reactive_power_a));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_reactive_power_b));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_sys_output_reactive_power_c));

                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_attery_backup_time));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_attery_tempereture));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_UPSParam[*id].m1_enviro_tempereture));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f)"),m_UPSParam[*id].m1_attery_old_ratio));

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

bool CSelfDefAnalogDataThree::isCheckSumValid(const char* pData,UINT Len)
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

void CSelfDefAnalogDataThree::processCmd(const char* pData,unsigned int len)
{
	if(((UCHAR)pData[0] == UPSSOI) && ((UCHAR)pData[len -1] == UPSEOI) && (len >= 150))
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
							m_UPSParam[id].m1_output_reactive_power_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 17));
							m_UPSParam[id].m1_output_reactive_power_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 25));
							m_UPSParam[id].m1_output_reactive_power_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 33));
							m_UPSParam[id].m1_sys_output_active_power_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 41));
							m_UPSParam[id].m1_sys_output_active_power_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 49));
							m_UPSParam[id].m1_sys_output_active_power_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 57));
							m_UPSParam[id].m1_sys_output_apparent_power_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 65));
							m_UPSParam[id].m1_sys_output_apparent_power_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 73));
							m_UPSParam[id].m1_sys_output_apparent_power_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 81));
							m_UPSParam[id].m1_sys_output_reactive_power_a = GetPlug()->UCHARToFloat((PUCHAR)(pData + 89));
							m_UPSParam[id].m1_sys_output_reactive_power_b = GetPlug()->UCHARToFloat((PUCHAR)(pData + 97));
							m_UPSParam[id].m1_sys_output_reactive_power_c = GetPlug()->UCHARToFloat((PUCHAR)(pData + 105));
                            m_UPSParam[id].m1_attery_backup_time = GetPlug()->UCHARToFloat((PUCHAR)(pData + 113));
                            m_UPSParam[id].m1_attery_tempereture = GetPlug()->UCHARToFloat((PUCHAR)(pData + 121));
                            m_UPSParam[id].m1_enviro_tempereture = GetPlug()->UCHARToFloat((PUCHAR)(pData + 129));
                            m_UPSParam[id].m1_attery_old_ratio = GetPlug()->UCHARToFloat((PUCHAR)(pData + 137));

						}
						else
						{
							GetPlug()->AddLog(LOG_TYPE_ERROR,"ups self data three wrong RTN");
						}

					}

				}
			}

		}

	}
}

void CSelfDefAnalogDataThree::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CSelfDefAnalogDataThree::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}
