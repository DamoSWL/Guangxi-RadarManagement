#include "SimAirConditonerController.h"


CSimAirConditonerController::CSimAirConditonerController(void):m_UDPHandler(NULL)
{
}


CSimAirConditonerController::~CSimAirConditonerController(void)
{
}

bool CSimAirConditonerController::startReceive()
{
    m_UDPHandler = m_UDPFactory.GetInstance();
    if(m_UDPHandler)
    {
        m_UDPHandler->RegHandle(this, CSimAirConditonerController::processData,CSimAirConditonerController::processEvent);
        auto portNum =std::stoi(GetPlug()->getSiteInfo().substr(1,4));
        if (m_UDPHandler->StartReceive(0, "0.0.0.0", portNum, nullptr, 0, 8192, 8192, 1000000))
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,"普通空调控制端口绑定成功");
		}
		else
		{
			GetPlug()->AddLog(LOG_TYPE_ERROR,"普通空调控制端口绑定失败");
			return false;
		}
    }

    return true;
}

bool CSimAirConditonerController::stopReceive()
{
    if(m_UDPHandler)
    {
        m_UDPHandler->StopReceive();
		m_UDPFactory.ReleaseInstance(m_UDPHandler);
        m_UDPHandler = NULL;
        return true;
    }
    return false;
}

bool CSimAirConditonerController::sendData(const char* cmd,UINT len,const char* dstIP,UINT port)
{
	if(m_UDPHandler->IsWorking())
	{
		return m_UDPHandler->SendData(cmd,len,dstIP,port);
	}
	return false;
}

int CSimAirConditonerController::processData(void *pHandle, const char *pData, UINT Len)
{
    CSimAirConditonerController* self = static_cast<CSimAirConditonerController*>(pHandle);
    if(self)
    {

		static UINT retCodeNum = 0;
        auto IDSet = self->getID();

		auto centerIPAddress = GetPlug()->getCenterIPStr();
		auto portNum = std::stoi(GetPlug()->getSiteInfo().substr(1,4));

		auto envRadarIP = GetPlug()->getEnvPowerIP();

		char cmd[6] = {'\0'};

		if((Len == 4) && (std::memcmp(pData,"OPEN",4) == 0))
		{
			cmd[0] = '\x04';

			for(auto id = IDSet.cbegin(); id != IDSet.cend(); id++)
			{
				cmd[1] = (char)(*id);

				cmd[2] = (CHAR)0xFF;
				cmd[3] = 0x08;
				cmd[4] = 0x08;

				cmd[5] = cmd[0];
				for(auto index = 1; index < 5; index++)
				{
					cmd[5] ^= cmd[index];
				}
				self->sendData(cmd,6,envRadarIP.c_str(),ENVFRAMEPORT);
				::wxMilliSleep(300);

			}
		}
		else if((Len == 5) && (std::memcmp(pData,"CLOSE",5) == 0))
		{
			cmd[0] = '\x04';

			for(auto id = IDSet.cbegin(); id != IDSet.cend(); id++)
			{
				cmd[1] = (char)(*id);

				cmd[2] = 0;
				cmd[3] = 0x08;
				cmd[4] = 0x08;

				cmd[5] = cmd[0];
				for(auto index = 1; index < 5; index++)
				{
					cmd[5] ^= cmd[index];
				}
				self->sendData(cmd,6,envRadarIP.c_str(),ENVFRAMEPORT);
				::wxMilliSleep(300);
			}
		}
		else if((Len == 1) && (*pData == '\x89'))
		{
			//self->sendData("SUCCESS",7,centerIPAddress.c_str(),portNum);
			retCodeNum++;
		}
		else if((Len == 1) && (*pData == '\xE0'))
		{
			retCodeNum++;
			self->sendData("FAIL",4,centerIPAddress.c_str(),portNum);
		}
		else{}

		return EXIT_SUCCESS;
    }

	return EXIT_FAILURE;
}

void CSimAirConditonerController::processEvent(void *pHandle, int Opt, char *pData /*= nullptr*/, UINT Len /*= 0*/)
{

}
