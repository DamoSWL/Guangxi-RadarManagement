/* \file WRSOCcontrtolMenu.h
	\brief WRSOCcontrtol�˵�����
	\ingroup WRSOCcontrtolGroup
	
	���ļ�������WRSOCcontrtol�˵����ݡ�

	\version 1.0
	\author 
	\date 20170904

	\b �޸ļ�¼��
	\arg \c version 1.0�� <author>��20170904 ����

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup WRSOCcontrtolGroup
*/


#ifndef __HEADER_GuangXi_Menu_20171117
#define __HEADER_GuangXi_Menu_20171117

/*
\b A_Menu ��ʾ���˵���ֻ����һ������parGUID��ʶ�����˵��Ѿ�����ʱ�����룬�������򴴽�
\b B_Menu ��ʾ�Ӳ˵��������ж�����Ӳ˵��»�������� LeafMenu �˵���
\b LeafMenu �˵���

\arg \c name �˵��������
\arg \c id �˵����id����id�ڲ������Ψһ�ģ�������ݴ�id�ж�UI�¼�����Դ
\arg \c tooltip �˵�����ʾ�� tooltip
\arg \c separator �˵��������
\arg \c order �˵������λ��

- -1 �ָ������Ͳ˵���
- 0 ��ͨ�˵���
- 1 check ���Ͳ˵���
- 2 RADIO ���Ͳ˵���

��ݼ�XML������ʽ��(&amp;T)
*/
//! �˵�����
const char g_MenuXML[] ="<?xml version=\"1.0\" encoding=\"gb2312\" ?>"
"<Plugin name=\"wxPlugin\">"

	"<LeafMenu name=\"��������\" id=\"11\" separator=\"0\" order=\"4\" />"
	"<LeafMenu name=\"-\" separator=\"-1\" order=\"3\" />"

"</Plugin>";

static const PL_MENU_ID g_guid_menu = { 0x32A822ED, 0x4C1F, 0x4AC5, { 0x92, 0x70, 0xA0, 0xD0, 0xF8, 0x6B, 0x8B, 0x9D } };

#endif



