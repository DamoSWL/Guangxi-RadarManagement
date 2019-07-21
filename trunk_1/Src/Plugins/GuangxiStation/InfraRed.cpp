#include "InfraRed.h"

static const int CMDTIMERID = 135;
static const int WRITETIMERID = 136; 


BEGIN_EVENT_TABLE(CInfraRed,wxEvtHandler)
    EVT_TIMER(WRITETIMERID,CInfraRed::onwriteDataTimeout)
    EVT_TIMER(CMDTIMERID,CInfraRed::onCmdTimeout)
END_EVENT_TABLE()

CInfraRed::CInfraRed(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID),
m_Count(0)
{

}


CInfraRed::~CInfraRed(void)
{
}

void CInfraRed::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CInfraRed::setID(const std::set<UINT>& id)
{
	m_IDSet = id;
    for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newInfraRedFlag[*index] = false;
	}
}

void CInfraRed::processCmd(const char* pData,unsigned int len)
{
	if((len == 7) && ((UCHAR)pData[1] == 0x03))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			m_newInfraRedFlag[id] = true;
			m_InfraRedParam[id] = (UINT)pData[4];
		}
	}
}

int CInfraRed::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CInfraRed::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}



void CInfraRed::startTimer(int interval)
{
    m_cmdTimer.Start(interval);
}

void CInfraRed::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CInfraRed::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(15*1000,wxTIMER_ONE_SHOT);
	}
}

void CInfraRed::sendInfraRedCmd(UINT id)
{
	UCHAR InfraRedCmd[8] = {'\0'};
	InfraRedCmd[0] = id;
	InfraRedCmd[1] = '\x03';
	InfraRedCmd[2] = '\x00';
	InfraRedCmd[3] = '\x03';
	InfraRedCmd[4] = '\x00';
	InfraRedCmd[5] = '\x01';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(InfraRedCmd,6);

	InfraRedCmd[6] = CRCCheckData.crcByte[1];
	InfraRedCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(InfraRed,(const char*)&InfraRedCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send infrared");
}

void CInfraRed::onwriteDataTimeout(wxTimerEvent& event)
{
    m_cmdTimer.Stop();
	
	for(auto item = m_InfraRedParam.cbegin(); item != m_InfraRedParam.cend(); item++)
	{
		if(item->second == 1)
		{
			writeToDataBase();
		}
		else if(item->second == 0)
		{
			if(m_Count == 0)
			{
				writeToDataBase();
			}
		}
	}

	m_Count++;
	if(m_Count >= 12)
	{
		m_Count = 0;
	}
	
	for(auto item = m_newInfraRedFlag.begin(); item != m_newInfraRedFlag.end(); item++)
	{
		item->second = false;
	}
    
    m_cmdTimer.Start(15*1000);
}

void CInfraRed::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	
	for(auto item = m_newInfraRedFlag.cbegin(); item != m_newInfraRedFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendInfraRedCmd(item->first);
		}
		::wxMilliSleep(200);
	}
	
	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CInfraRed::writeToDataBase()
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
                if(m_newInfraRedFlag[*id])
                {
                    auto sqlSentenceSingle = sqlSentence;
                    sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

                    sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
					sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
					sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
                    sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));

                    sqlSentenceSingle.Append(wxString::Format(wxT("%d)"),m_InfraRedParam[*id]));


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
