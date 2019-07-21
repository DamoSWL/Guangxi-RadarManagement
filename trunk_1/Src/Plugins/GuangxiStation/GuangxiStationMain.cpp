// PluginTest.cpp : Defines the entry point for the DLL application.
//
#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangxiStationMain.h"
#include "Temperature.h"
#include "ThreePhasePower.h"
#include "SixRoute.h"
#include "SinglePhasePower.h"
#include "InfraRed.h"
#include "Leakage.h"
#include "AirCompressor.h"
#include "SystemAnalogData.h"
#include "SelfDefAnalogDataOne.h"
#include "SelfDefAnalogDataTwo.h"
#include "SelfDefAnalogDataThree.h"
#include "SwitchData.h"
#include "UPSAlarm.h"
#include "UPSDataQ2.h"
#include "UPSDataWA.h"
#include "UPSDataG1.h"
#include "UPSDataG2.h"
#include "UPSDataG3.h"
#include "UPSDataEP60.h"
#include "SimAirConditonerController.h"



const int PINGTIMERID = 65;
BEGIN_EVENT_TABLE(GuangxiStationPlug,wxEvtHandler)
	EVT_TIMER(PINGTIMERID,GuangxiStationPlug::onPingTimeout)
END_EVENT_TABLE()


GuangxiStationPlug* g_pMain = NULL;

// 全局函数
GuangxiStationPlug* GetPlug() 
{ 
	return g_pMain;
};
PL_GUID	g_DependGuid[10] = {0};
const char *g_szDependPluginName[10];


GuangxiStationPlug::GuangxiStationPlug():
  m_IHost(NULL),m_IComm(NULL),
	  m_PingTimer(this,PINGTIMERID)
  {
	g_pMain = this;
	////////////////
	m_bExit = false;
	m_WorkPath = wxEmptyString;
	for(auto index = 0; index < DEVICENO; index++)
	{
		m_pUDPInstanceRadar[index] = NULL;
		m_pDataProcessRadar[index] = NULL;

		m_pUDPInstancePower[index] = NULL;
		m_pDataProcessPower[index] = NULL;
	}
}


bool GuangxiStationPlug::OnCommandHandle(
	PL_WORD wNotifyCode, 
	PL_WORD wID, 
	void * hWndCtl) 
{
	//wxString szOut( wxT("hello"));
	//::wxMessageBox( szOut );
	
	static bool bEnable = true;
	bEnable = !bEnable;
	
	//m_IHost->EnableToolBarCommand( m_toolbar, 21, bEnable );
	//m_IHost->CheckToolBarCommand( m_toolbar,22, !bEnable);
		
	//	m_IHost->EnableMenuCommand(m_menu,11,bEnable);
	//m_IHost->CheckMenuCommand(m_menu,12,!bEnable);
		
	if( wID == 22 )
	{
		m_IHost->ShowPanel( m_panel,true );
	}
	else
	{
		m_IHost->ShowPanel( m_panel,false );
	}
	if(wID == 11)
	{
		m_IHost->ShowTheSetDlg(g_guid_Set);

	}
		
	
	return true;
}

//! 初始化完毕后调用, 插件在此函数中进行UI初始化等操作
void GuangxiStationPlug::OnInitialized()
{
	//// 获取需要的接口
	m_IHost = (IHostPlugin*)m_IFrame->QueryInterfaceP( g_PL_GRSFrame, g_I_Host );
	m_IComm = (CDYW::Plugin::ICommon *)m_IFrame->QueryInterfaceP( g_PL_GRSFrame, guid_I_Common );
	if( m_IComm ) {
		m_WorkPath = wxString(m_IComm->GetWorkPath(),wxConvLocal);
		m_IComm->AddLog( 0, "插件[  GuangxiStation ]..." );
	}

	//m_pTCPClient = (ITCPClient *)m_IFrame->QueryInterfaceP(g_PL_TCPClient,g_I_TCPClient);
	
	//// 注册工具栏 和 菜单
	if( m_IHost ) 
	{
				//// 注册菜单
		//memset( &m_menu, 0, sizeof(IPluginMenu));
		m_menu.pluginGUID = g_PL_GuangxiStation;
		m_menu.parGUID = guid_SetConfig;
		m_menu.selfGUID = g_guid_menu;
		m_menu.menuDefine = (char*)g_MenuXML;
		//m_menu.lSize = strlen(g_MenuXML);
		m_IHost->RegisterMenu( m_menu );
				
				//// 注册工具栏
		//memset( &m_toolbar, 0, sizeof(IPluginToolBar));
		//m_toolbar.bNewBar = true;
		//m_toolbar.pluginGUID = g_PL_GuangxiStation;
		//m_toolbar.parGUID = guid_NULL;
		//m_toolbar.selfGUID = g_guid_toolbar;
		//strcpy( m_toolbar.name, "TB_GuangxiStation" );
		//m_toolbar.barDefine = (char*)g_ToolBarXML;
		//m_toolbar.lSize = strlen(g_ToolBarXML);

		//m_IHost->RegisterToolBar( m_toolbar );
				
				//// 注册参数设置面板
		//memset( &m_DlgPara, 0 , sizeof(IPluginDlgPara));
		strcpy( m_DlgPara.cName, "参数设置" );
		m_DlgPara.selfGUID = g_guid_Set;
		m_DlgPara.parGUID = guid_SET_GRS;
		m_DlgPara.pluginGUID = g_PL_GuangxiStation;
		m_DlgPara.funCreate = GuangxiStationSet::CreatePage;

		m_IHost->RegisterSettingDialog( m_DlgPara );
				
				////  注册普通面板
		//memset( &m_panel, 0, sizeof(IPluginDlgPara));
// 		strcpy( m_panel.cName, "GuangxiStation" );
// 		m_panel.pluginGUID = g_PL_GuangxiStation;
// 		m_panel.selfGUID = g_guid_panel;
// 		m_panel.parGUID = guid_ConfigBook;
// 		m_panel.funCreate = GuangxiStationPanel::CreatePage;
// 		m_panel.funDelete = GuangxiStationPanel::DeletePage;
// 		m_panel.lStyle = PANE_DEF_STYLE;

		m_IHost->RegisterPanel( m_panel );
				
	}
}

void GuangxiStationPlug::OnExit()
{
	m_bExit = true;

	m_PingTimer.Stop();

	m_pDataBase->CloseDatabase();

	delete m_pDataBase;
	delete m_sqlMutex;
	delete m_xmlMutex;

	for(auto index = 0; index < DEVICENO; index++)
	{
		if(m_pDataProcessRadar[index])
		{
			m_pDataProcessRadar[index]->stopTimer();
		}
		
		if(m_pDataProcessPower[index])
		{
			m_pDataProcessPower[index]->stopTimer();
		}
		
	}

	m_SimpleAirConditionerCtrl.stopReceive();

	for(auto index = 0; index < DEVICENO; index++)
	{
		if(m_pUDPInstanceRadar[index])
		{
			m_pUDPInstanceRadar[index]->StopReceive();
			m_UDPFactory.ReleaseInstance(m_pUDPInstanceRadar[index]);
			m_pUDPInstanceRadar[index] = NULL;
			if(m_pDataProcessRadar[index])
			{
				m_pDataProcessRadar[index]->stopTimer();
				delete m_pDataProcessRadar[index];
				m_pDataProcessRadar[index] = NULL;
			}
		}
	}

	for(auto index = 0; index < DEVICENO; index++)
	{
		if(m_pUDPInstancePower[index])
		{
			m_pUDPInstancePower[index]->StopReceive();
			m_UDPFactory.ReleaseInstance(m_pUDPInstancePower[index]);
			m_pUDPInstancePower[index] = NULL;
			if(m_pDataProcessPower[index])
			{
				m_pDataProcessPower[index]->stopTimer();
				delete m_pDataProcessPower[index];
				m_pDataProcessPower[index] = NULL;
			}
		}
	}

}

void GuangxiStationPlug::OnActived()
{
	m_bExit = false;

	m_pDataBase = new MySqlDatabase();
	m_sqlMutex = new wxMutex(wxMUTEX_DEFAULT);
	m_xmlMutex = new wxMutex(wxMUTEX_DEFAULT);

	if(loadConfig())
	{
		AddLog(LOG_TYPE_MESSAGE,"xml配置文件读取成功");
	}
	else
	{
		AddLog(LOG_TYPE_ERROR,"xml配置文件读取失败");
		return;
	}

	if(m_pDataBase->Connect((char*)m_dataBaseConfig.hostname.c_str(),
		(char*)m_dataBaseConfig.user.c_str(),
		(char*)m_dataBaseConfig.password.c_str(),
		(char*)m_dataBaseConfig.databasename.c_str()))
	{
		AddLog(LOG_TYPE_MESSAGE,"数据库打开成功");
	}
	else
	{
		AddLog(LOG_TYPE_ERROR,"数据库打开失败");
		return;
	}

	getCenterIPAddress();
	getEnvIPAddressRadar();
	getEnvIPAddressPower();

 	std::set<UINT> airConditionID;
 	airConditionID.insert(0x90);
 	airConditionID.insert(0x91);
 	m_SimpleAirConditionerCtrl.setID(airConditionID);
 	m_SimpleAirConditionerCtrl.startReceive();

  	for(auto index = 0; index < DEVICENO; index++)
  	{
  		m_pUDPInstanceRadar[index] = m_UDPFactory.GetInstance();
  		m_pUDPInstancePower[index] = m_UDPFactory.GetInstance();
  	}
  
  	if(m_pUDPInstanceRadar[Temperature])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x21);
			id.insert(0x22);
			id.insert(0x23);
		}
 
  		m_pDataProcessRadar[Temperature] = new CTemperature;
  		m_pDataProcessRadar[Temperature]->setID(id);
  		m_pDataProcessRadar[Temperature]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[Temperature]->RegHandle(m_pDataProcessRadar[Temperature],CTemperature::processData,CTemperature::processEvent);
  		m_pUDPInstanceRadar[Temperature]->StartReceive(0,"0.0.0.0",5001,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[ThreePhasePower])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x31);
			id.insert(0x32);
		}
  
  		m_pDataProcessRadar[ThreePhasePower] = new CThreePhasePower;
  		m_pDataProcessRadar[ThreePhasePower]->setID(id);
  		m_pDataProcessRadar[ThreePhasePower]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[ThreePhasePower]->RegHandle(m_pDataProcessRadar[ThreePhasePower],CThreePhasePower::processData,CThreePhasePower::processEvent);
  		m_pUDPInstanceRadar[ThreePhasePower]->StartReceive(0,"0.0.0.0",5007,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[SinglePhasePower])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{  		
			id.insert(0x41);
			id.insert(0x42);
			id.insert(0x43);
			id.insert(0x44);
			id.insert(0x45);
			id.insert(0x46);
		}

  
  		m_pDataProcessRadar[SinglePhasePower] = new CSinglePhasePower;
  		m_pDataProcessRadar[SinglePhasePower]->setID(id);
  		m_pDataProcessRadar[SinglePhasePower]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[SinglePhasePower]->RegHandle(m_pDataProcessRadar[SinglePhasePower],CSinglePhasePower::processData,CSinglePhasePower::processEvent);
  		m_pUDPInstanceRadar[SinglePhasePower]->StartReceive(0,"0.0.0.0",5012,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[SixRoute])
  	{	
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x51);
		}
  		
  		m_pDataProcessRadar[SixRoute] = new CSixRoute;
  		m_pDataProcessRadar[SixRoute]->setID(id);
  		m_pDataProcessRadar[SixRoute]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[SixRoute]->RegHandle(m_pDataProcessRadar[SixRoute],CSixRoute::processData,CSixRoute::processEvent);
  		m_pUDPInstanceRadar[SixRoute]->StartReceive(0,"0.0.0.0",5018,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[InfraRed])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x71);
		}
  	
  		m_pDataProcessRadar[InfraRed] = new CInfraRed;
  		m_pDataProcessRadar[InfraRed]->setID(id);
  		m_pDataProcessRadar[InfraRed]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[InfraRed]->RegHandle(m_pDataProcessRadar[InfraRed],CInfraRed::processData,CInfraRed::processEvent);
  		m_pUDPInstanceRadar[InfraRed]->StartReceive(0,"0.0.0.0",5025,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[Leakage])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x61);
			id.insert(0x62);
			id.insert(0x63);
		}

  
  		m_pDataProcessRadar[Leakage] = new CLeakage;
  		m_pDataProcessRadar[Leakage]->setID(id);
  		m_pDataProcessRadar[Leakage]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[Leakage]->RegHandle(m_pDataProcessRadar[Leakage],CLeakage::processData,CLeakage::processEvent);
  		m_pUDPInstanceRadar[Leakage]->StartReceive(0,"0.0.0.0",5027,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstanceRadar[AirCompressor])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x81);
		}
  		
  
  		m_pDataProcessRadar[AirCompressor] = new CAirCompressor;
  		m_pDataProcessRadar[AirCompressor]->setID(id);
  		m_pDataProcessRadar[AirCompressor]->setIPAddress(m_envRadarIPAddress);
  		m_pUDPInstanceRadar[AirCompressor]->RegHandle(m_pDataProcessRadar[AirCompressor],CAirCompressor::processData,CAirCompressor::processEvent);
  		m_pUDPInstanceRadar[AirCompressor]->StartReceive(0,"0.0.0.0",5030,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstancePower[Temperature])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x24);
		}
  		
  
  		m_pDataProcessPower[Temperature] = new CTemperature;
  		m_pDataProcessPower[Temperature]->setID(id);
  		m_pDataProcessPower[Temperature]->setIPAddress(m_envPowerIPAddress);
  		m_pUDPInstancePower[Temperature]->RegHandle(m_pDataProcessPower[Temperature],CTemperature::processData,CTemperature::processEvent);
  		m_pUDPInstancePower[Temperature]->StartReceive(0,"0.0.0.0",5032,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstancePower[Leakage])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x64);
		}
  		
  
  		m_pDataProcessPower[Leakage] = new CLeakage;
  		m_pDataProcessPower[Leakage]->setID(id);
  		m_pDataProcessPower[Leakage]->setIPAddress(m_envPowerIPAddress);
  		m_pUDPInstancePower[Leakage]->RegHandle(m_pDataProcessPower[Leakage],CLeakage::processData,CLeakage::processEvent);
  		m_pUDPInstancePower[Leakage]->StartReceive(0,"0.0.0.0",5038,NULL,0,8192,8192,1000000);
  	}
  
  	if(m_pUDPInstancePower[InfraRed])
  	{
  		std::set<UINT> id;
		if(m_siteInfo == "Z9771")
		{
			id.insert(0x72);
		}
  		
  		m_pDataProcessPower[InfraRed] = new CInfraRed;
  		m_pDataProcessPower[InfraRed]->setID(id);
  		m_pDataProcessPower[InfraRed]->setIPAddress(m_envPowerIPAddress);
  		m_pUDPInstancePower[InfraRed]->RegHandle(m_pDataProcessPower[InfraRed],CInfraRed::processData,CInfraRed::processEvent);
  		m_pUDPInstancePower[InfraRed]->StartReceive(0,"0.0.0.0",5041,NULL,0,8192,8192,1000000);
  	}

	if((m_siteInfo == "Z9771") || (m_siteInfo == "Z9775") || (m_siteInfo == "Z9770") || (m_siteInfo == "Z9779"))
	{
		if(m_pUDPInstanceRadar[UPSSystemAnalogData])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[UPSSystemAnalogData] = new CSystemAnalogData;
			m_pDataProcessRadar[UPSSystemAnalogData]->setID(id);
			m_pDataProcessRadar[UPSSystemAnalogData]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSSystemAnalogData]->RegHandle(m_pDataProcessRadar[UPSSystemAnalogData],CSystemAnalogData::processData,CSystemAnalogData::processEvent);
			m_pUDPInstanceRadar[UPSSystemAnalogData]->StartReceive(0,"0.0.0.0",5065,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[UPSExtendAnalogDataOne])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[UPSExtendAnalogDataOne] = new CSelfDefAnalogDataOne;
			m_pDataProcessRadar[UPSExtendAnalogDataOne]->setID(id);
			m_pDataProcessRadar[UPSExtendAnalogDataOne]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSExtendAnalogDataOne]->RegHandle(m_pDataProcessRadar[UPSExtendAnalogDataOne],CSelfDefAnalogDataOne::processData,CSelfDefAnalogDataOne::processEvent);
			m_pUDPInstanceRadar[UPSExtendAnalogDataOne]->StartReceive(0,"0.0.0.0",5074,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[UPSExtendAnalogDataTwo])
		{
			std::set<UINT> id;
			id.insert(0x01);


			m_pDataProcessRadar[UPSExtendAnalogDataTwo] = new CSelfDefAnalogDataTwo;
			m_pDataProcessRadar[UPSExtendAnalogDataTwo]->setID(id);
			m_pDataProcessRadar[UPSExtendAnalogDataTwo]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSExtendAnalogDataTwo]->RegHandle(m_pDataProcessRadar[UPSExtendAnalogDataTwo],CSelfDefAnalogDataTwo::processData,CSelfDefAnalogDataTwo::processEvent);
			m_pUDPInstanceRadar[UPSExtendAnalogDataTwo]->StartReceive(0,"0.0.0.0",5088,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[UPSExtendAnalogDataThree])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[UPSExtendAnalogDataThree] = new CSelfDefAnalogDataThree;
			m_pDataProcessRadar[UPSExtendAnalogDataThree]->setID(id);
			m_pDataProcessRadar[UPSExtendAnalogDataThree]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSExtendAnalogDataThree]->RegHandle(m_pDataProcessRadar[UPSExtendAnalogDataThree],CSelfDefAnalogDataThree::processData,CSelfDefAnalogDataThree::processEvent);
			m_pUDPInstanceRadar[UPSExtendAnalogDataThree]->StartReceive(0,"0.0.0.0",5090,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[UPSSwitchState])
		{
			std::set<UINT> id;
			id.insert(0x01);


			m_pDataProcessRadar[UPSSwitchState] = new CSwitchData;
			m_pDataProcessRadar[UPSSwitchState]->setID(id);
			m_pDataProcessRadar[UPSSwitchState]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSSwitchState]->RegHandle(m_pDataProcessRadar[UPSSwitchState],CSwitchData::processData,CSwitchData::processEvent);
			m_pUDPInstanceRadar[UPSSwitchState]->StartReceive(0,"0.0.0.0",5092,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[UPSAlarmState])
		{
			std::set<UINT> id;
			id.insert(0x01);


			m_pDataProcessRadar[UPSAlarmState] = new CUPSAlarm;
			m_pDataProcessRadar[UPSAlarmState]->setID(id);
			m_pDataProcessRadar[UPSAlarmState]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[UPSAlarmState]->RegHandle(m_pDataProcessRadar[UPSAlarmState],CUPSAlarm::processData,CUPSAlarm::processEvent);
			m_pUDPInstanceRadar[UPSAlarmState]->StartReceive(0,"0.0.0.0",5098,NULL,0,8192,8192,1000000);
		}
	}
	else if((m_siteInfo == "Z9774") || (m_siteInfo == "Z9776") || (m_siteInfo == "Z9778"))
	{
		if(m_pUDPInstanceRadar[EP60UPS])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[EP60UPS] = new CUPSDataEP60;
			m_pDataProcessRadar[EP60UPS]->setID(id);
			m_pDataProcessRadar[EP60UPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[EP60UPS]->RegHandle(m_pDataProcessRadar[EP60UPS],CUPSDataEP60::processData,CUPSDataEP60::processEvent);
			m_pUDPInstanceRadar[EP60UPS]->StartReceive(0,"0.0.0.0",5121,NULL,0,8192,8192,1000000);
		}
	}
	else if(m_siteInfo == "Z9772")
	{
		if(m_pUDPInstanceRadar[Q2UPS])
		{
			std::set<UINT> id;
			id.insert(0x01);
	

			m_pDataProcessRadar[Q2UPS] = new CUPSDataQ2;
			m_pDataProcessRadar[Q2UPS]->setID(id);
			m_pDataProcessRadar[Q2UPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[Q2UPS]->RegHandle(m_pDataProcessRadar[Q2UPS],CUPSDataQ2::processData,CUPSDataQ2::processEvent);
			m_pUDPInstanceRadar[Q2UPS]->StartReceive(0,"0.0.0.0",5102,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[WAUPS])
		{
			std::set<UINT> id;
			id.insert(0x01);


			m_pDataProcessRadar[WAUPS] = new CUPSDataWA;
			m_pDataProcessRadar[WAUPS]->setID(id);
			m_pDataProcessRadar[WAUPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[WAUPS]->RegHandle(m_pDataProcessRadar[WAUPS],CUPSDataWA::processData,CUPSDataWA::processEvent);
			m_pUDPInstanceRadar[WAUPS]->StartReceive(0,"0.0.0.0",5107,NULL,0,8192,8192,1000000);
		}
	}
	else if(m_siteInfo == "Z9075")
	{
		if(m_pUDPInstanceRadar[G1UPS])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[G1UPS] = new CUPSDataG1;
			m_pDataProcessRadar[G1UPS]->setID(id);
			m_pDataProcessRadar[G1UPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[G1UPS]->RegHandle(m_pDataProcessRadar[G1UPS],CUPSDataG1::processData,CUPSDataG1::processEvent);
			m_pUDPInstanceRadar[G1UPS]->StartReceive(0,"0.0.0.0",5112,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[G2UPS])
		{
			std::set<UINT> id;
			id.insert(0x01);

			m_pDataProcessRadar[G2UPS] = new CUPSDataG2;
			m_pDataProcessRadar[G2UPS]->setID(id);
			m_pDataProcessRadar[G2UPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[G2UPS]->RegHandle(m_pDataProcessRadar[G2UPS],CUPSDataG2::processData,CUPSDataG2::processEvent);
			m_pUDPInstanceRadar[G2UPS]->StartReceive(0,"0.0.0.0",5116,NULL,0,8192,8192,1000000);
		}

		if(m_pUDPInstanceRadar[G3UPS])
		{
			std::set<UINT> id;
			id.insert(0x01);
			

			m_pDataProcessRadar[G3UPS] = new CUPSDataG3;
			m_pDataProcessRadar[G3UPS]->setID(id);
			m_pDataProcessRadar[G3UPS]->setIPAddress(m_envRadarIPAddress);
			m_pUDPInstanceRadar[G3UPS]->RegHandle(m_pDataProcessRadar[G3UPS],CUPSDataG3::processData,CUPSDataG3::processEvent);
			m_pUDPInstanceRadar[G3UPS]->StartReceive(0,"0.0.0.0",5121,NULL,0,8192,8192,1000000);
		}
	}
	else{}

	m_PingTimer.Start(10*60*1000);

	m_pDataProcessRadar[Temperature]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[ThreePhasePower]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[SixRoute]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[SinglePhasePower]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[InfraRed]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[Leakage]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessRadar[AirCompressor]->startTimer(STIMEINTERVAL);

	::wxMilliSleep(1000);
	m_pDataProcessPower[Temperature]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessPower[Leakage]->startTimer(STIMEINTERVAL);
	::wxMilliSleep(1000);
	m_pDataProcessPower[InfraRed]->startTimer(STIMEINTERVAL);

	if((m_siteInfo == "Z9771") || (m_siteInfo == "Z9775") || (m_siteInfo == "Z9770") || (m_siteInfo == "Z9779"))
	{
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSSystemAnalogData]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSExtendAnalogDataOne]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSExtendAnalogDataTwo]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSExtendAnalogDataThree]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSSwitchState]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[UPSAlarmState]->startTimer(STIMEINTERVAL);
	}
	else if((m_siteInfo == "Z9774") || (m_siteInfo == "Z9776") || (m_siteInfo == "Z9778"))
	{
		::wxMicroSleep(1000);
		m_pDataProcessRadar[EP60UPS]->startTimer(STIMEINTERVAL);
	}
	else if(m_siteInfo == "Z9772")
	{
		::wxMicroSleep(1000);
		m_pDataProcessRadar[Q2UPS]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[WAUPS]->startTimer(STIMEINTERVAL);
	}
	else if(m_siteInfo == "Z9075")
	{
		::wxMicroSleep(1000);
		m_pDataProcessRadar[G1UPS]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[G2UPS]->startTimer(STIMEINTERVAL);
		::wxMicroSleep(1000);
		m_pDataProcessRadar[G3UPS]->startTimer(STIMEINTERVAL);
	}
	else{}

}

void GuangxiStationPlug::OnDestroy()
{

}

void * GuangxiStationPlug::QueryInterfaceP(PL_IID guid)
{
	if( IsEqualID( guid, g_I_GuangxiStation ) )
	{
		return (IGuangxiStation*)(g_pMain); 
	}
	return NULL;  
}

PL_GUID * GuangxiStationPlug::GetDependenceList(int &cnt ,const char ***ppszDepNames)
{
	cnt = 0;
	if( ppszDepNames != NULL )
	{
		*ppszDepNames = g_szDependPluginName;
	}
	return g_DependGuid;
}

unsigned int GuangxiStationPlug::calccrc(unsigned char crcbuf,unsigned int crc)
{
	unsigned char i; 
	unsigned char chk; 
	crc=crc ^ crcbuf; 
	for(i=0;i<8;i++) 
	{ 		
		chk=( unsigned char)(crc&1); 
		crc=crc>>1; 
		crc=crc&0x7fff;  
		if (chk==1)
			crc=crc^0xa001;  
		crc=crc&0xffff;  
	}
	return crc; 

}

bool GuangxiStationPlug::loadConfig()
{
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");

	pugi::xml_parse_result result = m_xmlDoc.load_file(fileName.c_str());
	if(result.status !=pugi::status_ok)
	{
		return false;
	}

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node siteInfo = root.child("siteInfo");
	pugi::xml_node computersIP = root.child("computersIP");
	pugi::xml_node database = root.child("database");

	m_siteInfo.assign(siteInfo.text().as_string());

	for(auto item = computersIP.attributes_begin(); item != computersIP.attributes_end(); item++)
	{
		std::string IPAddr(item->value());
		if(IPAddr.empty())
		{
			GetPlug()->AddLog(LOG_TYPE_ERROR,"xml配置文件中IP为空");
		}
		else
		{
			m_computersIP[item->name()] = IPAddr;
			m_computerDisconnectFlag[IPAddr] = FALSE;
		}
		
	}

	m_dataBaseConfig.hostname.assign(database.child("hostname").text().as_string());
	m_dataBaseConfig.user.assign(database.child("user").text().as_string());
	m_dataBaseConfig.password.assign(database.child("password").text().as_string());
	m_dataBaseConfig.databasename.assign(database.child("databasename").text().as_string());


	return true;
}

void GuangxiStationPlug::clearComputersIPinXML()
{
	wxMutexLocker locker(*m_xmlMutex);
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node computersIP = root.child("computersIP");

	std::set<pugi::xml_attribute> attributeNode;
	for(auto item = computersIP.attributes_begin(); item != computersIP.attributes_end(); item++)
	{
		if(!item->empty())
		{
			attributeNode.insert(*item);
		}
	}

	for(auto item = attributeNode.begin(); item != attributeNode.end(); item++)
	{
		computersIP.remove_attribute(*item);
	}
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存computersIP失败");
	}
}

void GuangxiStationPlug::addComputersIPinXML()
{
	wxMutexLocker locker(*m_xmlMutex);
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node computersIP = root.child("computersIP");


	if(m_computersIP.find("RPG") != m_computersIP.end())
	{
		auto attributeNode = computersIP.append_attribute("RPG");
		attributeNode.set_value(m_computersIP["RPG"].c_str());
	}
	if(m_computersIP.find("PUP") != m_computersIP.end())
	{
		auto attributeNode = computersIP.append_attribute("PUP");
		attributeNode.set_value(m_computersIP["PUP"].c_str());
	}
	if(m_computersIP.find("PUP2") != m_computersIP.end())
	{
		auto attributeNode = computersIP.append_attribute("PUP2");
		attributeNode.set_value(m_computersIP["PUP2"].c_str());
	}


	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存computersIP失败");
	}
}

void GuangxiStationPlug::onPingTimeout(wxTimerEvent& event)
{
	if(getEnvIPAddressRadar())
	{
		for(auto index = 0; index < DEVICENO; index++)
		{
			if(m_pDataProcessRadar[index])
			{
				m_pDataProcessRadar[index]->setIPAddress(m_envRadarIPAddress);
			}
		}
	}

	if(getEnvIPAddressPower())
	{
		for(auto index = 0; index < DEVICENO; index++)
		{
			if(m_pDataProcessPower[index])
			{
				m_pDataProcessPower[index]->setIPAddress(m_envPowerIPAddress);
			}
		}
	}


	for(auto item = m_computersIP.cbegin(); item != m_computersIP.cend(); item++)
	{
		HANDLE hIcmpFile;
		ULONG ipaddr = INADDR_NONE;
		DWORD dwRetVal = 0;
		char SendData[32] = "Data Buffer";
		LPVOID ReplyBuffer = NULL;
		DWORD ReplySize = 0;
		if(!item->second.empty())
		{
			inet_pton(AF_INET, item->second.c_str(), &ipaddr);

			hIcmpFile = IcmpCreateFile();
			if (hIcmpFile == INVALID_HANDLE_VALUE) 
			{
				GetPlug()->AddLog(LOG_TYPE_ERROR,"invalid ping handle");
				break;
			}   
			else
			{
				ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
				ReplyBuffer = (VOID*)malloc(ReplySize);

				if(ReplyBuffer)
				{
					dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),NULL, ReplyBuffer, ReplySize, 2000);
					PICMP_ECHO_REPLY replyInfo = static_cast<PICMP_ECHO_REPLY>(ReplyBuffer);
					writePingtoDataBase(item->first,replyInfo->Status);
					writePingAlarmtoDataBase(item->second,replyInfo->Status);
				}

				free(ReplyBuffer);
			}

		}

		IcmpCloseHandle(hIcmpFile);
	}

}

bool GuangxiStationPlug::writePingtoDataBase(const std::string& computerName,INT result)
{
	wxString sqlSentence = wxT("INSERT INTO radar_net_status(updateTime, \
																fromAddress, \
																toAddress, \
																runStatus, \
																radarcd) VALUES");

	wxString sqlError;
	wxMutexLocker locker(*m_sqlMutex);

	if(m_pDataBase)
	{

		try
		{
			sqlSentence.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxString(m_siteInfo.c_str(),wxConvLocal)));
			sqlSentence.Append(wxString::Format(wxT("'%s', "), wxString(computerName.c_str(),wxConvLocal)));

			int connectionFlag = -1;
			if(result == 0)
			{
				connectionFlag = 0;
			}
			else
			{
				connectionFlag = 1;
			}
			sqlSentence.Append(wxString::Format(wxT("%d, "), connectionFlag));
			sqlSentence.Append(wxString::Format(wxT("'%s')"), wxString(m_siteInfo.c_str(),wxConvLocal)));

			if(!m_pDataBase->ExecuteNoQuery(sqlSentence,sqlError))
			{
				throw sqlError;
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

bool GuangxiStationPlug::writePingAlarmtoDataBase(const std::string& computerIP,INT result)
{

	if(result == 0)
	{
		if(m_computerDisconnectFlag[computerIP] == TRUE)
		{
			m_computerDisconnectFlag[computerIP] = FALSE;

			wxString sqlError;
			wxMutexLocker locker(*m_sqlMutex);
			if(m_pDataBase)
			{
				try
				{
					wxString sqlSentence; 

					wxString alarmMessage;
					alarmMessage.Append(wxString(computerIP.c_str(),wxConvLocal));
					alarmMessage.Append(wxT("网络异常，无法访问"));

					sqlSentence.Printf(wxT("UPDATE alarm_info SET endTime=%d WHERE endTime IS NULL AND content='%s' AND radarcd='%s'"), ::wxGetUTCTime(),
						alarmMessage,
						wxString(m_siteInfo.c_str(),wxConvLocal));

					if(!m_pDataBase->ExecuteNoQuery(sqlSentence,sqlError))
					{
						throw sqlError;
					}

					return true;
				}
				catch (wxString& error)
				{
					GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
					GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
				}

			}
		}
	}
	else
	{
		if(m_computerDisconnectFlag[computerIP] == FALSE)
		{
			m_computerDisconnectFlag[computerIP] = TRUE;

			wxString sqlSentence = wxT("INSERT INTO alarm_info (time, \
									   endTime, \
									   type, \
									   alarmName, \
									   level, \
									   mode, \
									   content, \
									   origin, \
									   radarcd) VALUES");
			wxString sqlError;
			wxMutexLocker locker(*m_sqlMutex);

			if(m_pDataBase)
			{

				try
				{
					sqlSentence.Append(wxString::Format(wxT("(%d, "), ::wxGetUTCTime()));
					sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
					sqlSentence.Append(wxString::Format(wxT("'%s', "),  wxT("net_run")));
					sqlSentence.Append(wxString::Format(wxT("%s, "), wxT("NULL")));
					sqlSentence.Append(wxString::Format(wxT("%d, "), 1));
					sqlSentence.Append(wxString::Format(wxT("'%s', "), wxT("0")));

					wxString alarmMessage;
					alarmMessage.Append(wxString(computerIP.c_str(),wxConvLocal));
					alarmMessage.Append(wxT("网络异常，无法访问"));
					sqlSentence.Append(wxString::Format(wxT("'%s', "), alarmMessage));

					sqlSentence.Append(wxString::Format(wxT("%d, "), 2));
					sqlSentence.Append(wxString::Format(wxT("'%s')"), wxString(m_siteInfo.c_str(),wxConvLocal)));

					if(!m_pDataBase->ExecuteNoQuery(sqlSentence,sqlError))
					{
						throw sqlError;
					}

					return true;


				}
				catch (wxString& error)
				{
					GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
					GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
				}

			}

		}
	}

	return false;
}

PCHAR GuangxiStationPlug::UnicodeToUtf8(const wchar_t* unicode)
{
	if (unicode)
	{
		int Len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
		PCHAR szUtf8 = new CHAR[Len + 1];
		memset(szUtf8, 0, Len + 1);
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, Len, NULL, NULL);
		return szUtf8;
	}

	return nullptr;
}

PWCHAR GuangxiStationPlug::Utf8ToUnicode(const char* utf8Str)
{
	if(utf8Str)
	{
		int Len = MultiByteToWideChar(CP_UTF8, NULL, utf8Str, strlen(utf8Str), NULL, 0);
		PWCHAR wszString = new WCHAR[Len + 1];
		MultiByteToWideChar(CP_UTF8, NULL, utf8Str, strlen(utf8Str), wszString, Len);
		wszString[Len] = '\0';
		return wszString;
	}

	return nullptr;
}

unsigned int GuangxiStationPlug::chkcrc(unsigned char *buf,unsigned char len)
{
	unsigned char hi,lo; 
	unsigned int i;  
	unsigned int crc;  
	crc=0xFFFF; 
	for(i=0;i<len;i++) 
	{ 
		crc=calccrc(*buf,crc); 
		buf++; 
	} 
	hi=( unsigned char)(crc%256); 
	lo=( unsigned char)(crc/256);
	crc=(((unsigned int)(hi))<<8)|lo; 
	return crc;

}


bool GuangxiStationPlug::crcCheck(unsigned char* buf,unsigned int len)
{
	if(buf && (len > 0))
	{
	
		CRCData CRCCheckData;
		CRCCheckData.crcData = chkcrc(buf,len - 2);
		if((buf[len-2] == CRCCheckData.crcByte[1]) && (buf[len-1] == CRCCheckData.crcByte[0]))
		{
			return true;
		}
	}

	return false;
}

void GuangxiStationPlug::setComputersIP(const std::map<std::string,std::string>& computersIP)
{
	m_computersIP = computersIP;

	m_computerDisconnectFlag.clear();
	for(auto item = m_computersIP.cbegin(); item != m_computersIP.cend(); item++)
	{
		m_computerDisconnectFlag[item->second] = FALSE;
	}

	clearComputersIPinXML();
	addComputersIPinXML();

}

bool GuangxiStationPlug::sendCmd(UINT index,const char *pData, UINT Len,const char *DstIP,UINT DstPort)
{
	std::string IPAddress(DstIP);
	if(IPAddress == m_envRadarIPAddress)
	{
		if(m_pUDPInstanceRadar[index]->IsWorking())
		{
			return m_pUDPInstanceRadar[index]->SendData(pData,Len,DstIP,DstPort);
		}
	}
	else if(IPAddress == m_envPowerIPAddress)
	{
	
		if(m_pUDPInstancePower[index]->IsWorking())
		{
			return m_pUDPInstancePower[index]->SendData(pData,Len,DstIP,DstPort);
		}
	}
	

	return false;

}


bool GuangxiStationPlug::getCenterIPAddress()
{
	wxString sqlError;
	wxMutexLocker locker(*m_sqlMutex);	
	if (m_pDataBase)
	{
		try
		{	
			char sqlQuery[128] = {'\0'};
			sprintf_s(sqlQuery,128,"SELECT content FROM system_info WHERE channel = 'centerIPAddress'");	
			int resultNum = m_pDataBase->ExecuteQuery(sqlQuery);

			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = m_pDataBase->GetStringValue(index,"content",length);
					if(sqlTmpStr)
					{
						m_CenterIPAddress = std::string(sqlTmpStr,std::strlen(sqlTmpStr));
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

bool GuangxiStationPlug::getEnvIPAddressRadar()
{
	wxString sqlError;
	wxMutexLocker locker(*m_sqlMutex);	
	if (m_pDataBase)
	{
		try
		{	
			char sqlQuery[128] = {'\0'};
			sprintf_s(sqlQuery,128,"SELECT content FROM system_info WHERE channel = 'envRadarIPAddress'");	
			int resultNum = m_pDataBase->ExecuteQuery(sqlQuery);

			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = m_pDataBase->GetStringValue(index,"content",length);
					if(sqlTmpStr)
					{
						m_envRadarIPAddress = std::string(sqlTmpStr,std::strlen(sqlTmpStr));
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

bool GuangxiStationPlug::getEnvIPAddressPower()
{
	wxString sqlError;
	wxMutexLocker locker(*m_sqlMutex);	
	if (m_pDataBase)
	{
		try
		{	
			char sqlQuery[128] = {'\0'};
			sprintf_s(sqlQuery,128,"SELECT content FROM system_info WHERE channel = 'envPowerIPAddress'");	
			int resultNum = m_pDataBase->ExecuteQuery(sqlQuery);

			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = m_pDataBase->GetStringValue(index,"content",length);
					if(sqlTmpStr)
					{
						m_envPowerIPAddress = std::string(sqlTmpStr,std::strlen(sqlTmpStr));
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

std::pair<UCHAR, UCHAR> GuangxiStationPlug::HexToASCII(UCHAR hexData)
{
	auto upperPart = hexData >> 4;
	auto lowerPart = hexData & 0x0F;

	std::pair<UCHAR, UCHAR> AsciiData = std::make_pair(0,0);
	AsciiData.first = (upperPart <= 9)? ('0' + upperPart - 0):('A' + upperPart - 10);
	AsciiData.second = (lowerPart <= 9)? ('0' + lowerPart - 0):('A' + lowerPart - 10);

	return AsciiData;

}

UCHAR GuangxiStationPlug::ASCIIToHex(std::pair<UCHAR,UCHAR> asciiData)
{
	auto upperPart = (asciiData.first <= '9')? (asciiData.first - '0'):(asciiData.first - 'A' + 10);
	auto lowerPart = (asciiData.second <= '9')? (asciiData.second - '0'):(asciiData.second - 'A' + 10);

	return upperPart * 16 + lowerPart;
}

FLOAT GuangxiStationPlug::UCHARToFloat(PUCHAR upsData)
{
	FLOAT upsAnalogData = 0.0;
	UCHAR upsAnalogDataChar[4] = {'\0'};
	if(upsData)
	{
		upsAnalogDataChar[0] = ASCIIToHex(std::make_pair(upsData[0],upsData[1]));
		upsAnalogDataChar[1] = ASCIIToHex(std::make_pair(upsData[2],upsData[3]));
		upsAnalogDataChar[2] = ASCIIToHex(std::make_pair(upsData[4],upsData[5]));
		upsAnalogDataChar[3] = ASCIIToHex(std::make_pair(upsData[6],upsData[7]));

		std::memcpy((PUCHAR)&upsAnalogData,upsAnalogDataChar,4);

	}

	return upsAnalogData;

}

bool GuangxiStationPlug::InitToolbarData()
{
	wxString FileName;
	FileName = wxString( m_IComm->GetWorkPath(),wxConvLocal);
	FileName += wxT("\\Config\\GuangxiStation\\GuangxiStationToolbar.xml");
	//if (!wxFileExists(FileName))
	{
		return false;
	}
	//return InitMenuData(g_ToolBarXML,MAX_TOOL_CON,FileName);
}

bool GuangxiStationPlug::InitMenuData(char* pData,int DataSize,wxString FileName)
{
	memset(pData,0,sizeof(char)*DataSize);
	FILE *fp = fopen(FileName.mb_str(wxConvLocal),"rb");
	if(fp!=NULL)
	{
		fseek(fp,0,SEEK_END);
		int Size = ftell(fp);
		if (Size>=0)
		{
			fseek(fp,0,SEEK_SET);
			fread(pData,Size,1,fp);
		}
		fclose(fp);
	}
	else
	{
		return false;
	}
	if (strlen(pData) <= 0)
	{
		return false;
	}
	return true;
}

