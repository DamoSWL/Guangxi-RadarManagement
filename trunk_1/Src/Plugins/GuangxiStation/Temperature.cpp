#include "Temperature.h"


static const int CMDTIMERID = 95;
static const int WRITETIMERID = 96; 


BEGIN_EVENT_TABLE(CTemperature,wxEvtHandler)
	EVT_TIMER(WRITETIMERID,CTemperature::onwriteDataTimeout)
	EVT_TIMER(CMDTIMERID,CTemperature::onCmdTimeout)
END_EVENT_TABLE()

CTemperature::CTemperature(void) : 
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{

}

CTemperature::~CTemperature(void)
{
	//for(auto item = m_temperatureRadarAlarm.begin(); item != m_temperatureRadarAlarm.end(); item++)
	//{
	//	delete m_temperatureRadarAlarm[item->first];
	//	item->second = NULL;
	//}
}

void CTemperature::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CTemperature::setID(const std::set<UINT>& id)
{
    m_IDSet = id;
	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newTemperatureFlag[*index] = false;
		//m_temperatureRadarAlarm[*index] = new CTemperatureRadarAlarm;
	}
}

void CTemperature::processCmd(const char* pData,unsigned int len)
{
	if((len == 15) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			// auto temperature = pData[7] + pData[8] / 100.0;
			// auto humidity = pData[10];

			auto tmptemperature = ((UINT)pData[7] << 8) + (UINT)pData[8];
			float sign = -1.0;
			if((tmptemperature & (1 << 15)) == 0)
			{
				sign = 1.0;
			}
			tmptemperature &= 0x7FFF;
			FLOAT temperature = tmptemperature * sign / 10.0;
	

			auto tmphumidity = ((UINT)pData[9] << 8) + (UINT)pData[10];
			FLOAT humidity = tmphumidity / 10.0;

			auto smogAlarm = pData[12];

			m_newTemperatureFlag[id] = true;
			m_TemperatureParam[id] = make_tuple(temperature,humidity,smogAlarm);
		}
	}
}

int CTemperature::processData(void *pHandle, const char *pData, UINT Len)
{
    IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CTemperature::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}


void CTemperature::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CTemperature::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CTemperature::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CTemperature::sendTemperatureCmd(UINT id)
{
    UCHAR TemperatureCmd[8] = {'\0'};
	TemperatureCmd[0] = id;
	TemperatureCmd[1] = '\x03';
	TemperatureCmd[2] = '\x00';
	TemperatureCmd[3] = '\x00';
	TemperatureCmd[4] = '\x00';
	TemperatureCmd[5] = '\x0A';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(TemperatureCmd,6);

	TemperatureCmd[6] = CRCCheckData.crcByte[1];
	TemperatureCmd[7] = CRCCheckData.crcByte[0];

	GetPlug()->sendCmd(Temperature,(const char*)&TemperatureCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send Temperature");
}

void CTemperature::onwriteDataTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	writeToDataBase();
	
	m_cmdTimer.Start(LTIMEINTERVAL);
	for(auto item = m_newTemperatureFlag.begin(); item != m_newTemperatureFlag.end(); item++)
	{
		//if(item->second)
		//{
		//	auto id = item->first;
		
			//m_temperatureRadarAlarm[id]->getAlarmType();
			//m_temperatureRadarAlarm[id]->getThresholdValue();
			//m_temperatureRadarAlarm[id]->getEnvValue(std::get<0>(m_TemperatureParam[id]),std::get<1>(m_TemperatureParam[id]));
			//m_temperatureRadarAlarm[id]->envCheck();
		//}
		item->second = false;
	}

	
}

void CTemperature::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	
	for(auto item = m_newTemperatureFlag.cbegin(); item != m_newTemperatureFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendTemperatureCmd(item->first);
		}
		::wxMilliSleep(200);
	}
	
	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CTemperature::writeToDataBase()
{
    wxString sqlSentence = wxT("INSERT INTO radar_env_temp (updateTime,\
        radarcd, \
        equipcode, \
		temperature, \
		humidity, \
		smoke, \
		water, \
		guard) VALUES");

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
                if(m_newTemperatureFlag[*id])
                {
                    auto sqlSentenceSingle = sqlSentence;
                    sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

                    sqlSentenceSingle.Append(wxString::Format(wxT("%f, "),std::get<0>(m_TemperatureParam[*id])));
					sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),std::get<1>(m_TemperatureParam[*id])));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),std::get<2>(m_TemperatureParam[*id])));
                    
                    sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%s)"),wxT("NULL")));


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
