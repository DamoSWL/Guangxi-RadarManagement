
#include "ThreePhasePower.h"

static const int CMDTIMERID = 105;
static const int WRITETIMERID = 106; 


BEGIN_EVENT_TABLE(CThreePhasePower,wxEvtHandler)
    EVT_TIMER(WRITETIMERID,CThreePhasePower::onwriteDataTimeout)
    EVT_TIMER(CMDTIMERID,CThreePhasePower::onCmdTimeout)
END_EVENT_TABLE()

CThreePhasePower::CThreePhasePower(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{


}


CThreePhasePower::~CThreePhasePower(void)
{
	//for(auto item = m_ThreePhaseAlarm.begin(); item != m_ThreePhaseAlarm.end(); item++)
	//{
	//	delete m_ThreePhaseAlarm[item->first];
	//	item->second = NULL;
	//}
}

void CThreePhasePower::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CThreePhasePower::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newThreePhaseFlag[*index] = false;
		//m_ThreePhaseAlarm[*index] = new CThreePhaseAlarm;
	}
}

void CThreePhasePower::processCmd(const char* pData,unsigned int len)
{
	if((len == 65) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			ThreePhaseParam param;
			std::memcpy((void*)&param,pData+3,60);

			param.APhasePower = ntohs(param.APhasePower);
			param.APhaseCurrent = ntohs(param.APhaseCurrent);
			param.APhasePowerRatio = ntohs(param.APhasePowerRatio);

			param.BPhasePower = ntohs(param.BPhasePower);
			param.BPhaseCurrent = ntohs(param.BPhaseCurrent);
			param.BPhasePowerRatio = ntohs(param.BPhasePowerRatio);

			param.CPhasePower = ntohs(param.CPhasePower);
			param.CPhaseCurrent = ntohs(param.CPhaseCurrent);
			param.CPhasePowerRatio = ntohs(param.CPhasePowerRatio);

			param.frequency = ntohs(param.frequency);

			m_newThreePhaseFlag[id] = true;
			m_threePhaseParam[id] = param;
		}
	}
}

int CThreePhasePower::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CThreePhasePower::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}

void CThreePhasePower::startTimer(int interval)
{
    m_cmdTimer.Start(interval);
}

void CThreePhasePower::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CThreePhasePower::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}


void CThreePhasePower::sendThreePhaseCmd(UINT id)
{
	UCHAR threePhaseCmd[8] = {'\0'};
	threePhaseCmd[0] = id;
	threePhaseCmd[1] = '\x03';
	threePhaseCmd[2] = '\x00';
	threePhaseCmd[3] = '\x48';
	threePhaseCmd[4] = '\x00';
	threePhaseCmd[5] = '\x1e';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(threePhaseCmd,6);

	threePhaseCmd[6] = CRCCheckData.crcByte[1];
	threePhaseCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(ThreePhasePower,(const char*)&threePhaseCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send three phase");
	
}

void CThreePhasePower::onwriteDataTimeout(wxTimerEvent& event)
{
    m_cmdTimer.Stop();
	writeToDataBase();
	
	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newThreePhaseFlag.begin(); item != m_newThreePhaseFlag.end(); item++)
	{
		//if(item->second)
		//{
		//	auto id = item->first;
		
			//m_ThreePhaseAlarm[id]->getAlarmType();
			//m_ThreePhaseAlarm[id]->getThresholdValue();
			//m_ThreePhaseAlarm[id]->getThreePhaseValue(m_threePhaseParam[id]);
			//m_ThreePhaseAlarm[id]->ThreePhaseCheck();
		//}
		item->second = false;
	}

}

void CThreePhasePower::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();

	for(auto item = m_newThreePhaseFlag.cbegin(); item != m_newThreePhaseFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendThreePhaseCmd(item->first);
		}
		::wxMilliSleep(200);
	}

	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();

}

bool CThreePhasePower::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO radar_power_supply (updateTime,\
																	radarcd, \
																	equipcode, \
																	avoltage, \
																	aelectricity, \
																	apower, \
																	bvoltage, \
																	belectricity, \
																	bpower, \
																	cvoltage, \
																	celectricity, \
																	cpower, \
																	frequency) VALUES");
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
				if(m_newThreePhaseFlag[*id])
				{
					auto sqlSentenceSingle = sqlSentence;
					sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].APhasePower / 100.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].APhaseCurrent / 1000.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].APhasePowerRatio / 1.0));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].BPhasePower / 100.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].BPhaseCurrent / 1000.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].BPhasePowerRatio / 1.0));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].CPhasePower / 100.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].CPhaseCurrent / 1000.0));
					sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),m_threePhaseParam[*id].CPhasePowerRatio / 1.0));

					sqlSentenceSingle.Append(wxString::Format(wxT("%f)"),m_threePhaseParam[*id].frequency / 100.0));
		

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
