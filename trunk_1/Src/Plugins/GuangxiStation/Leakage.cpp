#include "Leakage.h"

static const int CMDTIMERID = 145;
static const int WRITETIMERID = 146; 


BEGIN_EVENT_TABLE(CLeakage,wxEvtHandler)
    EVT_TIMER(WRITETIMERID,CLeakage::onwriteDataTimeout)
    EVT_TIMER(CMDTIMERID,CLeakage::onCmdTimeout)
END_EVENT_TABLE()

CLeakage::CLeakage(void):
m_cmdTimer(this,CMDTIMERID),
m_writeDataTimer(this,WRITETIMERID)
{

}


CLeakage::~CLeakage(void)
{
}

void CLeakage::setIPAddress(const std::string& IPStr)
{
	m_IPAddress = IPStr;
}

void CLeakage::setID(const std::set<UINT>& id)
{
	m_IDSet = id;

	for(auto index = m_IDSet.cbegin(); index != m_IDSet.cend(); index++)
	{
		m_newLeakFlag[*index] = false;
	}
	

}

void CLeakage::processCmd(const char* pData,unsigned int len)
{
	if((len == 7) && ((UCHAR)pData[1] == 0x04))
	{
		UINT id = (UCHAR)pData[0];
		if(m_IDSet.find(id) != m_IDSet.end())
		{
			m_newLeakFlag[id] = true;
			m_LeakParam[id] = (UINT)pData[4];
		}
	}
}

int CLeakage::processData(void *pHandle, const char *pData, UINT Len)
{
	IProcess* self = static_cast<IProcess*>(pHandle);
	if(self)
	{
		self->processCmd(pData,Len);
	}
	return 0;
}

void CLeakage::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}



void CLeakage::startTimer(int interval)
{
	m_cmdTimer.Start(interval);
}

void CLeakage::stopTimer()
{
	m_cmdTimer.Stop();
	m_writeDataTimer.Stop();
}

void CLeakage::startWriteDataTimer()
{
	if(!m_writeDataTimer.IsRunning())
	{
		m_writeDataTimer.Start(WTIMEINTERVAL,wxTIMER_ONE_SHOT);
	}
}

void CLeakage::sendLeakCmd(UINT id)
{
	UCHAR LeakCmd[8] = {'\0'};

	LeakCmd[0] = id;
	LeakCmd[1] = '\x04';
	LeakCmd[2] = '\x00';
	LeakCmd[3] = '\x00';
	LeakCmd[4] = '\x00';
	LeakCmd[5] = '\x01';

	CRCData CRCCheckData;
	CRCCheckData.crcData = GetPlug()->chkcrc(LeakCmd,6);

	LeakCmd[6] = CRCCheckData.crcByte[1];
	LeakCmd[7] = CRCCheckData.crcByte[0];
	GetPlug()->sendCmd(Leakage,(const char*)&LeakCmd,8,m_IPAddress.c_str(),ENVFRAMEPORT);

	GetPlug()->AddLog(LOG_TYPE_MESSAGE,"send Leak");
}

void CLeakage::onwriteDataTimeout(wxTimerEvent& event)
{
    m_cmdTimer.Stop();
	writeToDataBase();
	
	for(auto item = m_newLeakFlag.begin(); item != m_newLeakFlag.end(); item++)
	{
		item->second = false;
	}
    
    m_cmdTimer.Start(LTIMEINTERVAL);
}

void CLeakage::onCmdTimeout(wxTimerEvent& event)
{
	m_cmdTimer.Stop();
	
	for(auto item = m_newLeakFlag.cbegin(); item != m_newLeakFlag.cend(); item++)
	{
		if(!item->second)
		{
			sendLeakCmd(item->first);
		}
		::wxMilliSleep(200);
	}
	
	m_cmdTimer.Start(STIMEINTERVAL);
	startWriteDataTimer();
}

bool CLeakage::writeToDataBase()
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
                if(m_newLeakFlag[*id])
                {
                    auto sqlSentenceSingle = sqlSentence;
                    sqlSentenceSingle.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%s', "), wxString(GetPlug()->getSiteInfo().c_str(),wxConvLocal)));
                    sqlSentenceSingle.Append(wxString::Format(wxT("'%x', "), *id));

					sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
					sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
					sqlSentenceSingle.Append(wxString::Format(wxT("%s, "),wxT("NULL")));
					
                    sqlSentenceSingle.Append(wxString::Format(wxT("%d, "),m_LeakParam[*id]));
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
