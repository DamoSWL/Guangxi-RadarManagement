
/*********************************************************/
/*! \file IEditorManager.h
	\brief �༭��������

	\version 1.0
	\author �涨��
	\date 2008��05��28��

	\b �޸ļ�¼
		- 1.0 �涨�� 20080528

	\b ��Ȩ��Ϣ��
	<b>Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>
	
	\ingroup GRSFrameworkInterface
*/
/*********************************************************/

#ifndef _HEADER_EDITORMANAGER_20080528
#define _HEADER_EDITORMANAGER_20080528

#include "IEditor.h"

/*! \class IEditorManager IEditorManager.h
	\brief �༭��������

	\ingroup GRSFrameworkInterface
*/
class IEditorManager
{
public:
	//! ע��༭��
	/*! �� ���������ע��༭��
		\param pEditor �༭���ӿ�ָ��
		\param bShow ��ʾ�����ر༭��
	*/
	virtual bool RegEditor( IEditor *pEditor, bool bShow) = 0;
	
	//! ע��ָ���ı༭��
	/*! �� ���������ע���༭��
		\param editorID �༭����ID
	*/
	virtual bool UnregEditor( PL_IID editorID ) = 0;
	
	//! ע��ָ������ı༭��
	/*!
		\param pluginID �����ID
	*/
	virtual bool UnregPluginEditor( PL_PID pluginID ) = 0;
	
	//! ��ȡEditorManager�Ĵ��ڱ�ʶ��
	virtual PL_HWND GetManagerHwnd() = 0;

	//! ��ȡָ���ı༭���Ľӿ�ָ��
	/*!
		\param editorID �༭����ID
	*/
	virtual IEditor *GetEditor( PL_IID editorID ) = 0;

	/*! ��ʾ/���ر༭��
		\param editorID �༭��ID
		\param bShow true,��ʾ�༭����false,���ر༭��
	*/
	virtual void ShowEditor( PL_IID editorID, bool bShow ) = 0;

	//! ָ���ı༭���Ƿ���ʾ
	/*!
		\param editorID �༭����ID
	*/
	virtual bool IsEditorShow( PL_IID editorID ) = 0;

	//! �õ�ָ����������ı༭���ĸ���
	/*!
		\param pluginID �����ID
	*/
	virtual unsigned int GetPluginEditorsCount( PL_PID pluginID ) = 0;

	//! ��ȡ�༭���ĸ���
	virtual unsigned int GetEditorsCount() = 0;

	//! �ر�ָ���ı༭��
	/*!
		\param editorID �༭����ID
		\param dontsave true:�����ñ༭���ı��溯����false:���ñ༭���ı��溯��
	*/
	virtual bool Close( PL_IID editorID,bool dontsave = false) = 0;

	//! ����ָ���ı༭��
	/*!
		\param editorID �༭����ID
	*/
	virtual bool Save( PL_IID editorID) = 0;
};

#endif

