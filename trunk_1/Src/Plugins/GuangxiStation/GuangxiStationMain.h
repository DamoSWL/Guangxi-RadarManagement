
/* \file GuangxiStationMain.h
	\brief GuangxiStation插件类定义
	\ingroup GuangxiStationGroup
	
	本文件定义了GuangxiStationPlug 类。

	\version 1.0
	\author 
	\date 20171127

	\b 修改记录：
	\arg \c version 1.0， <author>，20171127 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangxiStationGroup
*/
#ifndef _HEADER_GuangxiStation_MAIN_20171127
#define _HEADER_GuangxiStation_MAIN_20171127


#include "GuangxiStationToolBar.h"
#include "GuangxiStationMenu.h"
#include "GuangxiStationPanel.h"
#include "GuangxiStationSet.h"
#include "mysqldatabase.h"
#include "pugixml.hpp"
#include <iphlpapi.h>
#include <icmpapi.h>
#include <WS2tcpip.h>
#include "IProcess.h"
#include "SimAirConditonerController.h"


const int WTIMEINTERVAL = 30*1000;
const int LTIMEINTERVAL = 3*60*1000 - 30*1000;
const int STIMEINTERVAL = 2*1000;

const int DEVICENO = 20;

const UINT ENVFRAMEPORT = 26;
const UINT UPSPORT = 23;

const UINT UPSSOI = 0x7E;
const UINT UPSEOI = 0x0D;


enum
{
	
	Temperature = 0,
	ThreePhasePower,
	SinglePhasePower,
	SixRoute,
	InfraRed,
	Leakage,
	AirCompressor,
	UPSSystemAnalogData,
	UPSExtendAnalogDataOne,
	UPSExtendAnalogDataTwo,
	UPSExtendAnalogDataThree,
	UPSSwitchState,
	UPSAlarmState,
	Q2UPS,
	WAUPS,
	G1UPS,
	G2UPS,
	G3UPS,
	EP60UPS
};

typedef union
{
	unsigned short crcData;
	unsigned char crcByte[2];
}CRCData;

typedef struct 
{
	std::string hostname;
	std::string user;
	std::string password;
	std::string databasename;	
}databaseConfig;

typedef struct
{
	WORD APhasePower;
	WORD APhaseCurrent;
	WORD APhasePowerRatio;
	WORD APhaseTotalPowerUpper;
	WORD APhaseTotalPwerLower;
	WORD APhasePowerFactor;
	WORD APhaseCarbonEmissionUpper;
	WORD APhaseCarbonEmissionLower;
	WORD APhaseAlarmState;

	WORD BPhasePower;
	WORD BPhaseCurrent;
	WORD BPhasePowerRatio;
	WORD BPhaseTotalPowerUpper;
	WORD BPhaseTotalPwerLower;
	WORD BPhasePowerFactor;
	WORD BPhaseCarbonEmissionUpper;
	WORD BPhaseCarbonEmissionLower;
	WORD BPhaseAlarmState;

	WORD CPhasePower;
	WORD CPhaseCurrent;
	WORD CPhasePowerRatio;
	WORD CPhaseTotalPowerUpper;
	WORD CPhaseTotalPwerLower;
	WORD CPhasePowerFactor;
	WORD CPhaseCarbonEmissionUpper;
	WORD CPhaseCarbonEmissionLower;
	WORD CPhaseAlarmState;

	WORD TotalPowerUpper;
	WORD TotalPowerLower;
	WORD frequency;
}ThreePhaseParam;



using namespace CDYW::PluginSystem;

/*! \class GuangxiStationPlug
	\brief GuangxiStation插件。
	\ingroup GuangxiStationGroup
*/
class GuangxiStationPlug: public wxEvtHandler,
	public IPluginHelper< GuangxiStationPlug >,
	public IGuangxiStation
{
	CDYW::Plugin::ICommon *m_IComm;
	bool m_bExit;
	wxString m_WorkPath;
public:
	// 接口定义
	IHostPlugin *m_IHost;
		// 成员变量定义
		IPluginToolBar m_toolbar;
		IPluginMenu m_menu;
		IPluginDlgPara m_DlgPara;
		IPluginDlgPara m_panel; 
public:
	GuangxiStationPlug();

	bool OnCommandHandle(PL_WORD wNotifyCode,PL_WORD wID,void * hWndCtl);

	//! 初始化完毕后调用, 插件在此函数中进行UI初始化等操作
	void OnInitialized();
	//! 获取插件的GUID
	PL_PID GetGUID()		{return g_PL_GuangxiStation;}
	//! 获取插件名称
	const char *GetName()	{return ("GuangxiStation");}
	//! 获取插件版本信息
	const char *GetVersion();
	//! 获取作者
	const char *GetAuthor()	{return ("CDYW");}
	//! 程序退出
	void OnExit();
	//! 程序是否退出
	bool GetExit()			{return m_bExit;}
	//! 初始化变量
	void OnActived();
	//! 释放资源
	void OnDestroy();
	//! 获取程序路径
	const wxString GetWorkPath()	{return m_WorkPath;}
		//! 从文件初始化工具栏
	bool InitToolbarData();
	bool InitMenuData(char* pData,int DataSize,wxString FileName);
		void *QueryInterfaceP( PL_IID guid );
	PL_GUID *GetDependenceList( int &cnt ,const char ***ppszDepNames );
	CDYW::Plugin::ICommon *GetComm() { return m_IComm; };
	//////////////////////////////////////////////////////////////////////////
public:
	unsigned int chkcrc(unsigned char *buf,unsigned char len);
	bool crcCheck(unsigned char* buf,unsigned int len);
	wxMutex* getSqlMutex() {return m_sqlMutex;}
	MySqlDatabase* getDatabase() const {return m_pDataBase;}

	const std::map<std::string,std::string>& getComputersIP() {return m_computersIP;}
	void setComputersIP(const std::map<std::string,std::string>& computersIP);

	const std::string& getSiteInfo() {return m_siteInfo;}

	bool sendCmd(UINT index,const char *pData, UINT Len,const char *DstIP,UINT DstPort);

	bool getCenterIPAddress();
	const std::string& getCenterIPStr() {return m_CenterIPAddress;}

	bool getEnvIPAddressRadar();
	bool getEnvIPAddressPower();

	const std::string& getEnvRadarIP() {return m_envRadarIPAddress;}
	const std::string& getEnvPowerIP() {return m_envPowerIPAddress;}

	std::pair<UCHAR, UCHAR> HexToASCII(UCHAR hexData);
	UCHAR ASCIIToHex(std::pair<UCHAR,UCHAR> asciiData);
	FLOAT UCHARToFloat(PUCHAR upsData);


private:
	unsigned int calccrc(unsigned char crcbuf,unsigned int crc);
	bool loadConfig();

	void clearComputersIPinXML();
	void addComputersIPinXML();

	void onPingTimeout(wxTimerEvent& event);
	bool writePingtoDataBase(const std::string& computerName,INT result);
	bool writePingAlarmtoDataBase(const std::string& computerIP,INT result);

private:
	PCHAR UnicodeToUtf8(const wchar_t* unicode);
	PWCHAR Utf8ToUnicode(const char* utf8Str);
	
private:

	CUDP m_UDPFactory;
	ISocketNet* m_pUDPInstanceRadar[DEVICENO];
	IProcess* m_pDataProcessRadar[DEVICENO];

	ISocketNet* m_pUDPInstancePower[DEVICENO];
	IProcess* m_pDataProcessPower[DEVICENO];

	std::string m_siteInfo;
	std::map<std::string,std::string> m_computersIP;
	std::map<std::string,BOOL> m_computerDisconnectFlag;

	databaseConfig m_dataBaseConfig;
	MySqlDatabase* m_pDataBase;
	wxMutex* m_sqlMutex;
	wxMutex* m_xmlMutex;
	pugi::xml_document m_xmlDoc;

	wxTimer m_PingTimer;


	std::string m_CenterIPAddress;
	std::string m_envRadarIPAddress;
	std::string m_envPowerIPAddress;

	CSimAirConditonerController m_SimpleAirConditionerCtrl;


	
	DECLARE_EVENT_TABLE()

};



#endif

