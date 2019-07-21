#include "SixRoute.h"

static const int CMDTIMERID = 115;
static const int WRITETIMERID = 116; 

BEGIN_EVENT_TABLE(CSixRoute,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CSixRoute::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CSixRoute::onCmdTimeout)
END_EVENT_TABLE()

CSixRoute::CSixRoute(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{

}


CSixRoute::~CSixRoute(void)
{
}

void CSixRoute::setID(const std::set<UINT>& id)
{
	m_IDSet = id;
	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newSixRouteFlag[*index] = false;
	}
	
}

void CSixRoute::processCmd(const char* pData,unsigned int len)
{
	if((len == 21) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			std::map<UINT,UINT> sixRouteParam;
			for(auto index = 1; index <= 6; index++)
			{
				sixRouteParam[index] = pData[2 * index + 6];

			}

			m_newSixRouteFlag[id] = true;
			m_sixRouteParam[id] = sixRouteParam;
		}
	}

	if((len == 89) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			std::map<UINT,SixRouteParam> newsixRouteParam;
			for(auto index = 1; index <= 6; index++)
			{
				auto voltage = ((UINT)pData[14*index - 11] << 8) + (UINT)(pData[14*index - 10]);
				auto current = ((UINT)pData[14*index - 9] << 8) + (UINT)(pData[14*index - 8]);
				auto activePower = ((UINT)pData[14*index - 7] << 8) + (UINT)(pData[14*index - 6]);
				auto activeTotalPower = ((UINT)pData[14*index - 5] << 24) + ((UINT)pData[14*index - 4] << 16) + ((UINT)pData[14*index - 3] << 8) + (UINT)pData[14*index - 2];
				auto powerFacotr = ((UINT)pData[14*index - 1] << 8) + (UINT)pData[14*index - 0];
				auto frequency = ((UINT)pData[14*index + 1]) + (UINT)pData[14*index + 2];
				
				SixRouteParam tmpSixRouteParam;
				tmpSixRouteParam.voltage = voltage / 100.0;
				tmpSixRouteParam.current = current / 1000.0;
				tmpSixRouteParam.activePower = activePower * 1.0;
				tmpSixRouteParam.activeTotalPower = activeTotalPower / 100.0;
				tmpSixRouteParam.powerFactor = powerFacotr / 1000.0;
				tmpSixRouteParam.frequency = frequency / 100.0;

				m_newSixRouteFlag[id] = true;
				newsixRouteParam[index] = tmpSixRouteParam;

			}
		}
	}
}

int CSixRoute::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
		return 0;
}

void CSixRoute::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CSixRoute::startTimer(int interval)
{
	 m_cmdTimer.Start(interval);
}

void CSixRoute::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CSixRoute::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CSixRoute::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CSixRoute::sendSixRouteCmd(UINT id)
{
	UCHAR sixRouteCmd[8] = {'\0'};
	sixRouteCmd[0] = id;
	sixRouteCmd[1] = '\x03';
	sixRouteCmd[2] = '\x00';
	sixRouteCmd[3] = '\x00';
	sixRouteCmd[4] = '\x00';
	sixRouteCmd[5] = '\x08';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(sixRouteCmd,6);

	sixRouteCmd[6] = CRCCheckData.crcByte[1];
	sixRouteCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(SixRoute,(const char*)&sixRouteCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send six route");
}

void CSixRoute::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();

	for(auto item = m_newSixRouteFlag.begin(); item != m_newSixRouteFlag.end(); item++)
	{
		item->second = false;
	}

	m_cmdTimer.Start(LTIMEINTERVAL);
}

void CSixRoute::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();

	for(auto item = m_newSixRouteFlag.cbegin(); item != m_newSixRouteFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendSixRouteCmd(item->first);
		}
		::wxMilliSleep(200);
	}

	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CSixRoute::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO radar_six_route (updateTime,\
							   radarcd, \
							   equipcode, \
							   way1, \
							   way2, \
							   way3, \
							   way4, \
							   way5, \
							   way6) VALUES");
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
				if(m_newSixRouteFlag[*id])
				{
					auto sqlSentenceSingle = sqlSentence;
					sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_sixRouteParam[*id][1]));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_sixRouteParam[*id][2]));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_sixRouteParam[*id][3]));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_sixRouteParam[*id][4]));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_sixRouteParam[*id][5]));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d)"), m_sixRouteParam[*id][6]));

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


	wxString newSqlSentence = wxT("INSERT INTO radar_six_route_info (updateTime,\
		radarcd, \
		equipcode, \
		voltage_1, \
		current_1, \
		power_1, \
		totalenergy_1, \
		powerfactor_1, \
		frequency_1, \
		voltage_2, \
		current_2, \
		power_2, \
		totalenergy_2, \
		powerfactor_2, \
		frequency_2, \
		voltage_3, \
		current_3, \
		power_3, \
		totalenergy_3, \
		powerfactor_3, \
		frequency_3, \
		voltage_4, \
		current_4, \
		powerfactor_4, \
		power_4, \
		totalenergy_4, \
		frequency_4, \
		voltage_5, \
		current_5, \
		power_5, \
		totalenergy_5, \
		powerfactor_5, \
		frequency_5, \
		voltage_6, \
		current_6, \
		power_6, \
		totalenergy_6, \
		powerfactor_6, \
		frequency_6) VALUES");


	if(pDataBase)
	{
		try
		{

			for(auto id = m_IDSet.cbegin(); id != m_IDSet.cend(); id++)
			{
				if(m_newSixRouteFlag[*id])
				{
					
					auto newSqlSentenceSingle = newSqlSentence;
					newSqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
					newSqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					newSqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

					auto item = m_newSixRouteParam[*id];
					for(auto newSixParam = item.cbegin(); newSixParam != item.cend(); newSixParam++)
					{
						newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.voltage));
						newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.current));
						newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.activePower));
						newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.activeTotalPower));
						newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.powerFactor));
						if(newSixParam->first < 6)
						{
							newSqlSentenceSingle.Append(wxString::Format(wxT("%f, "),newSixParam->second.frequency));
						}
						else
						{
							newSqlSentenceSingle.Append(wxString::Format(wxT("%f)"),newSixParam->second.frequency));
						}
						
					}

					if(!pDataBase->ExecuteNoQuery(newSqlSentenceSingle,sqlError))
					{
						throw sqlError;
					}



				}

			}

		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}

	}

	return false;


}
