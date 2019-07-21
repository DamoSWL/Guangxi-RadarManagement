// PluginTest.cpp : Defines the entry point for the DLL application.
//
#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangXiMain.h"

GuangXiPlug* g_pMain = NULL;

static const int STATUSTIMERID = 60;

BEGIN_EVENT_TABLE(GuangXiPlug,wxEvtHandler)
	EVT_TIMER(STATUSTIMERID,GuangXiPlug::onStatusTimeout)
END_EVENT_TABLE()

// 全局函数
GuangXiPlug* GetPlug() 
{ 
	return g_pMain;
};
PL_GUID	g_DependGuid[10];
const char *g_szDependPluginName[10];

GuangXiPlug::GuangXiPlug():
  m_IHost(NULL),m_IComm(NULL),
  m_JishujuCnt(0),
  m_ZhuangtaiCnt(0),
  m_ZhuangtaiPreviousTime(0),
  m_ZhuangtaiOneMin(false),
  m_alarmFlag(false),
  m_excessiveFlag(false),
  m_StatusTimer(this,STATUSTIMERID)
   // ,m_IEditorM(NULL)
  {
	g_pMain = this;
	////////////////
	m_bExit = false;
	m_WorkPath = wxEmptyString;
}


bool GuangXiPlug::OnCommandHandle(
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
		
		
	if( wID == 22 )
	{
		m_IHost->ShowPanel( m_panel,true );
	}
	else{
		m_IHost->ShowPanel( m_panel,false );
	}
		
	if( wID == 13 )
	{
	/*	if( !m_IEditorM->IsEditorShow( m_editor.guid ))
			m_IEditorM->ShowEditor( m_editor.guid, true );*/
	}

	if(wID == 11)
	{
		m_IHost->ShowTheSetDlg(g_guid_Set);

	}
	
	return true;
}

//! 初始化完毕后调用, 插件在此函数中进行UI初始化等操作
void GuangXiPlug::OnInitialized()
{
	//// 获取需要的接口
	m_IHost = (IHostPlugin*)m_IFrame->QueryInterfaceP( g_PL_GRSFrame, g_I_Host );
	m_IComm = (CDYW::Plugin::ICommon *)m_IFrame->QueryInterfaceP( g_PL_GRSFrame, guid_I_Common );
	if( m_IComm ) {
		m_WorkPath = wxString(m_IComm->GetWorkPath(),wxConvLocal);
		m_IComm->AddLog( 0, "插件[  GuangXi ]..." );
	}
	
		//m_IEditorM = (IEditorManager *)m_IFrame->QueryInterfaceP( g_PL_GRSFrame, guid_I_EditorManager );
	
	//// 注册工具栏 和 菜单
	if( m_IHost ) 
	{
				
				//// 注册工具栏
		//memset( &m_toolbar, 0, sizeof(IPluginToolBar));
		//m_toolbar.bNewBar = true;
		//m_toolbar.pluginGUID = g_PL_GuangXi;
		//m_toolbar.parGUID = guid_NULL;
		//m_toolbar.selfGUID = g_guid_toolbar;
		//strcpy( m_toolbar.name, "TB_GuangXi" );
		//m_toolbar.barDefine = (char*)g_ToolBarXML;
		//m_toolbar.lSize = strlen(g_ToolBarXML);

		//m_IHost->RegisterToolBar( m_toolbar );

		m_menu.pluginGUID = g_PL_GuangXi;
		m_menu.parGUID = guid_SetConfig;
		m_menu.selfGUID = g_guid_menu;
		m_menu.menuDefine = (char*)g_MenuXML;
		//m_menu.lSize = strlen(g_MenuXML);
		m_IHost->RegisterMenu( m_menu );
				
			//// 注册参数设置面板
		//memset( &m_DlgPara, 0 , sizeof(IPluginDlgPara));
		strcpy( m_DlgPara.cName, "系统路径设置" );
		m_DlgPara.selfGUID = g_guid_Set;
		m_DlgPara.parGUID = guid_SET_GRS;
		m_DlgPara.pluginGUID = g_PL_GuangXi;
		m_DlgPara.funCreate = GuangXiSet::CreatePage;

		m_IHost->RegisterSettingDialog( m_DlgPara );
				
			////  注册普通面板
		//memset( &m_panel, 0, sizeof(IPluginDlgPara));
// 		strcpy( m_panel.cName, "GuangXi" );
// 		m_panel.pluginGUID = g_PL_GuangXi;
// 		m_panel.selfGUID = g_guid_panel;
// 		m_panel.parGUID = guid_ConfigBook;
// 		m_panel.funCreate = GuangXiPanel::CreatePage;
// 		m_panel.funDelete = GuangXiPanel::DeletePage;
// 		m_panel.lStyle = PANE_DEF_STYLE;

		m_IHost->RegisterPanel( m_panel );
				
				/// 注册编辑器
		//wxTextCtrl *wxEditorW =  new wxTextCtrl( (wxWindow*)m_IEditorM->GetManagerHwnd(), wxID_ANY, wxT("Some text"),
		//	wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER);
		//m_editor.hWnd = (PL_HWND)(wxWindow*)wxEditorW;
		//m_editor.bChange = true;
		//m_editor.guid = g_guid_Editor;

		//m_IEditorM->RegEditor( (IEditor*)(&m_editor ), true);
	}
}

void GuangXiPlug::OnExit()
{
	m_bExit = true;


	m_pDataBase->CloseDatabase();
	delete m_pDataBase;
	delete m_sqlMutex;
	delete m_xmlMutex;

	DeleteCriticalSection(m_BaseDataFTPCritical);
	DeleteCriticalSection(m_ProductFTPCritical);
	DeleteCriticalSection(m_StateDataFTPCritical);
	DeleteCriticalSection(m_PintuFTPCritical);

	m_StatusTimer.Stop();
	m_FTPCheck.quit();
	m_alarmFileCheck.quit();
	m_fileGenerationCheck.quit();
	m_softwareCheck.stopTimer();
}

void GuangXiPlug::OnActived()
{
	m_bExit = false;

	m_pDataBase = new MySqlDatabase();
	m_sqlMutex = new wxMutex(wxMUTEX_DEFAULT);
	m_xmlMutex = new wxMutex(wxMUTEX_DEFAULT);

	m_BaseDataFTPCritical = new CRITICAL_SECTION;
	m_ProductFTPCritical = new CRITICAL_SECTION;
	m_StateDataFTPCritical = new CRITICAL_SECTION;
	m_PintuFTPCritical = new CRITICAL_SECTION;
	
	InitializeCriticalSection(m_BaseDataFTPCritical);
	InitializeCriticalSection(m_ProductFTPCritical);
	InitializeCriticalSection(m_StateDataFTPCritical);
	InitializeCriticalSection(m_PintuFTPCritical);

	if(loadBasicInfo())
	{
		AddLog(LOG_TYPE_MESSAGE,"basicInfo.xml配置文件读取成功");
	}
	else
	{
		AddLog(LOG_TYPE_ERROR,"basicInfo.xml配置文件读取失败");
		return;
	}

	if(loadConfig())
	{
		AddLog(LOG_TYPE_MESSAGE,"config.xml配置文件读取成功");
	}
	else
	{
		AddLog(LOG_TYPE_ERROR,"config.xml配置文件读取失败");
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

	getRadarType();


	if(m_computerType == "PUP2")
	{
		m_StatusTimer.Start(60*1000);
	}


	m_softwareCheck.startTimer();

	m_FTPCheck.fileMonitor();
	m_alarmFileCheck.fileMonitor();
	m_fileGenerationCheck.fileMonitor();
}

void GuangXiPlug::OnDestroy()
{

}

void * GuangXiPlug::QueryInterfaceP(PL_IID guid)
{
	if( IsEqualID( guid, g_I_GuangXi ) )
	{
		return (IGuangXi*)(g_pMain); 
	}
	return NULL;  
}

PL_GUID * GuangXiPlug::GetDependenceList(int &cnt ,const char ***ppszDepNames)
{
	cnt = 0;
	if( ppszDepNames != NULL )
	{
		*ppszDepNames = g_szDependPluginName;
	}
	return g_DependGuid;
}

void GuangXiPlug::setFileDirConfig(const fileDirConfig& config)
{
	 m_fileDirConfig = config;
	 wxMutexLocker locker(*m_xmlMutex);

	 pugi::xml_node root = m_xmlDoc.child("config");
	 pugi::xml_node fileDir = root.child("fileDir");

	 pugi::xml_node JishujuDir = fileDir.child("JishujuDir");
	 pugi::xml_node ProductDir = fileDir.child("ProductDir");
	 pugi::xml_node ZhuangtaiDir = fileDir.child("ZhuangtaiDir");
	 pugi::xml_node BaojingDir = fileDir.child("BaojingDir");
	 pugi::xml_node PingtuDir = fileDir.child("PingtuDir");


	 PCHAR JishujuDirStr = UnicodeToUtf8(config.JishuDir.c_str());
	 PCHAR ProductDirStr = UnicodeToUtf8(config.ProductDir.c_str());
	 PCHAR ZhuangtaiDirStr = UnicodeToUtf8(config.ZhuangtaiDir.c_str());
	 PCHAR BaojingDirStr = UnicodeToUtf8(config.BaojingDir.c_str());
	 PCHAR PingtuDirStr =  UnicodeToUtf8(config.PingtuDir.c_str());

	 JishujuDir.text().set(JishujuDirStr);
	 ProductDir.text().set(ProductDirStr);
	 ZhuangtaiDir.text().set(ZhuangtaiDirStr);
	 BaojingDir.text().set(BaojingDirStr);
	 PingtuDir.text().set(PingtuDirStr);

	 wxString fileName = GetWorkPath();
	 fileName += wxT("\\Config\\Config.xml");
	 if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	 {
		 GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存失败");
	 }

	 delete[] JishujuDirStr;
	 delete[] ProductDirStr;
	 delete[] ZhuangtaiDirStr;
	 delete[] BaojingDirStr;
	 delete[] PingtuDirStr;


}

void GuangXiPlug::setFTPDirConfig(const FTPDirConfig& config)
{
	m_FTPDirConfig = config;
	clearFtpDirinXML();
	addFtpDirinXML();

}

void GuangXiPlug::clearFtpDirinXML()
{
	wxMutexLocker locker(*m_xmlMutex);
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node ftpDir = root.child("FtpDir");

	std::set<pugi::xml_attribute> attributeNode;
	for(auto item = ftpDir.attributes_begin(); item != ftpDir.attributes_end(); item++)
	{
		if(!item->empty())
		{
			attributeNode.insert(*item);
		}
	}

	for(auto item = attributeNode.begin(); item != attributeNode.end(); item++)
	{
		ftpDir.remove_attribute(*item);
	}
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtpDir失败");
	}

}

void GuangXiPlug::addFtpDirinXML()
{
	wxMutexLocker locker(*m_xmlMutex);
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node ftpDir = root.child("FtpDir");


	if(!m_FTPDirConfig.FTPJishuDir.IsEmpty())
	{
		auto attributeNode = ftpDir.append_attribute("Dir_1");
		PCHAR attributeValue = UnicodeToUtf8(m_FTPDirConfig.FTPJishuDir.c_str());
		attributeNode.set_value(attributeValue);
		delete[] attributeValue;
	}
	if(!m_FTPDirConfig.FTPProductDir.IsEmpty())
	{
		auto attributeNode = ftpDir.append_attribute("Dir_2");
		PCHAR attributeValue = UnicodeToUtf8(m_FTPDirConfig.FTPProductDir.c_str());
		attributeNode.set_value(attributeValue);
		delete[] attributeValue;
	}
	if(!m_FTPDirConfig.FTPZhuangtaiDir.IsEmpty())
	{
		auto attributeNode = ftpDir.append_attribute("Dir_3");
		PCHAR attributeValue = UnicodeToUtf8(m_FTPDirConfig.FTPZhuangtaiDir.c_str());
		attributeNode.set_value(attributeValue);
		delete[] attributeValue;
	}
	if(!m_FTPDirConfig.FTPPintuDir.IsEmpty())
	{
		auto attributeNode = ftpDir.append_attribute("Dir_4");
		PCHAR attributeValue = UnicodeToUtf8(m_FTPDirConfig.FTPPintuDir.c_str());
		attributeNode.set_value(attributeValue);
		delete[] attributeValue;
	}
	

	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtpDir失败");
	}
}

void GuangXiPlug::onStatusTimeout(wxTimerEvent& event)
{
	m_JishujuCnt++;
	m_ZhuangtaiCnt++;

	auto currentDateTime = wxDateTime::Now();
	auto month = currentDateTime.GetMonth() + 1;
	auto day = currentDateTime.GetDay();
	auto hour = currentDateTime.GetHour();

	if(m_alarmFlag)
	{
		writeStatusToDataBase(ALARM);
	}
	else if(((month >= 10) && (month <= 12)) || ((month >= 1) && (month <=3)) || ((month == 4) && (day <= 15)))
	{
		if(((hour >= 15) && (hour <= 24)) || ((hour >= 0) && (hour <= 10)))
		{
			writeStatusToDataBase(NONEOBSERVE);
		}
		else
		{
			if(m_excessiveFlag)
			{
				writeStatusToDataBase(EXCESSIVE);
			}
			else if((m_ZhuangtaiOneMin) && (m_JishujuCnt > 6))
			{
				writeStatusToDataBase(STANDBY);
			}
			else if((m_JishujuCnt >= 30) && (m_ZhuangtaiCnt >= 30))
			{
				writeStatusToDataBase(NODATA);
			}
			else 
			{
				writeStatusToDataBase(NORMAL);
			}
		}
	}
	else
	{
		if(m_excessiveFlag)
		{
			writeStatusToDataBase(EXCESSIVE);
		}
		else if((m_ZhuangtaiOneMin) && (m_JishujuCnt > 6))
		{
			writeStatusToDataBase(STANDBY);
		}
		else if((m_JishujuCnt >= 30) && (m_ZhuangtaiCnt >= 30))
		{
			writeStatusToDataBase(NODATA);
		}
		else 
		{
			writeStatusToDataBase(NORMAL);
		}
	}

}

bool GuangXiPlug::writeStatusToDataBase(RADARSTATUS status)
{
	wxString sqlSentence = wxT("INSERT INTO radar_status (updateTime, \
															status, \
															radarcd) VALUES");

	wxString sqlError;
	wxMutexLocker locker(*m_sqlMutex);

	if(m_pDataBase)
	{
		try
		{
			sqlSentence.Append(wxString::Format(wxT("(%d, "),::wxGetUTCTime()));
			sqlSentence.Append(wxString::Format(wxT("%d, "), (UINT)status));
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

void GuangXiPlug::setBaseDataTime(UINT time)
{

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node updateTime = root.child("updateTime");
	pugi::xml_node baseDataTime = updateTime.child("baseDataTime");

	baseDataTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存baseDataTime失败");
	}

}

void GuangXiPlug::setProductTime(UINT time)
{
	wxMutexLocker locker(*m_xmlMutex);

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node updateTime = root.child("updateTime");
	pugi::xml_node productTime = updateTime.child("productTime");

	productTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存productTime失败");
	}
}

void GuangXiPlug::setZhuangtaiTime(UINT time)
{
	wxMutexLocker locker(*m_xmlMutex);

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node updateTime = root.child("updateTime");
	pugi::xml_node zhuangdingTime = updateTime.child("zhuangtaiTime");

	zhuangdingTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存zhuangtaiTime失败");
	}
}

void GuangXiPlug::setPintuTime(UINT time)
{
	wxMutexLocker locker(*m_xmlMutex);

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node updateTime = root.child("updateTime");
	pugi::xml_node pintuTime = updateTime.child("pintuTime");

	pintuTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存pintuTime失败");
	}
}

void GuangXiPlug::setAlarmTime(UINT time)
{
	wxMutexLocker locker(*m_xmlMutex);
	
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node updateTime = root.child("updateTime");
	pugi::xml_node alarmTime = updateTime.child("alarmTime");

	alarmTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存alarmTime失败");
	}
}

void GuangXiPlug::setFtpBaseDataTime(UINT time)
{
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node FtpDir = root.child("FtpDir");
	pugi::xml_node baseDataTime = FtpDir.child("baseDataTime");

	baseDataTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtpbaseDataTime失败");
	}
}

void GuangXiPlug::setFtpProductTime(UINT time)
{
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node FtpDir = root.child("FtpDir");
	pugi::xml_node productTime = FtpDir.child("productTime");

	productTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtpproductTime失败");
	}
}

void GuangXiPlug::setFtpZhuangtaiTime(UINT time)
{
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node FtpDir = root.child("FtpDir");
	pugi::xml_node zhuangtaiTime = FtpDir.child("zhuangtaiTime");

	zhuangtaiTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtpzhuangtaiTime失败");
	}
}

void GuangXiPlug::setFtpPintuTime(UINT time)
{
	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node FtpDir = root.child("FtpDir");
	pugi::xml_node pintuTime = FtpDir.child("pintuTime");

	pintuTime.text().set(time);
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	if(!m_xmlDoc.save_file(fileName.c_str(),"\t",1U,pugi::encoding_utf8))
	{
		GetPlug()->AddLog(LOG_TYPE_MESSAGE, "Config.xml配置文件保存FtppintuTime失败");
	}
}


void GuangXiPlug::newBaseFileArrival()
{
	m_JishujuCnt = 0;
}

void GuangXiPlug::newZhuangtaiFileArrival(UINT time)
{

	if(m_ZhuangtaiPreviousTime != 0)
	{
		if((time - m_ZhuangtaiPreviousTime) > 120)
		{
			m_ZhuangtaiOneMin = false;
		}
		else
		{
			m_ZhuangtaiOneMin = true;
		}
	}
	m_ZhuangtaiPreviousTime = time;
	m_ZhuangtaiCnt = 0;
}

void GuangXiPlug::setAlarmFlag(INT flag)
{
	if(flag == 1)
	{
		m_alarmFlag = true;
	}
	else if(flag == 2)
	{
		m_alarmFlag = false;
	}
	else{}
}

UINT GuangXiPlug::fileModificationTime(const wxString& fileName)
{
	auto indexStart = std::strlen("Z_RADR_I_Z977X_");
	auto fileTimeStr = fileName.Mid(indexStart,14);

	auto yearStr = fileTimeStr.Left(4);
	auto monthStr = fileTimeStr.Mid(4,2);
	auto dayStr = fileTimeStr.Mid(6,2);
	auto hourStr = fileTimeStr.Mid(8,2);
	auto minStr = fileTimeStr.Mid(10,2);
	auto secStr= fileTimeStr.Mid(12,2);

	ULONG year = 0;
	ULONG month = 0;
	ULONG day = 0;
	ULONG hour = 0;
	ULONG min = 0;
	ULONG sec = 0;

	yearStr.ToULong(&year);
	monthStr.ToULong(&month);
	dayStr.ToULong(&day);
	hourStr.ToULong(&hour);
	minStr.ToULong(&min);
	secStr.ToULong(&sec);

	struct tm fileTime;
	fileTime.tm_year = year - 1900;
	fileTime.tm_mon = month - 1;
	fileTime.tm_mday = day;
	fileTime.tm_hour = hour;
	fileTime.tm_min = min;
	fileTime.tm_sec = sec;

	return static_cast<UINT>(std::mktime(&fileTime));
}

bool GuangXiPlug::loadBasicInfo()
{
	wxString fileName = GetWorkPath() + wxT("\\Config\\basicInfo.xml");
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());\
	if(result.status !=pugi::status_ok)
	{
		return false;
	}

	pugi::xml_node root = doc.child("basicInfo");
	pugi::xml_node siteInfo = root.child("siteInfo");
	pugi::xml_node computerType = root.child("computerType");

	m_siteInfo.assign(siteInfo.text().as_string());
	m_computerType.assign(computerType.text().as_string());

	return true;
}

bool GuangXiPlug::loadConfig()
{
	wxString fileName = GetWorkPath() + wxT("\\Config\\Config.xml");
	
	pugi::xml_parse_result result = m_xmlDoc.load_file(fileName.c_str());
	if(result.status !=pugi::status_ok)
	{
		return false;
	}

	pugi::xml_node root = m_xmlDoc.child("config");
	pugi::xml_node database = root.child("database");

	m_dataBaseConfig.hostname.assign(database.child("hostname").text().as_string());
	m_dataBaseConfig.user.assign(database.child("user").text().as_string());
	m_dataBaseConfig.password.assign(database.child("password").text().as_string());
	m_dataBaseConfig.databasename.assign(database.child("databasename").text().as_string());

	pugi::xml_node fileDir = root.child("fileDir");

	PWCHAR JishujuDir = Utf8ToUnicode(fileDir.child("JishujuDir").text().as_string());
	PWCHAR ProductDir = Utf8ToUnicode(fileDir.child("ProductDir").text().as_string());
	PWCHAR ZhuangtaiDir = Utf8ToUnicode(fileDir.child("ZhuangtaiDir").text().as_string());
	PWCHAR PingtuDir = Utf8ToUnicode(fileDir.child("PingtuDir").text().as_string());
	PWCHAR BaojingDir = Utf8ToUnicode(fileDir.child("BaojingDir").text().as_string());
	

	m_fileDirConfig.JishuDir = wxString(JishujuDir);
	m_fileDirConfig.ProductDir = wxString(ProductDir);
	m_fileDirConfig.ZhuangtaiDir = wxString(ZhuangtaiDir);
	m_fileDirConfig.PingtuDir = wxString(PingtuDir);
	m_fileDirConfig.BaojingDir = wxString(BaojingDir);
	

	delete[] JishujuDir;
	delete[] ProductDir;
	delete[] ZhuangtaiDir;
	delete[] PingtuDir;
	delete[] BaojingDir;


	pugi::xml_node updateTime = root.child("updateTime");
	m_updateTime.baseDataTime = updateTime.child("baseDataTime").text().as_uint();
	m_updateTime.productTime = updateTime.child("productTime").text().as_uint();
	m_updateTime.zhuangtaiTime = updateTime.child("zhuangtaiTime").text().as_uint();
	m_updateTime.pintuTime = updateTime.child("pintuTime").text().as_uint();
	m_updateTime.alarmTime = updateTime.child("alarmTime").text().as_uint();

	pugi::xml_node ftpDir = root.child("FtpDir");
	
	for(auto item = ftpDir.attributes_begin();item != ftpDir.attributes_end(); item++)
	{
		wxString fileTypeStr(item->name(),wxConvLocal);
		fileTypeStr.Remove(0,4);
		long fileType = 0;
		fileTypeStr.ToLong(&fileType);

		if((fileType < 1) || (fileType > 4))
		{
			AddLog(LOG_TYPE_ERROR,"FTP文件类型值异常,不在1-4范围内");
			return false;
		}

		PWCHAR Dir = Utf8ToUnicode(item->value());
		wxString tmpDir(Dir);
		delete[] Dir;

		switch(fileType)
		{
			case 1:
				m_FTPDirConfig.FTPJishuDir = tmpDir;
			break;

			case 2:
				m_FTPDirConfig.FTPProductDir = tmpDir;
			break;

			case 3:
				m_FTPDirConfig.FTPZhuangtaiDir = tmpDir;
			break;

			case 4:
				m_FTPDirConfig.FTPPintuDir = tmpDir;
			break;
			
			default:
			break;

		}

	}

	m_FtpUpdateTime.baseDataTime = ftpDir.child("baseDataTime").text().as_uint();
	m_FtpUpdateTime.productTime = ftpDir.child("productTime").text().as_uint();
	m_FtpUpdateTime.zhuangtaiTime = ftpDir.child("zhuangtaiTime").text().as_uint();
	m_FtpUpdateTime.pintuTime = ftpDir.child("pintuTime").text().as_uint();

	return true;

}

void GuangXiPlug::getRadarType()
{
	wxMutexLocker locker(*m_sqlMutex);	
	if (m_pDataBase)
	{
		try
		{	
			std::string radarType;
			char sqlQuery[128] = {'\0'};
			sprintf_s(sqlQuery,128,"SELECT content FROM system_info WHERE channel = 'radarType'");	
			int resultNum = m_pDataBase->ExecuteQuery(sqlQuery);

			if(resultNum != 0)
			{
				for(int index = 0; index < resultNum; index++)
				{
					int length = 0;
					char* sqlTmpStr = m_pDataBase->GetStringValue(index,"content",length);
					if(sqlTmpStr)
					{
						m_radarType.append(sqlTmpStr,std::strlen(sqlTmpStr));
					}
				}
			}
		}
		catch (wxString& error)
		{
			GetPlug()->AddLog(LOG_TYPE_MESSAGE,__FUNCTION__);
			GetPlug()->AddLog(LOG_TYPE_ERROR,error.mb_str(wxConvLocal));
		}
	}

}

PCHAR GuangXiPlug::UnicodeToUtf8(const wchar_t* unicode)
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

PWCHAR GuangXiPlug::Utf8ToUnicode(const char* utf8Str)
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

bool GuangXiPlug::InitToolbarData()
{
	wxString FileName;
	FileName = wxString( m_IComm->GetWorkPath(),wxConvLocal);
	FileName += wxT("\\Config\\GuangXi\\GuangXiToolbar.xml");
//	if (!wxFileExists(FileName))
	{
		return false;
	}
	//return InitMenuData(g_ToolBarXML,MAX_TOOL_CON,FileName);
}

bool GuangXiPlug::InitMenuData(char* pData,int DataSize,wxString FileName)
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

