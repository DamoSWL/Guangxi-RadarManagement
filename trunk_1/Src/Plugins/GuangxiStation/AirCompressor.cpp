#include "AirCompressor.h"

static const int CMDTIMERID = 155;
static const int WRITETIMERID = 156; 


BEGIN_EVENT_TABLE(CAirCompressor,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CAirCompressor::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CAirCompressor::onCmdTimeout)
END_EVENT_TABLE()

CAirCompressor::CAirCompressor(void):
	m_cmdTimer(this,CMDTIMERID),
	m_writeDataTimer(this,WRITETIMERID)
{

}


CAirCompressor::~CAirCompressor(void)
{
}

void CAirCompressor::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CAirCompressor::setID(const std::set<UINT>& id)
{
	m_IDSet = id;
	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newAirCompressorFlag[*index] = false;
	}
	
}

void CAirCompressor::processCmd(const char* pData,unsigned int len)
{
	if((len == 15) && ((UCHAR)pData[1] == 0x03))
	{
		if(!GetPlug()->crcCheck((PUCHAR)pData,len))
		{
			return;
		}
		UINT id = (UCHAR)pData[0];
		
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			auto pressureAlarm = pData[8];
			auto humidityAlarm = pData[10];

			m_newAirCompressorFlag[id] = true;
			m_AirCompressorParam[id] = make_pair(pressureAlarm,humidityAlarm);
		}

	}
}

int CAirCompressor::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CAirCompressor::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}


void CAirCompressor::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CAirCompressor::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CAirCompressor::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CAirCompressor::sendAirCompressorCmd(UINT id)
{
	UCHAR airCompressorCmd[8] = {'\0'};
	airCompressorCmd[0] = id;
	airCompressorCmd[1] = '\x03';
	airCompressorCmd[2] = '\x00';
	airCompressorCmd[3] = '\x00';
	airCompressorCmd[4] = '\x00';
	airCompressorCmd[5] = '\x0A';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(airCompressorCmd,6);

	airCompressorCmd[6] = CRCCheckData.crcByte[1];
	airCompressorCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(AirCompressor,(const char*)&airCompressorCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send airCompressor");
}

void CAirCompressor::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();
	
	for(auto item = m_newAirCompressorFlag.begin(); item != m_newAirCompressorFlag.end(); item++)
	{
		item->second = false;
	}
    
    m_cmdTimer.Start(LTIMEINTERVAL);
}

void CAirCompressor::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	
	for(auto item = m_newAirCompressorFlag.cbegin(); item != m_newAirCompressorFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendAirCompressorCmd(item->first);
		}
		::wxMilliSleep(200);
	}
	
	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}


bool CAirCompressor::writeToDataBase()
{
	wxString sqlSentence = wxT("INSERT INTO radar_env_aircompressor (radarcd, \
																updateTime,\
																equipcode, \
																pressstatus, \
																humiditystatus) VALUES");

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
                if(m_newAirCompressorFlag[*id])
                {
                    auto sqlSentenceSingle = sqlSentence;
                    sqlSentenceSingle.Append(wxString::Format(wxT("('%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),::wxGetUTCTime()));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

                    sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_AirCompressorParam[*id].first));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d)"),m_AirCompressorParam[*id].second));

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
