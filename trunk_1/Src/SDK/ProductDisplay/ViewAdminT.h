//本文件定义了产品视图管理层模板类，用于简化产品产品视图管理层的创建。

#include "../../interface/ProductDisplay/IProductLayer.h"
#include "../../Plugins/ProductDisplay/productdisplay_gui.h"
#include "../../Plugins/ProductDisplay/VAPanel.h"
#include <CDYW/wxDlgPack.hpp>

#ifndef ViewAdminT_h__20081006
#define ViewAdminT_h__20081006

using namespace CDYW::PluginSystem;

//产品显示分层层信息

class CLayerInfo
{
public:
	string m_strAddinName;		//插件的名称
	string m_strAddinExName;	//插件的外部名称
	string m_strAddinPath;		//插件的路径名
	string m_strResource;		//菜单工具栏字符串，XML格式描述符
	bool m_bLoadAddin;			//插件是否正常装载

	HINSTANCE m_hDLLInstance;	//插件 DLL 的实例句柄
	IProductView* m_pHost;		//宿主指针( 插件指针 )
	std::vector< IPluginCommandInfo > m_vecTB; // 插件的工具栏命令结构
	IPluginCommandInfo m_Menu;  // 插件的菜单结构

	unsigned int m_iProductIndex;  // 产品序号

	bool m_bVisiable;			//是否显示插件

	CLayerInfo()
	{
		m_pHost=NULL;
		m_strAddinName="";
		m_strResource = "";
		m_bLoadAddin=false;
		m_iProductIndex = -1;
		m_bVisiable = true;
	};

	CLayerInfo( CLayerInfo const& AddinInfo )
	{
		m_pHost = AddinInfo.m_pHost;
		m_strAddinName = AddinInfo.m_strAddinName;
		m_strAddinExName = AddinInfo.m_strAddinExName;
		m_strAddinPath = AddinInfo.m_strAddinPath;
		m_strResource = AddinInfo.m_strResource;
		m_hDLLInstance = AddinInfo.m_hDLLInstance;
		m_bLoadAddin = AddinInfo.m_bLoadAddin;

		m_vecTB.resize(  AddinInfo.m_vecTB.size() );
		size_t i = 0;
		for(i=0;i<m_vecTB.size();i++)
		{
			m_vecTB[i] = AddinInfo.m_vecTB[i];
		}

		m_Menu = AddinInfo.m_Menu;
		m_iProductIndex = AddinInfo.m_iProductIndex;
		m_bVisiable = AddinInfo.m_bVisiable;
	};

	void operator =( CLayerInfo const& AddinInfo)
	{
		m_pHost=AddinInfo.m_pHost ;
		m_strAddinName=AddinInfo.m_strAddinName;
		m_strAddinExName = AddinInfo.m_strAddinExName;
		m_strAddinPath = AddinInfo.m_strAddinPath;
		m_strResource = AddinInfo.m_strResource;
		m_hDLLInstance = AddinInfo.m_hDLLInstance;
		m_bLoadAddin=AddinInfo.m_bLoadAddin;

		m_vecTB.resize(  AddinInfo.m_vecTB.size() );
		size_t i = 0;
		for(i=0; i<m_vecTB.size(); i++)
		{
			m_vecTB[i] = AddinInfo.m_vecTB[i];
		}

		m_Menu = AddinInfo.m_Menu;
		m_iProductIndex = AddinInfo.m_iProductIndex;
		m_bVisiable = AddinInfo.m_bVisiable;
	};
};

//产品视图管理层辅助模板类,此模板类简化了视图管理层的创建。
template< class T >
class ViewAdminT:
	public IViewAdmin,
	public VAPanel< T >
{
	tagYWRect _rect;			//视图大小
	tagYWPoint _leftTop;		//视图左上角的位置

	tagSynInfo _syn;			//同步源信息
	bool _bSynReady;			//同步参数是否准备好了
	bool _bSynLocked;			//同步状态（锁定/解锁）

	bool _bDataInLocked;		//是否锁定接收数据输入功能
	bool _bScaleLocked;			//是否启用了缩放系数锁定功能

	string m_szHoverInfo;

	int m_iSpaceWid;			//空格宽度

	string m_szPersistence;
protected:
	//绘图和双缓冲 lava_sdb  060303
	bool		m_bInitDC;
	HDC			m_memDC ;
	HBITMAP		m_memBMP ;	
	COLORREF	m_clrBK;		// 背景颜色
	SIZE		m_size;
	bool		m_bHasDraw;		// 是否已经绘制过产品了
	//
	bool m_bLButDown;			// 鼠标左键是否按下
	tagYWPoint m_ptLeftDown;
protected:
	IViewFrame *	m_pFrm;
	IFrameAdmin*	m_pFA;
	int m_iX;			// 绘制区域在框架中的X
	int m_iY;			// 绘制区域在框架中的Y
	int m_iWidth;		// 视图宽度 
	int m_iHeight;		// 视图高度

	bool m_bOverlay;	//是否开启重叠模式

	vector< tagPVConfig > m_pvCfg;			//分层信息

	int m_iParkIndex;						// 作为叠加底产品的序号
	int m_iHostIndex;						// 显示主产品(host)的在产品列表中的序号
	int m_iLockIndex;						// 锁住UI操作的产品在列表中的序号

	bool m_bLoadAllAddins;					// 是否装载了所有插件
	std::vector< CLayerInfo > m_vecLayer;	// 分层容器，用于存放 VA 的各个分层的信息
	
	ViewAdminT( IWallPanel *pPanel, IViewFrame *pFrm ,vector<tagPVConfig> pvCfg, int x=0, int y=0, int cx=100, int cy=200):
		m_pFrm( NULL )
		,VAPanel< T >( pPanel, wxPoint(x,y), wxSize(cx, cy) )
	{
		m_iHostIndex = 0;
		m_iParkIndex = -1;

		m_pFrm = pFrm;

		m_iX = x;
		m_iY = y;
		m_iWidth = cx;
		m_iHeight = cy;
		_rect.left = m_iX;
		_rect.top = m_iY;
		_rect.right = m_iX + m_iWidth;
		_rect.bottom = m_iY + m_iHeight;

		m_pvCfg =  pvCfg;

		// 同步信息初始化
		memset( &_syn, 0, sizeof(tagSynInfo) );
		_bSynLocked		= true;
		_bSynReady		= false;
		_bDataInLocked	= false;
		_bScaleLocked	= false;
		m_bEnableSyn	= true;
		m_bEnableDataIn = true;

		m_iSpaceWid = 3;

		m_pFA = m_pFrm->GetFrameAdmin();
		m_bLButDown = false;

		m_memDC = NULL;
		m_memBMP = NULL;

		m_bInitDC = false;
		m_bHasDraw = false;
	}
	
	~ViewAdminT( )
	{
		size_t cnt = m_vecLayer.size();
		size_t i = 0;
		for( i=0; i< cnt; i++ )					// 移除加载的显示模块
		{
			if( m_vecLayer[i].m_pHost )
			{
				UnLoadProduct( m_vecLayer[i] );
			}
		}

		if( m_memDC ) 
		{
			::DeleteDC( m_memDC );
		}

		if( m_memBMP )
		{
			::DeleteObject( m_memBMP );
		}
	}
	void *QueryInterfaceP( PL_PID pid, PL_IID iid  )
	{
		return (m_pFrm)?m_pFrm->QueryInterfaceP( pid, iid ) : NULL;
	};
	tagYWRect GetViewRect()
	{
		return _rect;
	};
public:
	inline IViewFrame *GetViewFrame()
	{
		return m_pFrm;
	}
protected:
	tagYWRect SetViewRect( tagYWRect rect )
	{
		tagYWRect rectTmp;
		rectTmp		= _rect;
		_rect.left	= rect.left;
		_rect.top	= rect.top;
		_rect.right = rect.right;
		_rect.bottom = rect.bottom;

		m_iX = _rect.left;
		m_iY = _rect.top;
		m_iWidth = _rect.right - _rect.left;
		m_iHeight = _rect.bottom - _rect.top;
	
		return rectTmp;
	}
	int GetSpace()					//获取视图边距
	{
		return m_iSpaceWid;
	}
	void SetSpace( int iSpace )		//设置空格边距
	{
		m_iSpaceWid = iSpace;
	}
	tagYWPoint GetTopLeft()
	{
		_leftTop.x = _rect.left;
		_leftTop.y = _rect.top;
		return _leftTop;
	};
	inline int GetWidth()			//获取产品视图管理层的宽度，像素
	{ 
		return _rect.right - _rect.left;
	};
	inline int GetHeight()			//获取产品视图管理层的高度，像素
	{
		return _rect.bottom - _rect.top;
	};
	inline int GetViewSize()
	{
		return (m_iWidth>m_iHeight)?GetHeight():GetWidth();
	};
	void *GetCanvas()
	{
		return (wxWindow*)this;
	};
	void *SetWall( void *pWall )
	{
		IWallPanel *pW = (IWallPanel*)pWall;
		m_pFrm = pW->GetViewFrame();
		return VAPanel<T>::SetWall( pWall );
	}
	void SetDataReceiveStatus( bool bReceive )
	{

	};
	bool GetDataReceiveStatus()
	{
		return false;
	};
	size_t GetLayerCount()
	{
		return m_vecLayer.size();
	}
	int GetHostLayerIdx()
	{
		return m_iHostIndex;
	};
	int GetParkLayerIdx()
	{
		if( m_iParkIndex == -1 )
		{
			m_iParkIndex = m_iHostIndex;
		}
		return m_iParkIndex;
	}
	IProductView *GetLayer( unsigned int iInd )
	{
		unsigned int uiSize = (unsigned int)m_vecLayer.size();
		if( iInd >= uiSize )
		{
			return NULL;
		}
		return m_vecLayer[iInd].m_pHost;
	}
	void RegProductInfo( unsigned long lSessionID, const char* cXML, unsigned long iXmlLen )
	{
		CLayerInfo &AddinInfo = GetAddinInfo(lSessionID);
		AddinInfo.m_strResource = cXML;
	}
	CLayerInfo &GetAddinInfo(unsigned long iAddinIndex)		// 获取产品信息
	{
		return m_vecLayer[ iAddinIndex ];
	}
	// 同步处理函数
	void SetSynInfo( tagSynInfo syn ,bool bLock=false )
	{
		_syn = syn;
		_bSynReady = true;
		LockSyn( bLock );
	}
	tagSynInfo GetSynInfo()
	{
		return _syn;
	}
	void LockSyn( bool bLock )				//锁定/解锁同步功能
	{
		_bSynLocked = bLock;
	}
	bool IsSynLocked( )						// 视图管理层同步功能是否处于锁定状态
	{
		return _bSynLocked;
	}
	void LockDataIn( bool bLock )			// 锁定/解锁数据传入功能
	{
		_bDataInLocked = bLock;
	}
	bool IsDataInLocked()					// 是否处于数据装载锁定状态
	{
		return _bDataInLocked;
	}
	void LockScale( bool bLock )
	{
		_bScaleLocked = bLock;
	}
	//视图管理层是否启动了锁定缩放系数功能
	//启用的此功能后，视图管理在打开新的数据或接收到同步操作时忽略新的缩放系数
	bool IsScaleLocked()
	{
		return _bScaleLocked;
	}
	// 产品数据传递
	bool PutData( const char *szMCode, IN unsigned char*pBuffer, IN int lLen, int iType  )
	{
		// 发送数据到各个模块
		size_t cnt = m_vecLayer.size();
		size_t i = 0;
		bool bKeepScale = IsScaleLocked();
		
		for( i=0; i< cnt; i++ )
		{
			IProductView *pPV = m_vecLayer[i].m_pHost;

			if( m_vecLayer[i].m_bLoadAddin && pPV )
			{
				int pd = pPV->GetProductNumber();
				if( pPV->GetProductNumber() == iType )
				{
					try
					{
						return pPV->PutData(  pBuffer, lLen,iType );
					}
					catch( int ex )
					{
						if( ex == 1 )
						{
							YWTRACE("\n [%d]%s, Product Draw Busy", __LINE__, __FILE__ );
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	bool GetData( const char *szMCode, unsigned char IN OUT **pData, IN OUT int &lLen  )
	{
		return false;
	}
	int GetProductNumber()						//获取产品的索引号
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV ) 
		{
			return pPV->GetProductNumber();
			
		}
		return -1;
	}
	bool SetProductNumber( unsigned int iProNum )
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV ) 
		{
			return pPV->SetProductNumber( iProNum );
		}

		return false;
	}
	/// 通信处理函数
	void FireComm( tagViewComm &comm )
	{
		comm.pView = (IViewAdmin *)this;

		switch( comm.funComm.iType )
		{
		case COMM_TYPE_TB_CMD:
			{
				m_pFrm->FireComm( comm );
			}
			break;
		default:
			{
				size_t cnt = m_vecLayer.size();										// 通知其它各个视图层
				size_t i = 0;
				for( i=0; i< cnt; i++ )
				{
					IProductView *pView = m_vecLayer[i].m_pHost;
					if( m_vecLayer[i].m_bLoadAddin &&
						pView && 
						( comm.pPView != pView )
						)
					{
						pView->OnViewComm( comm );
					}
				}
				if( IsEnableFunction( FUN_SYN) && !IsSynLocked() )					// 视图管理层锁定功能有效且没有锁定就通知框架层
				{
					m_pFrm->FireComm( comm );
				}
			}
			break;
		};
	};
	void OnViewFrameComm( tagViewComm &comm )				//! 通信处理函数, 处理来自产品显示框架的通信信息
	{
		if( comm.funComm.iType == COMM_TYPE_VA_RESIZE  )
		{
			return NotifyResize( comm.funComm.resize.width,comm.funComm.resize.height );
		}
		if ( comm.funComm.iType != COOM_TYPE_TB_MAP_SWITCH )
		{
			if( !IsEnableFunction( FUN_SYN ) || IsSynLocked() || IsScaleLocked() ) 
			{
				return;
			}
		}
		{
			size_t cnt = m_vecLayer.size();								// 通知各个视图层更新
			size_t i = 0;
			for( i=0; i< cnt; i++ )
			{
				IProductView *pView = m_vecLayer[i].m_pHost;
				if( m_vecLayer[i].m_bLoadAddin &&
					pView && pView->HasData() )
				{
					pView->OnViewComm( comm );
				}
			}
		}

	};
	bool GetComm( tagComm &comm )
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() ) 
		{
			return pPV->GetViewComm( comm );
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	/// UI 操作
public:

	void OnMouseDown( int x, int y )
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() )
		{
			//绘制产品过程中不响应任何鼠标事件
			if(pPV->isDrawingProduct()) return;

			pPV->OnMouseDown( x, y, 0, 0 );
		}
	}
	void OnMouseUp( int x, int y )
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() ) 
		{
			//绘制产品过程中不响应任何鼠标事件
			if(pPV->isDrawingProduct()) return;

			pPV->OnMouseUp( x, y, 0, 0 );
		}
	}
	void OnMouseMove( int x, int y )
	{
		//绘制产品过程中不响应任何鼠标事件
		{
			int iBaseI = GetParkLayerIdx();
			IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
			if( pPV && pPV->HasData()&& pPV->isDrawingProduct()) 
				return;
		}	

		if( m_bLButDown && (m_memDC != NULL) )
		{
			HWND hWnd = (HWND)GetHandle();

			HDC hdc = ::GetDC( hWnd );
			MouseMoveImage( hdc, m_memDC, x, y, m_ptLeftDown.x, m_ptLeftDown.y);
			::ReleaseDC( hWnd, hdc);
		}
		else
		{
			int iBaseI = GetParkLayerIdx();
			IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
			if( pPV && pPV->HasData() ) 
			{
				pPV->OnMouseMove( x, y, 0, 0 );
			}
		}
	}
	bool OnActionStart( int iAction, int x, int y )
	{
		m_bLButDown = true;
		m_ptLeftDown.x = x;
		m_ptLeftDown.y = y;
		
		bool bRet = false;
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() )
		{
			if(pPV->isDrawingProduct()) return bRet;
			bRet = pPV->OnActionStart( iAction, x, y );
		}
		return bRet;
	}
	bool OnActionStop( int iAction, int x, int y )
	{
		bool bRet = false;
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() ) 
		{
			if(pPV->isDrawingProduct()) return bRet;
			bRet = pPV->OnActionStop( iAction, x, y );
		}
		m_bLButDown = false;
		return bRet;
	}
	bool OnActionContinue( int iAction, int x, int y )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	/// 功能扩展类功能函数
	bool OnCommandHandle( unsigned short wNotifyCode, unsigned short wID )		//工具条/菜单 消息处理函数
	{
		return false;
	}
	void OnMenuZoom( unsigned short wID, int x, int y )
	{
		int iBaseI = GetParkLayerIdx();
		IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
		if( pPV && pPV->HasData() )
		{
			pPV->OnMenuZoom( wID, x, y,m_iWidth, m_iHeight, 0, 0 );
		}
	}
protected:	
	void OnToolBarDef( unsigned short wID )
	{

	}
	bool GetMouseHoverPoint( int x, int y, tagYWPoint &pt)
	{
		if( m_vecLayer.size() > 0 )
		{
			int iBaseI = GetParkLayerIdx();
			IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
			if( pPV && pPV->HasData() ) 
			{
				return pPV->GetMouseHoverPoint( x, y, pt );
			}
		} 
		return false;
	}
	bool GetMouseHoverXY( int &x, int &y, tagYWPoint pt)
	{
		if( m_vecLayer.size() > 0 )
		{
			int iBaseI = GetParkLayerIdx();
			IProductView *pPV = m_vecLayer[ iBaseI ].m_pHost;
			if( pPV && pPV->HasData() ) 
			{
				return pPV->GetMouseHoverXY( x, y, pt );
			}
		} 
		return false;
	}
	const char *GetMouseHoverInfo( tagYWPoint &pt, bool bCurVA )		//获取指定点的产品数据信息
	{
		m_szHoverInfo.clear();

		size_t cnt = m_vecLayer.size();							// 发送数据到各个模块
		size_t i = 0;
		string szOut = "";
		for( i=0; i< cnt; i++ )
		{
			IProductView *pPV = m_vecLayer[i].m_pHost;
			if( pPV && pPV->HasData() )
			{
				szOut = pPV->GetMouseHoverInfo( pt, bCurVA );
				if( !szOut.empty() ) 
				{
					m_szHoverInfo += szOut;
				}
			}
		}
		return m_szHoverInfo.c_str();
	}
	void Refresh( bool bRefresh )			//是否刷新绘图,bRefresh true:强制刷新;false: 不用刷新
	{

	};
	IProductView *LoadProduct( string szCfg, unsigned int iHostProductNumber )		//产品模块读取函数
	{
		try
		{
			string szPath		= string(szCfg.begin(), szCfg.begin() + szCfg.rfind('\\') + 1 );
			string szBinPath	= string(szPath.begin(), szPath.end()-1 );
			szBinPath			= string(szBinPath.begin(), szBinPath.begin() + szBinPath.rfind('\\') + 1 );
			string szWorkPath	= string(szBinPath.begin(),szBinPath.end()-1);
			szWorkPath			= string(szWorkPath.begin(),szWorkPath.begin()+szWorkPath.rfind('\\')+1);
			
			ticpp::Document doc( szCfg.c_str() );						// 计算路径

			doc.LoadFile();

			ticpp::Element *root = doc.FirstChildElement("Product");
			if( root != NULL )											// 获取显示模块名称
			{
				string szDispPath;
				ticpp::Element *elTypeInfo = root->FirstChildElement( "TypeInfo" );
				if( elTypeInfo )
				{
					ticpp::Element* elDisPath = elTypeInfo->FirstChildElement( "display" );
					if( elDisPath )
					{
						szDispPath = elDisPath->GetText( );
					}
				}
				CLayerInfo info;
				info.m_strAddinPath = szWorkPath+"\\Products\\";
				info.m_strAddinPath += szDispPath;
				info.m_iProductIndex  = iHostProductNumber;								// 创建产品层信息
				m_vecLayer.push_back( info );

				unsigned long posLayer = (unsigned long)m_vecLayer.size() - 1;			// 读取产品层

				if( ! LoadProduct( m_vecLayer[ posLayer ], posLayer ))
				{
					m_vecLayer[posLayer].m_bLoadAddin = false;
				}
				else
				{
					m_vecLayer[posLayer].m_bLoadAddin = true;
				}

				m_iLockIndex = m_iParkIndex;
				
				return m_vecLayer[ posLayer ].m_pHost;
			}
		} 
		catch( ticpp::Exception& ex ) 
		{
			wxLogDebug(wxT("读取重叠信息异常，忽略掉重叠信息。[%s]\n"),wxString( ex.m_details.c_str(), wxConvLocal ));
			return NULL;
		}
		return NULL;
	}

	//装载产品显示模块
	//param info 插件结构体
	//param index 插件在容器中的序号
	
	bool LoadProduct( CLayerInfo &info, unsigned long iIndex )
	{
		HMODULE hModule = LoadLibraryA( info.m_strAddinPath.c_str() );

		if (!hModule)
		{
			return false;
		}
		else
		{
			info.m_hDLLInstance = hModule;

			bool bFuncCalled = false;
			PRODUCT_INITFUNC pInitFunc = ( PRODUCT_INITFUNC )GetProcAddress( info.m_hDLLInstance, MAKEINTRESOURCEA(1));
			if (pInitFunc != NULL)
			{
				info.m_pHost = (*pInitFunc)(iIndex,(IViewAdmin*)this,true,info.m_iProductIndex );
				if( info.m_pHost != NULL )
				{
					info.m_strAddinName = "";
					info.m_bLoadAddin	= true;
					bFuncCalled			= true;
					info.m_pHost->SetProductNumber( info.m_iProductIndex );
				}
			} 
			else 				// Remove Invalid/Corrupt DLL Instance from memory
			{
				UnLoadProduct( info );
			}
			return bFuncCalled;					// Return Success or failure
		}
	}

	bool UnLoadProduct( CLayerInfo &info )
	{
		bool bFuncCalled = false;
		if (info.m_hDLLInstance != NULL)
		{
			if( info.m_pHost != NULL )
			{
				PRODUCT_DESTROYFUNC pDestroyFunc = (PRODUCT_DESTROYFUNC)GetProcAddress(info.m_hDLLInstance , MAKEINTRESOURCEA(2));
				if (pDestroyFunc != NULL)
				{
					(*pDestroyFunc)( info.m_pHost);

					bFuncCalled = true;
				}
				else
				{
					bFuncCalled = false;
				}
			}
			FreeLibrary(info.m_hDLLInstance );			// Release library
		}
		return bFuncCalled;
	}

	bool m_bEnableSyn;
	bool m_bEnableDataIn;

	void EnableFunction( int iFun, bool bEnable )				//打开/关闭对某功能的支持
	{
		switch( iFun )
		{
		case FUN_SYN:
			m_bEnableSyn = bEnable;
			break;
		case FUN_DATAIN:
			m_bEnableDataIn = bEnable;
			break;
		}
	}
	bool IsEnableFunction( int iFun )							//确定是否支持某项功能
	{
		switch( iFun )
		{
		case FUN_SYN:
			return m_bEnableSyn;
		case FUN_DATAIN:
			return m_bEnableDataIn;
		}
		return false;
	}
	void MouseMoveImage( HDC &hdc, HDC &hdcLayer, int x, int y, int downX, int downY )
	{
		T *pT = (T*)this;

		int OffDx = x - downX; // 相对于鼠标按下移动的点的偏移
		int OffDy = y - downY;

		
		tagYWRect rect = GetViewRect();
		int vSize = GetViewSize();

		wxRect rectLast( 0, 0, vSize, vSize );  // 上一个矩形
		wxRect rectIntersect = rectLast;
		wxRect rectCur( OffDx, OffDy, vSize, vSize);
		rectIntersect.Intersect( rectCur );  // 求两个矩形的交集

		int paintWid = rectIntersect.GetWidth();
		int paintHei = rectIntersect.GetHeight();
		wxSize paintSize( paintWid, paintHei );
		wxPoint paintPT;  // 源dc剪切区域左上角顶点
		wxPoint ptLT;	  // 目标区域左上角顶点

		wxRect rectRePaint0, rectRePaint1;  // 绘制黑色矩形区域的矩形

		if( rectIntersect.Contains( rectLast.GetLeftTop() ) )		// 左上角
		{
			ptLT.x = rectIntersect.GetLeft();
			ptLT.y = rectIntersect.GetTop();

			paintPT.x = vSize - paintWid;
			paintPT.y = vSize - paintHei;

			rectRePaint0 = wxRect( 0, paintHei, paintWid, vSize- paintHei );
			rectRePaint1 = wxRect( paintWid, 0, vSize - paintWid, vSize );

		}
		else if( rectIntersect.Contains( rectLast.GetRightTop()) )  // 右上角
		{
			ptLT.x = rectIntersect.GetRight() - paintWid;
			ptLT.y = rectIntersect.GetTop();

			paintPT.x = 0;
			paintPT.y = vSize - paintHei;

			rectRePaint0 = wxRect( 0, 0, vSize - paintWid, paintHei );
			rectRePaint1 = wxRect( 0, paintHei, vSize, vSize- paintHei );
		}
		else if( rectIntersect.Contains(rectLast.GetLeftBottom()) )  // 左下角
		{
			ptLT.x = rectIntersect.GetLeft();
			ptLT.y = rectIntersect.GetBottom() - paintHei;

			paintPT.x = vSize - paintWid;
			paintPT.y = 0;

			rectRePaint0 = wxRect( 0, 0, vSize, vSize- paintHei );
			rectRePaint1 = wxRect( paintWid, rect.bottom - paintHei, 
				vSize-paintWid, paintHei );
		}
		else														// 右下角
		{
			ptLT.x = rectIntersect.GetRight() - paintWid;
			ptLT.y = rectIntersect.GetBottom() - paintHei;

			paintPT.x = 0;
			paintPT.y = 0;

			rectRePaint0 = wxRect( 0, 0, vSize - paintWid, vSize );
			rectRePaint1 = wxRect( vSize - paintWid,0, paintWid, vSize- paintHei );
		}

		if( !rectIntersect.IsEmpty() )
		{
			RECT rc = {rectRePaint0.GetLeft(), rectRePaint0.GetTop(), rectRePaint0.GetRight(),rectRePaint0.GetBottom()};

			FillRect( hdc,&rc,(HBRUSH)m_pFA->GetBKBrush());

			RECT rc1 = {rectRePaint1.GetLeft(), rectRePaint1.GetTop(), rectRePaint1.GetRight(),rectRePaint1.GetBottom()};
			FillRect( hdc,&rc1,(HBRUSH)m_pFA->GetBKBrush());

			::BitBlt( hdc,ptLT.x, ptLT.y,paintWid, paintHei,hdcLayer,paintPT.x, paintPT.y, SRCCOPY);
		}
	}
	unsigned long _styleInfo;
	unsigned long _styleColor;

	int _infoLeft,_infoTop, _infoWid, _infoHei;
	int _clrLeft,_clrTop, _clrWid, _clrHei;	

public:
	bool GetInfoRect( unsigned long &style , int &left, int &top, int &wid, int &hei )
	{
		style = _styleInfo;

		left = _infoLeft;
		top = _infoTop;
		wid = _infoWid;
		hei = _infoHei;

		return true;
	}

	bool SetInfoRect( unsigned long style , int left, int top, int wid, int hei )
	{
		// 设置信息区域相关信息
		_styleInfo = style;
		_infoTop = top;
		_infoLeft = left;
		_infoWid = wid;
		_infoHei = hei;
		return true;
	}

	// 获取色域绘制区域的大小和绘制的样式
	bool GetColorFieldRect( unsigned long &style , int &left, int &top, int &wid, int &hei )				// 获取颜色绘制区域相关信息
	{
		style = _styleColor;

		left = _clrLeft;
		top = _clrTop;
		wid = _clrWid;
		hei = _clrHei;

		return true;
	}
	bool SetColorFieldRect( unsigned long style , int x, int y, int wid, int hei )						// 设置色标绘制区域相关信息
	{

		_styleColor = style;
		_clrLeft = x;
		_clrTop = y;
		_clrWid = wid;
		_clrHei = hei;
		return true;
	}
	const char *Persistence()
	{
		char szHead[MAX_PATH] = {0};
		char szInfo[MAX_PATH] = {0};
		char szColor[MAX_PATH] = {0};
		char szLayers[MAX_PATH] = {0};
		string szFoot( "\n\t\t\t</Layers>\n\t\t</ViewAdmin>");

		wxPoint pt = GetPosition();
		wxSize size = GetSize();
		sprintf( szHead,
			"\n\t\t<ViewAdmin x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">",
			pt.x,pt.y,
			size.GetWidth(),size.GetHeight()
			);
		
		sprintf( szInfo, "\n\t\t<Info x=\"0\" y=\"0\" width=\"0.2\" height=\"0.6\" style=\"fe\" />" );
		sprintf( szColor, "\n\t\t<ColorField x=\"0\" y=\"0.6\" width=\"0.2\" height=\"0.4\" style=\"1\" />" );
		sprintf( szLayers,
			"\n\t\t\t<Layers host=\"%d\" park=\"%d\">",
			GetHostLayerIdx(),
			GetParkLayerIdx()
		);

		string szLayer;
		int cnt = GetLayerCount();
		int i = 0;
		for( i=0; i<cnt; i++ )
		{
			IProductView *pPV = GetLayer( i );

			szLayer += pPV->Persistence( i );
		}
		m_szPersistence = szHead;
		m_szPersistence +=  szInfo;
		m_szPersistence +=  szColor;
		m_szPersistence +=  szLayers;
		m_szPersistence +=  szLayer;
		m_szPersistence +=  szFoot;
		return m_szPersistence.c_str();
	}

	void NotifyResize( int wid, int hei )				// 重设置 VA窗口
	{
		wxRect oldRect( m_iX, m_iY, m_iWidth, m_iHeight );

		wxWindow *pVAWin = (wxWindow *)this;
		pVAWin->SetSize( wid, hei);

		tagYWRect rc;

		wxPoint pt = GetPosition();
		wxRect rect = GetRect();
		rc.left = pt.x; rc.top = pt.y;
		rc.right = rc.left + rect.GetWidth();
		rc.bottom = rc.top + rect.GetHeight();
		SetViewRect(  rc );

		if( m_memBMP != NULL )							// 重计算位图
		{
			::DeleteObject( m_memBMP );
		}

		HWND hWnd = (HWND)GetHandle();
		HDC hdc = ::GetDC( hWnd );

		m_memBMP = ::CreateCompatibleBitmap( hdc , wid, wid );
		::SelectObject ( m_memDC, m_memBMP );
		::ReleaseDC( hWnd, hdc );

		/// 设置VA大小
		m_iWidth = wid;
		m_iHeight = hei;
		_rect.right = _rect.left + m_iWidth;
		_rect.bottom = _rect.top + m_iHeight;

		//// 通知各个层重定义大小

		tagViewComm comm;
		comm.pFrm = GetViewFrame();
		comm.pView = (IViewAdmin*)this;
		comm.funComm.iType = COMM_TYPE_VA_RESIZE;

		// 通知各个视图层更新
		size_t cnt = m_vecLayer.size();
		size_t i = 0;
		for( i=0; i< cnt; i++ )
		{
			IProductView *pView = m_vecLayer[i].m_pHost;
			if( m_vecLayer[i].m_bLoadAddin && pView )
			{
				pView->OnViewComm( comm );
			}
		}

		wxWindow *pPar = GetParent();

		wxRect rcUn = rect.Union( oldRect );
		rcUn.Inflate( 30 );
		pPar->RefreshRect( rcUn, false );				// 通知 框架 刷新
	}
	const char *GetFileName()
	{
		return m_vecLayer[0].m_pHost->GetName();
	}

};


#endif // ViewAdminT_h__

