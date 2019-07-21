/*! \file wxNetChoice.cpp
	\brief 网卡选择类

	\version 1.0
	\author 舒定波
	\date 2010-1-15 15:47
	\b 修改记录：
	\arg \c 2010/01/15, 舒定波 创建

	\bug 
	
	Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
All rights reserved.

	\ingroup YWControl

*/

#ifndef HEADER_NETCHOICE_2010_7_7
#define HEADER_NETCHOICE_2010_7_7

#pragma comment(lib , "Iphlpapi.lib")
 
#include <wx/dc.h>
#include <wx/string.h>
#include <IPHlpApi.h>
#include <vector>
using namespace std;

#define NETCHOICE_STYLE_IP 0	// 显示IP
#define NETCHOICE_STYLE_MAC 1	// 显示 MAC 地址
#define NETCHOICE_STYLE_DES 2	// 显示 描述信息

namespace CDYW
{

class wxNetChoice:public wxChoice
{
	typedef struct _NET_INFO {
		wxString	macaddress; //!< 网卡的物理地址
		wxString	description; //!< 网卡的描述
		wxString	subnet; //!< 子网掩码
		wxString	IpAddress; //!< IP地址
		wxString	gateway;  //!< 网关
		wxString	PrimaryWinsServer;  //!< 首要域名服务器(DNS)
		wxString	dhcp; //!< dhcp 服务器
		bool bNet;	//!< 内网还是外网 false 内网, true 外网
		bool bOK;	//!< 是否连接
	}tagNetInfo;

	vector< tagNetInfo > m_NetInfos;

	long m_lStyle;
	
	
	BOOL EnumerateFunc()
	{
		PIP_ADAPTER_INFO pinfo = NULL;
		unsigned long len = 0;

		/// Get the network config;
		unsigned  long nError;
		nError	=	GetAdaptersInfo(pinfo,&len);
		if (nError==0)
		{
			ParseData( pinfo );
		}
		if (nError==ERROR_NO_DATA)
		{
			wxMessageBox( wxT("本地机器不存在适配器信息!"));
		}
		if (nError==ERROR_NOT_SUPPORTED)
		{
			wxMessageBox( wxT("操作系统不支持的适配器"));
		}
		if (nError==ERROR_BUFFER_OVERFLOW)
		{
			char *pChar = (char*)malloc(len);
			pinfo = (PIP_ADAPTER_INFO)pChar;
			nError = GetAdaptersInfo(pinfo,&len);
			if (nError==0)
			{
				ParseData( pinfo );
			}
			free(pChar);
			pinfo = NULL;
		}


		return TRUE;
	};
	
	void ParseData(  PIP_ADAPTER_INFO pinfo  )
	{
		bool b127ip=false,b127First=true;
		while( pinfo!=NULL)
		{
			tagNetInfo netInfo;

			wxString m_macaddress = wxString::Format(
				wxT("%02X:%02X:%02X:%02X:%02X:%02X"),
				pinfo->Address[0],pinfo->Address[1],pinfo->Address[2],
				pinfo->Address[3],pinfo->Address[4],pinfo->Address[5]
			);
			wxString m_description( pinfo->Description, wxConvLocal );

			PIP_ADDR_STRING pAddressList = &(pinfo->IpAddressList);
			wxString m_IpAddress;
			wxString szInvalIP= wxT("0.0.0.0");
			do 
			{
				wxString wszIP( pAddressList->IpAddress.String, wxConvLocal );
				if( wszIP.CmpNoCase( szInvalIP ) == 0 )
				{
					m_IpAddress = wxT("127.0.0.1");
					b127ip = true;
				}
				else
				{
					m_IpAddress	= wszIP  ;
					b127ip = false;
				}
	
				netInfo.description = m_description;
				netInfo.IpAddress = m_IpAddress;
				netInfo.macaddress = m_macaddress;
				if(b127ip==false ||(b127ip==true&&b127First==true))
					m_NetInfos.push_back(netInfo);

				if(b127ip)
					b127First = false;

				pAddressList = pAddressList->Next;	
			} while (pAddressList != NULL);

			pinfo = pinfo->Next;			
		}

		if(b127First)
		{
			tagNetInfo netInfo;
			netInfo.description = wxT("");
			netInfo.IpAddress = wxT("127.0.0.1");
			netInfo.macaddress = wxT("");
			m_NetInfos.push_back(netInfo);
		}		
	}

public:


	~wxNetChoice(void) {};

	wxNetChoice(wxWindow *parent,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		const wxArrayString &choices,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxChoiceNameStr)
		:wxChoice( parent, id, pos, size, choices, style, wxDefaultValidator, name )
	{
		EnumerateFunc();

		size_t sizeNets = m_NetInfos.size(), i = 0;
		for( i=0; i<sizeNets; i++ )
		{
			AppendString( m_NetInfos[i].IpAddress );
		}

		m_lStyle = 0;
	};


	bool GetValue( long lStyle, wxString &szVal )
	{
		int iSel = GetSelection();

		if( iSel >= 0 )
		{
			switch ( lStyle )
			{
			case NETCHOICE_STYLE_IP:
				{
					szVal = m_NetInfos[ iSel ].IpAddress;
					return true;
				}
				break;
			case NETCHOICE_STYLE_MAC:
				{
					szVal = m_NetInfos[ iSel ].macaddress;
					return true;
				}
				break;
			case NETCHOICE_STYLE_DES:
				{
					szVal = m_NetInfos[ iSel ].description;
					return true;
				}
				break;
			}
		}
		
		szVal = wxT("");
		return false;
	}

	bool SetValue( long lStyle, wxString szVal )
	{
		size_t sizeNets = m_NetInfos.size(), i = 0;

		bool bFind = false;

		for( i=0; i<sizeNets; i++ )
		{
			switch ( lStyle )
			{
			case NETCHOICE_STYLE_IP:
				{
					if( m_NetInfos[i].IpAddress == szVal )
					{
						SetSelection( i );
						bFind = true;
					}
				}
				break;
			case NETCHOICE_STYLE_MAC:
				{
					if( m_NetInfos[i].macaddress == szVal )
					{
						SetSelection( i );
						bFind = true;
					}
				}
				break;
			case NETCHOICE_STYLE_DES:
				{
					if( m_NetInfos[i].description == szVal )
					{
						SetSelection( i );
						bFind = true;
					}
				}
				break;

			}

			if( bFind ) break;

		}

		return bFind;
	
	}

	void SetStyle( long lStyle )
	{
		m_lStyle = lStyle;

		Clear();

		size_t sizeNets = m_NetInfos.size(), i = 0;
		for( i=0; i<sizeNets; i++ )
		{

			AppendString( m_NetInfos[i].IpAddress );

			switch ( lStyle )
			{
			case NETCHOICE_STYLE_IP:
				AppendString( m_NetInfos[i].IpAddress );
				break;
			case NETCHOICE_STYLE_MAC:
				AppendString( m_NetInfos[i].macaddress );
				break;
			case NETCHOICE_STYLE_DES:
				AppendString( m_NetInfos[i].description );
				break;
			}
		}
	
	}

};

}; // end namespace CDYW


#endif


