#pragma once


class IProcess
{
public:
	virtual ~IProcess() {}
    virtual void setID(const std::set<UINT>& id) = 0;
	virtual void setIPAddress(const std::string& IPStr) = 0;
	virtual void processCmd(const char* pData,unsigned int len) = 0;
    virtual void startTimer(int interval) = 0;
    virtual void stopTimer() = 0;
};