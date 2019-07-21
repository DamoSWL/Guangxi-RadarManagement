/*********************************************************/
/*! \file IEditor.h
	\brief �༭���ӿ���

	\version 1.0
	\author �涨��
	\date 2008��06��30��

	\b �޸ļ�¼
		- 1.0 �涨�� 20080630

	<b>Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSFrameworkInterface
*/
/*********************************************************/

#ifndef _INTERFACE_EDITOR_20080630
#define _INTERFACE_EDITOR_20080630


#define EVT_Editor_Close 1 // �ر��¼�
#define EVT_Editor_DisFocus 2 // ʧȥ���㣨����Notebook�ĵ�ǰ��ʾ��Tab��
#define EVT_Editor_Focus 3 // ��ý��㣨ΪNoteBook��ǰѡ��Tab��


/*! \class IEditor IEditor.h
	\brief �༭���ӿ��࣬���б༭�������ɴ˽ӿ�������
	��������Ҫ�����༭����������ı༭�������ɱ��ӿ�������

	\ingroup GRSFrameworkInterface
*/
class IEditor
{
public:
	PL_PID pluginID;  //!< �༭�����������ID
	PL_IID guid;      //!< �༭����ID
	char szName[100]; //!< �༭��������
	PL_HWND hWnd;  //!< �༭����ָ��
	bool bChange;  //!< �༭�������Ƿ�ı䡣true,�ı�;false,û�иı�
public:
	IEditor() {
		bChange = false;
		pluginID = guid_NULL;
		guid = guid_NULL;
		memset( szName, 0, sizeof(szName));
		hWnd = NULL;
	}

	/*! ����ر���Ϣ
		\param bDestroy �Ƿ�ע������ǵ��õĴ˺���
	*/
	virtual void OnClose( bool bDestroy = false ) = 0;
	virtual void OnSave() = 0;

	// Change�¼����������統ǰѡ���Tab�����˱仯��
	virtual void OnEditorManagerEvent( int iEvtType ) = 0;  // 
};



#endif

