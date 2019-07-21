#ifndef __HEADER_MultiViewFrameT_2008819
#define __HEADER_MultiViewFrameT_2008819

#include "../../interface/ProductDisplay/IProductLayer.h"
#include <boost/foreach.hpp>
#include <cdyw/include.h>

// 产品显示区域组成
#define PART_DISPLAY	0	//产品绘制区域
#define PART_INFO		1	//产品信息显示区域

//class ViewFrameT
//brief 产品显示框架模板类
//本模版类实现了框架层消息的传递，鼠标UI操作事件的传递。
//param hostT 宿主类，该宿主类必须实现IPaintPanel接口
//param viewAdminT 产品视图管理层分类
//ingroup ProductDisplayGroup

template< class hostT , class viewAdminT >
class ViewFrameT:public IViewFrame
{
	void *m_hFrm;		//产品显示框架标识符,为 void *指针
						//- 在windows下表示窗口句柄;
						//- 在wxWxidgets表示 wxWindow 类型的指针;
	PL_IID	m_id;
	int		_actionID;  // 当前动作ID
protected:
	pthread_mutex_t m_muVA;		//保护 VA的操作同步

	IFrameAdmin *m_pAdmin;		//产品显示框架管理层指针
	unsigned long m_ulStyle;	//产品显示框架的样式

	bool m_bFullScreen;
	int _X0,_Y0;				// 框架的左上角的坐标
	int _CX,_CY;				// 框架的宽度和高度

	int m_iScreenIndex;
	string m_szName;
	string m_szBinPath;			// 产品显示框架模块路径
	string m_szHoverInfo;
private:
	IWallPanel *_pPaintPanel;
	hostT *_pT;

	string m_szPersistence;
protected:
	inline IFrameAdmin *GetFA() { return m_pAdmin; };
public:
	vector< IViewAdmin* > m_vVA; // 框架包含的VA列表

	typedef vector< IViewAdmin* >::iterator ItVA;

	ViewFrameT( IFrameAdmin *pAdmin, const char *pszBinPath = NULL):
	  m_pAdmin(pAdmin),_pT(NULL)
	{
		memset( &m_muVA, 0, sizeof( pthread_mutex_t ) );
		m_muVA = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

		m_ulStyle = 0;						//FRAME_STYLE_REAL | FRAME_STYLE_POP
		m_bFullScreen = false;
		m_iScreenIndex = 0;

		m_szName = "";
		m_szBinPath = "";
		if( pszBinPath )
		{
			m_szBinPath = pszBinPath;
		}

		m_id = guid_NULL;

		SetActionID( ACTION_NULL );

		//////////////////////////////////////////////////////////////////////////

		_pT = ( hostT * )this;
		_pPaintPanel = (IWallPanel *)_pT;

		_pPaintPanel->SetPaintInterface( 
			(IViewFrame *)_pT,
			(wxWindow *)_pT,
			(wxWindow *)_pT->m_panelPaint
			);
	};
	~ViewFrameT( )
	{
		pthread_mutex_destroy( &m_muVA );
		BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
		{
			if( pVA )
			{
				viewAdminT *pAdmin = (viewAdminT *)(pVA);
				delete pAdmin;
				pVA = NULL;
			}
		}
	}
	inline IFrameAdmin *GetFrameAdmin()
	{
		return m_pAdmin;
	}
	void  SetFrameAdmin( IFrameAdmin* pFA )
	{
		m_pAdmin = pFA;
	}
	bool IsSupportProduct( const char *szMCode, bool bReal )			//是否支持指定的产品的显示
	{
		return false;
	}
	inline unsigned long GetStyle()												//获取显示框架样式
	{
		return m_ulStyle;
	};
	inline unsigned long SetStyle( unsigned long ulStyle )						//设置窗口样式
	{
		unsigned long style = m_ulStyle;
		m_ulStyle = ulStyle;
		return style;
	};
	void SetPopProperty( bool bFullScreen, int x, int y, int cx, int cy)		//设置弹出窗口属性（仅在窗口为Pop样式时有效）
	{
		m_bFullScreen = bFullScreen;
		_X0 = x; _Y0 = y;
		_CX = cx; _CY = cy;
	};
	//获取弹出窗口的属性（仅在窗口为Pop样式时有效）
	void GetPopProperty( bool *bFullScreen, int *x = NULL, int *y = NULL, int *cx=NULL, int *cy=NULL )
	{
		if(bFullScreen) *bFullScreen = m_bFullScreen;
		if(x) *x = _X0;
		if(y) *y = _Y0;
		if(cx) *cx = _CX;
		if(cy) *cy = _CY;
	};
	void SetFrameName( const char *szName )	//设置框架名称 
	{
		if( szName )
		{
			m_szName = szName;
		}
	}
	const char* GetFrameName()				//获取框架的名称
	{
		return m_szName.c_str();
	};
	const char *GetBinPath()				//获取框架的可执行文件的路径
	{
		return m_szBinPath.empty()?NULL:m_szBinPath.c_str();
	};
	void SetFrameID( PL_IID id )			//设置框架的 唯一 ID
	{
		m_id = id;
	}
	PL_IID GetFrameID( )					//获取框架的ID
	{
		return m_id;
	}
	// 视图管理层 通信功能
	void SetViewAdminRect( IViewAdmin *pViewAdmin, tagYWRect rect )
	{
		if( pViewAdmin ) 
		{
			pViewAdmin->SetViewRect( rect );
		}
	}
	int AddViewAdmin( IViewAdmin *pAdmin )						//给产品显示框架添加视图管理层
	{
		CDYW::Lock lock( &m_muVA );

		if( m_vVA.size() >= MAX_VIEW_ADMIN_COUNT )
		{
			return -1;
		}
		if( pAdmin ) 											//没有发现
		{
			if( find( m_vVA.begin() ,m_vVA.end(), pAdmin)== m_vVA.end() )
			{
				m_vVA.push_back( pAdmin );
			}
			return m_vVA.size() - 1;
		}

		return -2;
	};
	void RemoveViewAdmin( IViewAdmin *pAdmin )					//从产品显示框架移除指定的视图管理层
	{
		CDYW::Lock lock( &m_muVA );
		if( pAdmin )
		{
			ItVA it = find( m_vVA.begin() ,m_vVA.end(), pAdmin);
			if( it != m_vVA.end() )
			{
				m_vVA.erase( it );
			}
		}
	};
	void DeleteViewAdmin( IViewAdmin *pAdmin )					//从产品显示框架移除视图管理层并删除视图管理层
	{
		CDYW::Lock lock( &m_muVA );
		//  通知 FrameAdmin 改变 焦点 VA
		
		if( NULL != pAdmin )
		{
			/// 通知 ViewAdmin 移除 ProductView 相关信息（包括 PV链表 ）
			int cntLayer = pAdmin->GetLayerCount();
			int i = 0;
			for( i=0; i<cntLayer; i++ )
			{
				IProductView *pPV = pAdmin->GetLayer( i );
				if( pPV != NULL )
				{
					GetPlug()->GetFA().DeleteProductView( pPV );
				}
			}

			/// 从列表里移除 View Admin
			RemoveViewAdmin(  pAdmin );

			viewAdminT *pAdminWin = (viewAdminT *)(pAdmin);
			delete pAdminWin;
			pAdminWin = NULL;
		}
	};

	size_t GetViewAdminCount()
	{
		return m_vVA.size();
	};

	IViewAdmin *GetViewAdmin( unsigned int iInd )
	{
		CDYW::Lock lock( &m_muVA );

		if( iInd >= m_vVA.size() )
		{
			return NULL;
		}
		return m_vVA[ iInd ];
	};
	void EnumViewAdmin( CBFunEnumVA pFun, void *pParam0, void *pParam1 )
	{
		CDYW::Lock lock( &m_muVA );

		BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
		{
			if( pFun( pVA, pParam0, pParam1 ) )
				break;			
		}
	}
	bool FindViewAdmin( IViewAdmin *pVAIn )				//查找指定的VA是否存在
	{
		CDYW::Lock lock( &m_muVA );
		BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
		{
			if( pVA == pVAIn  )
			{
				return true;
			}
		}
		return false;
	}
	void FireComm( tagViewAdminComm &comm )				//触发通信， 此函数由视图管理层调用
	{
		comm.pFrm = (IViewFrame *)this;

		switch( comm.funComm.iType )
		{
		case COMM_TYPE_TB_CMD:
		case COMM_TYPE_SHOW_FRAME_INFO:
			{
				GetFrameAdmin()->FireComm( comm );
			}
			break;
		case COMM_TYPE_VA_REFRESH_COMPLETE:
			{
			}
			break;
		default:
			{
				CDYW::Lock lock( &m_muVA );
				BOOST_FOREACH( IViewAdmin *pVA, m_vVA )						// 通知其它视图管理层同步
				{
					if( pVA &&
						pVA != comm.pView && 
						!pVA->IsSynLocked()
						)
					{
						pVA->OnViewFrameComm( comm );
					}
				}
				m_pAdmin->FireComm( comm );									// 通知其它框架层
			}
			break;
		}
	};
	void OnFrameAdminComm( tagFrameComm &comm )
	{
		switch( comm.funComm.iType )
		{
		case COMM_TYPE_VA_LOST_FOCUS:
		case COMM_TYPE_VA_FOCUS:
			{
				if( comm.funComm.iType == COMM_TYPE_VA_LOST_FOCUS )
				{
					DisableFocus();
				}			
				
				wxWindow *pWin = (wxWindow*)_pT;
				IViewAdmin *pVA = comm.pView;
				if( pVA != NULL )
				{
					tagYWRect rcVA = pVA->GetViewRect();
					wxRect rc( rcVA.left, rcVA.top, rcVA.right - rcVA.left, rcVA.bottom - rcVA.top );
					rc.Inflate( 20,30 );
					pWin->RefreshRect( rc, false );	
				}
				else
				{
					pWin->Refresh( false );
				}
				return;
			}
			break;
		}
		CDYW::Lock lock( &m_muVA );
		
		BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
		{
			if( pVA && pVA->IsSynLocked() )
			{
				pVA->OnViewFrameComm( comm );
			}
		}

	};
	void *QueryInterfaceP( PL_PID pid, PL_IID iid  )
	{
		return (m_pAdmin)?m_pAdmin->QueryInterfaceP( pid, iid ):NULL;
	}
	IViewAdmin *OnFrameLeftDown( int iAction, int x, int y )
	{
		return NULL;
	}
	//鼠标左键弹起,轨迹球版本
	void OnFrameLeftUp( int iAct, int x, int y )
	{

	}
	IViewAdmin *OnAction( int uiAction, int iAction, int x, int y )
	{
		return NULL;
	}
	IViewAdmin *OnActionStart( int iAction, int x, int y )				//负责切换，获取焦点VA
	{
		return NULL;
	}
	IViewAdmin *OnActionContinue( int iAction, int x, int y )
	{
		return NULL;
	}
	IViewAdmin *OnActionStop( int iAction, int x, int y )
	{
		return NULL;
	}
	IViewAdmin * OnFrameRightDown( int iAct, int x, int y, int &iAction )		//鼠标右键按下命令
	{
		 return NULL;
	}
	IViewAdmin *OnFrameMouseMove(int iAct, int x, int y )						//鼠标移动
	{
		return NULL;
	}
	void Refresh( unsigned long refStyle )
	{
		if( refStyle & FRAME_REFRESH_ViewAdmin )							// 如果要求刷新视图
		{
			CDYW::Lock lock( &m_muVA );
			BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
			{
				pVA->Refresh( false );
			}
		}
	}
	void *GetWindow()
	{
		return (wxWindow*)_pT;
	}
	bool IsUIOperStart()
	{
		return m_bUIOperStart;
	}
	void SetUIOper( bool bStart )
	{
		m_bUIOperStart = bStart;
	}
	//////////////////////////////////////////////////////////////////////////
	//计算指定点在极坐标中的距离和方位
	//param x 
	//param y 
	//param coord
	//param fR 距离，单位：公里
	//param fSeta 方位，单位：度
	void GetPosition( IN int x, int y, IN tagRadialPolarCoord *coord, OUT float &fR, OUT float &fSeta)
	{
		float r = sqrt((float)(( x - coord->X)*( x - coord->X) 
			+(y - coord->Y)*( y - coord->Y) )) ;

		fSeta= atan2((float)( x - coord->X),(float)( coord->Y- y)) *180 /M_PI;
		if( fSeta < 0 ) fSeta += 360;
		fR = r * coord->detM /1000.f ;
	}

	void DisableFocus()
	{
		GetFA()->ShowMouseValueHover( NULL, NULL, 0, 0, false );
	}

	void SetFocus ()
	{
		IFrameAdmin *pAd = GetFrameAdmin();
		
		IViewFrame *pFrm = ( IViewFrame * )this;
		
		if( !pAd->IsFocusFrame( pFrm ) )
		{
			pAd->FireFocusFrameChange( pFrm );
		}
	}

	inline int GetActionID()
	{
		return _actionID;
	}

	inline void SetActionID( int newID )
	{
		_actionID = newID;
	}

	const char *Persistence()
	{
		m_szPersistence = "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n";
		
		char szHead[MAX_PATH] = { 0 };
		char szId[MAX_PATH] = { 0 };
		char szPop[MAX_PATH] = { 0 };
		char szLayout[MAX_PATH] = { 0 };
		string szVA;
		string szFoot = "\n\t</Layout>\n</MultiFrame>";
			
		// TODO 添加扩展窗体支持
		sprintf( szHead, "<MultiFrame bin=\"\" name=\"%s\" pop=\"%d\" ext=\"0\">\n",
			GetFrameName(),
			(GetStyle() & FRAME_STYLE_POP)?1:0
			);

		PL_GUID gui = m_id;

		if( IsEqualID( m_id, guid_NULL ) )
		{
			sprintf( szId, "\t<Id></Id>\n" );
		}
		else
		{
			sprintf( szId, "\t<Id>{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}</Id>\n"
				, m_id.Data1
				, m_id.Data2
				, m_id.Data3
				, m_id.Data4[0], m_id.Data4[1]
			, m_id.Data4[2], m_id.Data4[3], m_id.Data4[4], m_id.Data4[5]
			, m_id.Data4[6], m_id.Data4[7]
			);
		}

		/// 获取窗口位置及大小信息
		wxWindow *pWin = (wxWindow*)_pT->GetParent();
		wxPoint pt = pWin->GetPosition();
		wxSize size = pWin->GetSize();

		sprintf( szPop, "\t<Pop fullScreen=\"0\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n\t<Layout>",
				pt.x,pt.y,size.GetWidth(),size.GetHeight()
			);

		////  获取各个 VA 层的配置
		BOOST_FOREACH( IViewAdmin *pVA, m_vVA )
		{
			szVA += pVA->Persistence();
		}

		m_szPersistence += szHead;
		m_szPersistence += szId;
		m_szPersistence += szPop;
		m_szPersistence += szLayout;
		m_szPersistence += szVA;
		m_szPersistence += szFoot;

		return m_szPersistence.c_str();
	}
};


#endif // __HEADER_MultiViewFrameT_2008819

