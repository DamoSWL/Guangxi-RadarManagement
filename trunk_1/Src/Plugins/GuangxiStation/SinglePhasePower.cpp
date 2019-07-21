#include "SinglePhasePower.h"

static const int CMDTIMERID = 125;
static const int WRITETIMERID = 126; 


BEGIN_EVENT_TABLE(CSinglePhasePower,wxEvtHandler)
    EVT_TIMER(WRITETIMERID,CSinglePhasePower::onwriteDataTimeout)
    EVT_TIMER(CMDTIMERID,CSinglePhasePower::onCmdTimeout)
END_EVENT_TABLE()

CSinglePhasePower::CSinglePhasePower(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{

}


CSinglePhasePower::~CSinglePhasePower(void)
{
}

void CSinglePhasePower::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newSinglePhaseFlag[*index] = false;
	}
	

}

void CSinglePhasePower::processCmd(const char* pData,unsigned int len)
{
	if((len == 37) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			UINT current = 0;
			std::memcpy((char*)&current,pData + 7,4);
			current = ntohl(current);

			m_newSinglePhaseFlag[id] = true;
			m_singlePhaseParam[id] = current;
		}
	}
}

int CSinglePhasePower::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
		return 0;
}

void CSinglePhasePower::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CSinglePhasePower::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CSinglePhasePower::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CSinglePhasePower::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CSinglePhasePower::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CSinglePhasePower::sendSinglePhaseCmd(UINT id)
{
	UCHAR singlePhaseCmd[8] = {'\0'};
	singlePhaseCmd[0] = id;
	singlePhaseCmd[1] = '\x03';
	singlePhaseCmd[2] = '\x00';
	singlePhaseCmd[3] = '\x48';
	singlePhaseCmd[4] = '\x00';
	singlePhaseCmd[5] = '\x08';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(singlePhaseCmd,6);

	singlePhaseCmd[6] = CRCCheckData.crcByte[1];
	singlePhaseCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(SinglePhasePower,(const char*)&singlePhaseCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send single phase");
}

void CSinglePhasePower::onwriteDataTimeout(wxTimerEvent& event)
{
    m_cmdTimer.Stop();
	writeToDataBase();
	
	for(auto item = m_newSinglePhaseFlag.begin(); item != m_newSinglePhaseFlag.end(); item++)
	{
		item->second = false;
	}
    
    m_cmdTimer.Start(LTIMEINTERVAL);
}

void CSinglePhasePower::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	
	for(auto item = m_newSinglePhaseFlag.cbegin(); item != m_newSinglePhaseFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendSinglePhaseCmd(item->first);
		}
		::wxMilliSleep(200);
	}
	
	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CSinglePhasePower::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO radar_power_single_supply (updateTime,\
																			radarcd, \
																			equipcode, \
																			electricity) VALUES");
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
				if(m_newSinglePhaseFlag[*id])
				{
					auto sqlSentenceSingle = sqlSentence;
					sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f)"),m_singlePhaseParam[*id] / 10000.0));


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
