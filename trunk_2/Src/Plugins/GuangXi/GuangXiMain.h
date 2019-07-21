
/* \file GuangXiMain.h
	\brief GuangXi插件类定义
	\ingroup GuangXiGroup
	
	本文件定义了GuangXiPlug 类。

	\version 1.0
	\author 
	\date 20171117

	\b 修改记录：
	\arg \c version 1.0， <author>，20171117 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangXiGroup
*/
#ifndef _HEADER_GuangXi_MAIN_20171117
#define _HEADER_GuangXi_MAIN_20171117

#include <ctime>
#include "GuangXiMenu.h"
#include "GuangXiToolBar.h"
#include "GuangXiPanel.h"
#include "GuangXiSet.h"
#include "GuangXiEditor.h"
#include "mysqldatabase.h"
#include "pugixml.hpp"

#include "FTPCheck.h"
#include "AlarmFileCheck.h"
#include "FileGenerationCheck.h"

#include "SoftwareCheck.h"



typedef struct 
{
	std::string hostname;
	std::string user;
	std::string password;
	std::string databasename;	
}databaseConfig;

typedef struct
{
	wxString JishuDir;
	wxString ProductDir;
	wxString ZhuangtaiDir;
	wxString PingtuDir;
	wxString BaojingDir;
}fileDirConfig;

typedef struct
{
	UINT baseDataTime;
	UINT productTime;
	UINT zhuangtaiTime;
	UINT pintuTime;
	UINT alarmTime;

}updateTime;

typedef struct
{
	UINT baseDataTime;
	UINT productTime;
	UINT zhuangtaiTime;
	UINT pintuTime;
}FTPUpdateTime;

typedef struct
{
	wxString FTPJishuDir;
	wxString FTPProductDir;
	wxString FTPZhuangtaiDir;
	wxString FTPPintuDir;
}FTPDirConfig;

enum RADARSTATUS
{
	NORMAL = 1,
	ALARM,
	NODATA,
	NONEOBSERVE,
	EXCESSIVE,
	STANDBY
};



using namespace CDYW::PluginSystem;

/*! \class GuangXiPlug
	\brief GuangXi插件。
	\ingroup GuangXiGroup
*/
class GuangXiPlug:public wxEvtHandler,
	public IPluginHelper< GuangXiPlug >,
	public IGuangXi
{
	CDYW::Plugin::ICommon *m_IComm;
	bool m_bExit;
	wxString m_WorkPath;
public:
	// 接口定义
	IHostPlugin *m_IHost;
		//IEditorManager *m_IEditorM;
		// 成员变量定义
		IPluginToolBar m_toolbar;
		//GuangXiEditor m_editor;
		
		IPluginMenu m_menu;
		IPluginDlgPara m_DlgPara;
		IPluginDlgPara m_panel;
public:
	GuangXiPlug();

	bool OnCommandHandle(PL_WORD wNotifyCode,PL_WORD wID,void * hWndCtl);

	//! 初始化完毕后调用, 插件在此函数中进行UI初始化等操作
	void OnInitialized();
	//! 获取插件的GUID
	PL_PID GetGUID()		{return g_PL_GuangXi;}
	//! 获取插件名称
	const char *GetName()	{return ("GuangXi");}
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
	const fileDirConfig& getFileDirConfig() {return m_fileDirConfig;} 
	void setFileDirConfig(const fileDirConfig& config);

	const FTPDirConfig& getFTPDirConfig() {return m_FTPDirConfig;}
	void setFTPDirConfig(const FTPDirConfig& config);

	const wxString& getJishuDir() {return m_fileDirConfig.JishuDir;}
	const wxString& getProductDir() {return m_fileDirConfig.ProductDir;}
	const wxString& getZhuangtaiDir() {return m_fileDirConfig.ZhuangtaiDir;}
	const wxString& getPintuDir() {return m_fileDirConfig.PingtuDir;}
	const wxString& getBaojingDir() {return m_fileDirConfig.BaojingDir;}

	const wxString& getFTPBaseDataDir() {return m_FTPDirConfig.FTPJishuDir;}
	const wxString& getFTPProductDir() {return m_FTPDirConfig.FTPProductDir;}
	const wxString& getFTPStatusDataDir() {return m_FTPDirConfig.FTPZhuangtaiDir;}

	const std::string& getSiteInfo() {return m_siteInfo;}
	const std::string& getComputerType() {return m_computerType;}
	const std::string& getRadarTypeStr() {return m_radarType;} 

	wxMutex* getSqlMutex() {return m_sqlMutex;}
	MySqlDatabase* getDatabase() const {return m_pDataBase;}
	const updateTime& getUpdateTime() {return m_updateTime;}
	const FTPUpdateTime& getFtpUpdateTime() {return m_FtpUpdateTime;}

	void setBaseDataTime(UINT time);
	void setProductTime(UINT time);
	void setZhuangtaiTime(UINT time);
	void setPintuTime(UINT time);
	void setAlarmTime(UINT time);

	void setFtpBaseDataTime(UINT time);
	void setFtpProductTime(UINT time);
	void setFtpZhuangtaiTime(UINT time);
	void setFtpPintuTime(UINT time);

	void newBaseFileArrival();
	void newZhuangtaiFileArrival(UINT time);
	void setAlarmFlag(INT flag);
	void setExcessiveFlag(bool flag) {m_excessiveFlag = flag;}

	UINT fileModificationTime(const wxString& fileName);

	CRITICAL_SECTION* getBaseDataFTPCritical() {return m_BaseDataFTPCritical;}
	CRITICAL_SECTION* getProductFTPCritical() {return m_ProductFTPCritical;}
	CRITICAL_SECTION* getStateFTPCritical() {return m_StateDataFTPCritical;}
	CRITICAL_SECTION* getPintuFTPCritical() {return m_PintuFTPCritical;}


private:
	bool loadBasicInfo();
	bool loadConfig();
	void getRadarType();

	void clearFtpDirinXML();
	void addFtpDirinXML();

	void onStatusTimeout(wxTimerEvent& event);
	bool writeStatusToDataBase(RADARSTATUS status);

private:
	PCHAR UnicodeToUtf8(const wchar_t* unicode);
	PWCHAR Utf8ToUnicode(const char* utf8Str);

private:
	std::string m_siteInfo;
	std::string m_computerType;
	std::string m_radarType;
	
	MySqlDatabase* m_pDataBase;
	wxMutex* m_sqlMutex;
	wxMutex* m_xmlMutex;
	pugi::xml_document m_xmlDoc;
	
	databaseConfig m_dataBaseConfig;

	fileDirConfig m_fileDirConfig;
	updateTime m_updateTime;
	FTPDirConfig m_FTPDirConfig;
	FTPUpdateTime m_FtpUpdateTime;

	CFTPCheck m_FTPCheck;
	CAlarmFileCheck m_alarmFileCheck;
	CFileGenerationCheck m_fileGenerationCheck;
	CSoftwareCheck m_softwareCheck;

	wxTimer m_StatusTimer;
	UINT m_JishujuCnt;
	UINT m_ZhuangtaiCnt;
	bool m_ZhuangtaiOneMin;
	UINT m_ZhuangtaiPreviousTime;
	bool m_alarmFlag;
	bool m_excessiveFlag;

	CRITICAL_SECTION* m_BaseDataFTPCritical;
	CRITICAL_SECTION* m_ProductFTPCritical;
	CRITICAL_SECTION* m_StateDataFTPCritical;
	CRITICAL_SECTION* m_PintuFTPCritical;		


	DECLARE_EVENT_TABLE()	

};


#endif

