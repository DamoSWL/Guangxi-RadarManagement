
#ifndef IGRSFrame_h__
#define IGRSFrame_h__

#include <wx/aui/aui.h>
#include "../LibWRISFramework/Includes.h"
/*! \class IGRSFrame
	�������ӿ�
*/
class IGRSFrame
{
public:

	///// wxAuiToolbar
	//! ��ȡID��ʶ�Ĺ�����������ù�������ť��ID
	virtual wxAuiToolBar* GetToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, unsigned long& id ) = 0;
	
	/*! ��ʾ�����ع�����
		\param toolbar �������ṹ��
		\param bShow true����ʾ�������� false�����ع�����
	*/
	virtual void ShowToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, bool bShow ) = 0;

	/*! ָ���Ĺ������Ƿ���ʾ
		\param toolbar �������ṹ��
	*/
	virtual bool IsToolBarShow( CDYW::PluginSystem::IPluginToolBar &toolbar ) = 0;

	/*! Enable/Disable������
		\param toolbar �������ṹ
		\param bEnable ture,Enable Toolbar; false, Disable Toolbar
	*/
	virtual void EnableToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, bool bEnable ) = 0;

	/*! �����ƶ��Ĺ������Ƿ���Enable
		\param toolbar �������ṹ
	*/
	virtual bool IsToolBarEnable( CDYW::PluginSystem::IPluginToolBar &toolbar ) = 0;


	/*! ������������������
		�˹������ڱ���̫���Aui���¡������ܱ����� StopBatchOperToolBar �����ͬһ�߳���ʹ�ã�
	*/
	virtual void StartBatchOperToolBar() = 0;

	/*! ��ֹ��������������
	*/
	virtual void StopBatchOperToolBar( CDYW::PluginSystem::IPluginToolBar &toolBar ) = 0;

	/*! Enable/Disable �������ϵİ�ť
		\param toolbar �������ṹ
		\param src_id ��ť����xml�������id
		\param bEnable ture,Enable Tool; false, Disable Tool
	*/
	virtual bool EnableToolBarTool( CDYW::PluginSystem::IPluginToolBar &toolbar, int src_id, bool bEnable ) = 0;

	/*! Toggle/UnToggle �������ϵİ�ť
		\param toolbar �������ṹ
		\param src_id ��ť����xml�������id
		\param bEnable ture,Enable Tool; false, Disable Tool
	*/
	virtual bool ToggleToolBarTool( CDYW::PluginSystem::IPluginToolBar &toolbar, int src_id, bool bToggle ) = 0;

	/*! �ؼ��㹤������С
		\param pTB ������ָ��
	*/
	virtual void RealizeToolBar( wxAuiToolBar* pTB ) = 0;


	virtual wxAuiManager& GetAuiManager() = 0;

	//! ��ʾ�������ʱ����Ϣ
	virtual void DisplaySoftRunTime( bool bEnable ) = 0;

	//! ��ȡϵͳ��״̬��
	virtual wxStatusBar& GetStatusBar() = 0;

	//! ��ȡӦ�ó����ͼ��
	virtual wxIcon GetAppIcon() = 0;

	/*! �ƶ�������Ƿ���ʾ
		\param panel ���

		\return true,��ʾ;false,����
	*/
	virtual bool IsPanelShow( CDYW::PluginSystem::IPluginDlgPara &panel ) = 0;
};


/*! \brief GRSFrameID  {079FFFB9-01E0-4e07-B7EA-3FBBC42A719E}
*/
static const PL_GUID guid_I_GRSFrame =
{ 0x5d4a9885, 0xb0fb, 0x4216, { 0xa8, 0x78, 0xd3, 0x27, 0x3, 0x7, 0x51, 0x7d } };


#endif // IGRSFrame_h__





