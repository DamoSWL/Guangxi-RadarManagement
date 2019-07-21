/*! \file ProductViewT.h
	
	本文件定义了产品视图层模板类，用于简化产品视图层的创建。
*/



#ifndef CProductViewT_h_20081006
#define CProductViewT_h_20081006

#include "../../interface/ProductDisplay/IProductLayer.h"
#include "../../interface/seals/CLayerT.h"

#include <pthread.h>
#include <WRISFramework/Includes.h>
#include "../../include/WRIS/product/includes.h"
#include "../../interface/DataAdmin/includes.h"
#include "../../interface/Productinfo/Productinfo_I.h"
#include "../../interface/scanmodeedit/ScanModeEdit_I.h"
#include "../../interface/GISModule/GISModule_I.h"

using namespace CDYW::GRS::DataFormat::XFL;
using namespace CDYW::RadarSystem;

#define EXCEPTION_DRAW_BUSY 1	// 绘制线程忙

#define INFO_MAX_CNT 20  // 支持的最大信息条目数
#define INFO_BASE_CNT 11 // 基本信息包含的信息条目数


/*! \class ProductViewT

	\brief 产品视图层辅助模板类

	此模板类简化了产品视图层的创建。
	\P  - T 产品类
		- TPro1 产品文件解析类

	\ingroup ProductDisplayGroup
*/
template< class T, class TPro >
class ProductViewT:
	public IProductView,
	public ILayerT< ProductViewT< T, TPro > , TPro >
{
	//! 产品绘制参数结构体
	typedef struct _DrawProductPara
	{
		void *pThis;  //
		T *pT;

		TPro *pPro;
		HDC hdc;
		CDrawRadialT *pDPara;  // 产品绘制参数
		unsigned int iProId;
		bool bKeepScale;
	}tagDrawProductPara;

	///// 
	bool m_bRefresh; // 是否重画
	int m_Ruler;  // 是否显示标尺(距离圈)
	bool m_bDrawInfo; // 是否绘制的了产品信息；为了标识在每次刷新时是否需要重新绘制产品信息
	bool m_bHost;  // 视图层是否属于Host视图层
	bool m_bIsDrawing;  // 绘制线程是否正在绘制产品
	bool m_bIsScaleDrawing;  // 是否缩放线程正在绘制产品
	pthread_t m_thrDrawID;  // 新产品数据绘制线程
	pthread_t m_thrScaleID; // 产品缩放、平移操作绘制线程
	pthread_mutex_t m_mutScale;


	CDrawRadialT *m_pLeftDownDPara; // 鼠标左键按下时的缩放参数
	tagYWPoint m_ptLeftDown; // 鼠标左键按下

	bool m_bPVMCode;		//! 此产品视图是否启用区别码过滤:true,启用；false，不启用。缺省为true
	string m_szPVMCode;		//! 当前视图层的指定区别码
protected:
	string m_szMCode;		// 当前打开产品的区别码
	string m_szMCodeStr;		//! 当前视图层的区别码参数显示形式
	string m_szProDateTime;			//当前产品时间显示
private:
	bool m_bReceData;		// 视图层是否开启了数据接收
	bool m_bReal;			// 视图层是否接收实时类型的数据

	string m_szPersistence; // 持久化属性
protected:
	bool m_bLButDown;  // 鼠标左键是否按下
	char m_szInfo[20][60];    //产品信息
	string m_szHoverInfo;  // 鼠标位置信息
	bool m_bTer;  //!< 是否需要退出绘图线程

	string m_szName;	//!< 产品名称

public:

	IViewAdmin *m_pAdmin;  // 产品视图管理层指针
	CDYW::Plugin::ICommon  *m_IComm;     // 常用功能接口
	//IScanModeEdit *m_IScanMode;  // 扫描模式接口
	IProductInfo *m_IProInfo; // 产品信息接口
	IGISModule *m_IGISM;



	unsigned int m_iProNumber; // 产品类型编号

	bool m_bHasData;
	COLORREF m_clrBK;		// 背景颜色

	COLORREF m_clrCircle;	// 距离圈的颜色
	int m_iDrawKilo;	// 距离圈参数1
	int m_iDrawDeg;	// 距离圈参数2
	
	inline bool IsRefresh( )
	{
		return m_bRefresh;
	}

	inline void SetRefresh( bool bRefresh )
	{
		m_bRefresh = bRefresh;
	}

	inline bool IsDrawRuler()
	{
		return (m_Ruler>0?true:false);
	}

	inline void SetRuler( int DrawRuler )
	{
		m_Ruler = DrawRuler;
		if (DrawRuler==1)
		{
			m_iDrawDeg = 30;
		}
		else if(DrawRuler==2)
		{
			m_iDrawDeg = 45;
		}
	}

	inline bool IsDrawing()
	{
		return m_bIsDrawing;
	}

	inline bool IsScaleDrawing()
	{
		return m_bIsScaleDrawing;
	}
	bool isDrawingProduct()
	{
		if(m_bIsDrawing||m_bIsScaleDrawing) return true;
		else return false;
	}

public:
	
	ProductViewT( IViewAdmin *pAdmin, unsigned int iProNumber):
		m_pAdmin( NULL ),m_bTer( false),m_IGISM(NULL)
	{
		m_bHasData = false;
		m_bIsDrawing = false;
		m_bIsScaleDrawing = false;
		memset( &m_thrDrawID, 0, sizeof(pthread_t) );
		memset( &m_thrScaleID, 0, sizeof(pthread_t) );

		memset( &m_mutScale, 0, sizeof(pthread_mutex_t));
		pthread_mutex_init( &m_mutScale, NULL );

		m_pAdmin = pAdmin;
		if( m_pAdmin )
		{
			m_IComm = ( CDYW::Plugin::ICommon * )m_pAdmin->QueryInterfaceP( guid_Frame, guid_I_Common );
			//m_IScanMode = (IScanModeEdit *)m_pAdmin->QueryInterfaceP( g_PL_ScanModeEdit, g_I_ScanModeEdit );
			m_IProInfo = (IProductInfo *)m_pAdmin->QueryInterfaceP( g_PL_ProductInfo, g_I_ProductInfo );
			m_IGISM = ( IGISModule *)m_pAdmin->QueryInterfaceP( g_PL_GISModule, g_I_GISModule );
		}

		m_iProNumber = iProNumber;
		m_clrBK = RGB(0,0,0);
		m_clrCircle = RGB(103,103,103);
		m_iDrawKilo = 50;
		m_iDrawDeg = 45;
		m_bLButDown = false;
		m_pLeftDownDPara = NULL;
		m_bHost = false;

		SetRefresh( true );
		//int Ruler = m_pAdmin->GetViewFrame()->GetFrameAdmin()->GetRulerS();
		//SetRuler(Ruler);

		m_bPVMCode = true; //
		m_bReal = false;
		m_bReceData = false;
		m_szPVMCode = "";
	};

	~ProductViewT()
	{
		pthread_mutex_destroy( &m_mutScale );
	}

	void QueryInterGISM()
	{
		if ( m_IGISM == NULL )
		{
			m_IGISM = ( IGISModule *)m_pAdmin->QueryInterfaceP( g_PL_GISModule, g_I_GISModule );
		}
	}

	//! UI 交互操作序列开始
	bool OnActionStart( int iAct,int x, int y )
	{
		CDrawRadialT *pDPara = GetLayerBltPara();
		tagYWPoint pt;
		pt.x = x;
		pt.y = y;

		m_bLButDown = true;
		//YWTRACE("L Down: %s\r\n", m_bLButDown?"True":"False");

		if( IsScaleDrawing() ) return false;

		m_pLeftDownDPara = pDPara;
		T *pT = (T*)this;
		///// 

		wxWindow *pWin = (wxWindow*)m_pAdmin->GetCanvas();
		pWin->SetCursor( wxCursor(wxCURSOR_HAND ) );

		pDPara->TLButtonDown( pt.x, pt.y, 0, 0 );
		m_ptLeftDown = pt;

		return false;
	}

	//! UI 交互操作序列停止
	bool OnActionStop( int iAct, int x, int y )
	{
		if( !m_bLButDown ) return false;

		CDrawRadialT *pDPara = GetLayerBltPara();
		if( pDPara != m_pLeftDownDPara )
		{
			pDPara->TLButtonDown( m_ptLeftDown.x, m_ptLeftDown.y, 0, 0 );
			m_pLeftDownDPara = NULL;
		}

		///////
		tagYWPoint pt;
		pt.x = x;
		pt.y = y;

		wxWindow *pWin = (wxWindow*)m_pAdmin->GetCanvas();
		pWin->SetCursor( wxCursor(wxCURSOR_ARROW ) );
		
		///// 刷新

		if( pDPara->TLButtonUp(pt.x,pt.y, 0, 0 ) )
		{
			TPro *pPro = GetLayerBltData();
			HDC hdc = GetLayerBltDC();
			DrawScale( pPro, pDPara, hdc, true );

			tagViewComm comm;
			comm.pPView = (IProductView *)this;
			comm.funComm.iType = COMM_TYPE_POLAR;
			comm.funComm.polarCoord.latitude = 0;
			comm.funComm.polarCoord.longitude = 0;
			comm.funComm.polarCoord.X = pDPara->_X0;
			comm.funComm.polarCoord.Y = pDPara->_Y0;
			comm.funComm.polarCoord.detM = pDPara->_detM;
			comm.funComm.polarCoord.iRadius = pDPara->_iRadius;
			comm.funComm.polarCoord.iWinWid = pDPara->_width;

			m_pAdmin->FireComm( comm );
		}

		m_bLButDown = false;

		return false;
	}


	bool OnActionContinue( int iAct, int x, int y )
	{
		return false;
	}

	//! 鼠标左键按下
	void OnMouseDown(int x, int y, int offsetX, int offsetY)
	{

	};

	//! 鼠标左键弹起
	void OnMouseUp(int x, int y, int offsetX, int offsetY)
	{

	};

	//! 鼠标移动
	void OnMouseMove(int x, int y, int offsetX, int offsetY)
	{
		return;
	};

	//! 鼠标右键按下
	void OnRMouseDown(int x, int y, int offsetX, int offsetY)
	{

	};



	//! 菜单缩放命令
	void OnMenuZoom( unsigned short wID,int x, int y, long widView, long heiView, long offSetX, long offSetY )
	{
		if( IsDrawing() || IsScaleDrawing()) return;

		CDrawRadialT *pDPara = GetLayerBltPara();
		TPro *pPro = GetLayerBltData();
		HDC hdc = GetLayerBltDC();

		T *pT = (T*)this;
		/////
		int widV = widView, heiV = heiView;

		tagYWPoint ptOff;
		ptOff.x = offSetX;
		ptOff.y = offSetY;

		bool bRet = false;

		//////////////////////////////////////////////////////////////////////////
		switch( wID )
		{
		case ZOOM_ZOOMOUT:
			bRet = pDPara->ZoomOut( x, y, widV, heiV, ptOff.x, ptOff.y);
			break;
		case ZOOM_ZOOMIN:
			bRet = pDPara->ZoomIn(x, y, widV, heiV, ptOff.x, ptOff.y);
			break;
		case ZOOM_DEF:
			bRet = pDPara->ZoomDef();
			break;
		case ZOOM_X1:
			bRet = pDPara->ZoomScale(1, x, y, ptOff.x, ptOff.y );
			break;
		case ZOOM_X2:
			bRet = pDPara->ZoomScale(2, x, y, ptOff.x, ptOff.y );
			break;
		case ZOOM_X4:
			bRet = pDPara->ZoomScale(4, x, y, ptOff.x, ptOff.y );
			break;
		case ZOOM_X8:
			bRet = pDPara->ZoomScale(8, x, y, ptOff.x, ptOff.y );
			break;
		case ZOOM_RECENTER:
			bRet = pDPara->SetCenter(x, y, widV, heiV );
			break;
		case FULL_SCREEN_DIS:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		if( bRet )
		{
			SetRefresh( true );			
			DrawScale( pPro, pDPara, hdc, true );

			tagViewComm comm;
			comm.pPView = (IProductView *)this;
			comm.funComm.iType = COMM_TYPE_POLAR;
			comm.funComm.polarCoord.latitude = 0;
			comm.funComm.polarCoord.longitude = 0;
			comm.funComm.polarCoord.X = pDPara->_X0;
			comm.funComm.polarCoord.Y = pDPara->_Y0;
			comm.funComm.polarCoord.detM = pDPara->_detM;
			comm.funComm.polarCoord.iRadius = pDPara->_iRadius;
			comm.funComm.polarCoord.iWinWid = pDPara->_width;

			m_pAdmin->FireComm( comm );
		}
	};
	
	/*! RZ类直角坐标产品缩放操作
		\param wID 命令ID
		\param x X
		\param y Y
		\param widView 视图宽度
		\param heiView 视图高度
		\param fLenOfWin 窗口宽度
	*/
	void OnMenuZoomCoord( unsigned short wID,int x, int y, long widView, long heiView, float fLenOfWin)
	{
		if( IsDrawing() || IsScaleDrawing()) return;

		T *pT = (T*)this;

		CDrawRadialT *pDPara = GetLayerBltPara();
		TPro *pPro = GetLayerBltData();
		HDC hdc = GetLayerBltDC();

		/////
		int widV = widView, heiV = heiView;
		bool bRet = true;

		//////////////////////////////////////////////////////////////////////////
		switch( wID )
		{
		case ZOOM_ZOOMOUT:
			{
				pT->m_fScale -= pT->m_iScaleD;
				if( pT->m_fScale <= pT->m_iScaleD ) {
					pT->m_fScale = pT->m_iScaleD;
				}
			}
			break;
		case ZOOM_ZOOMIN:
			{
				pT->m_fScale += pT->m_iScaleD;
				if( pT->m_fScale > pT->m_sScale8 ) {
					pT->m_fScale = pT->m_sScale8;
				}
			}
			break;
		case ZOOM_DEF:
			pT->m_fScale = pT->m_sScale;
			pDPara->_X0 = pDPara->s_X0;
			pDPara->_Y0 = pDPara->s_Y0;
			break;
		case ZOOM_X1:
			{
				pT->m_fScale = pT->m_sScale;
			}
			break;
		case ZOOM_X2:
			{
				pT->m_fScale = pT->m_sScale * 2;
			}
			break;
		case ZOOM_X4:
			{
				pT->m_fScale = pT->m_sScale * 4;
			}
			break;
		case ZOOM_X8:
			{
				pT->m_fScale = pT->m_sScale * 8;
			}
			break;
		case ZOOM_RECENTER:
			{
				bRet = pDPara->SetCenter(x, y, widV, heiV );
			}
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		if( bRet )
		{
			SetRefresh( true );

			DrawScale( pPro, pDPara, hdc, true );

			pDPara->_detM = fLenOfWin * 1000.f/pT->m_fScale;

			tagViewComm comm;
			comm.pPView = (IProductView *)this;
			comm.funComm.iType = COMM_TYPE_POLAR;
			comm.funComm.polarCoord.latitude = 0;
			comm.funComm.polarCoord.longitude = 0;
			comm.funComm.polarCoord.X = pDPara->_X0;
			comm.funComm.polarCoord.Y = pDPara->_Y0;
			comm.funComm.polarCoord.detM = pDPara->_detM;
			comm.funComm.polarCoord.iRadius = pDPara->_iRadius;
			comm.funComm.polarCoord.iWinWid = pDPara->_width;

			m_pAdmin->FireComm( comm );
		}
	}

	void OnToolBarDef( unsigned short wID )
	{

	};


	/*! 获取鼠标位置产品数据信息
		模板函数缺省实现仅仅适用于产品解析从RVWPro类派生的函数
	*/
	bool GetMouseHoverPoint( int x, int y, tagYWPoint &pt)
	{
		return false;
	}
	bool GetMouseHoverXY( int &x, int &y, tagYWPoint pt)
	{
		return false;
	}
	bool GetMouseHoverPointSignal( int x, int y, tagYWPoint &pt)
	{
		T *pT = ( T* )this;
		CDrawRadialT *pDPara = GetLayerBltPara();

		if( !HasData() )  return false;


		float r = sqrt((float)((x - pDPara->_X0)*(x - pDPara->_X0) + (y - pDPara->_Y0)*(y - pDPara->_Y0) )) ;

		pt.fAzi = atan2((double)(x-pDPara->_X0),(double)(pDPara->_Y0-y)) * 180.0f /M_PI;

		if( pt.fAzi< 0 )
			pt.fAzi += 360.0f;

		//pt.fEle = pT->GetLayerBltData()->GetElevation();
		pt.fEle = 0;
		pt.fKilo = r * pDPara->_detM / 1000.f;
		pt.x = x;
		pt.y = y;

		int maxkilo = pT->GetLayerBltData()->GetBinWidth() * pT->GetLayerBltData()->GetRangeBinOfRad() / 1000;
		
		return true;
		if( int(pt.fKilo)<=maxkilo )
			return true;
		else
			return false;		
	}
	bool GetMouseHoverXYSignal( int &x, int &y, tagYWPoint pt)
	{
		T *pT = ( T* )this;
		CDrawRadialT *pDPara = GetLayerBltPara();

		if( !HasData() )  return false;

		int maxkilo = pT->GetLayerBltData()->GetBinWidth() * pT->GetLayerBltData()->GetRangeBinOfRad() / 1000;

		float r = pt.fKilo * 1000.0/pDPara->_detM;
		float Angle = pt.fAzi*M_PI/180.0f;
		float X_Del = sin(Angle)*r;
		float Y_Del = cos(Angle)*r;
		x = X_Del+pDPara->_X0;
		y = pDPara->_Y0-Y_Del;
		

		if( int(pt.fKilo)<maxkilo )
			return true;
		else
			return false;
	}
	// 获取指定点的产品数据信息
	const char *GetMouseHoverInfo( tagYWPoint &pt, bool bCurVA )
	{
		return m_szHoverInfo.c_str();
	}

	// 
	bool PutData( unsigned char*pBuffer, int lLen, int iType )
	{
		if( m_bTer ) return false;

		if( !m_bIsDrawing )
		{
			tagDrawProductPara *pPara = new tagDrawProductPara;

			pPara->pThis = this;
			pPara->pT = ( T* )this;
			pPara->iProId = 3;
			pPara->bKeepScale = true;
			pPara->pPro = GetLayerDrawData();
			pPara->hdc = GetLayerDrawDC();
			pPara->pDPara = GetLayerDrawPara();

			memset( &m_thrDrawID, 0, sizeof( pthread_t ));

			///// 获取数据

			if( pBuffer )
			int iRet = pPara->pPro->OpenBuff( (char*)pBuffer, lLen);

			m_bIsDrawing = true;
			int Ruler = m_pAdmin->GetViewFrame()->GetFrameAdmin()->GetRulerS();
			SetRuler(Ruler);
			pthread_create( &m_thrDrawID, NULL, thrDrawProduct, pPara );
			return true;
		}
		else
		{
			throw EXCEPTION_DRAW_BUSY;
		}

		return false;
	};


	static void *CB_CleanDrawProduct( void *pParaIn )
	{
		tagDrawProductPara *pPara = (tagDrawProductPara *)pParaIn;
		ProductViewT< T, TPro > *pThis = (ProductViewT< T, TPro > *)pPara->pThis;
		pThis->m_bIsDrawing = false;
		delete pPara;
		return NULL;
	}

	static void *thrDrawProduct( void *pParaIn )
	{
		///// 更改线程为 Detach 线程，当线程退出时，会自己清除资源和堆栈
		pthread_detach( pthread_self() );

		pthread_cleanup_push( CB_CleanDrawProduct, pParaIn );
		///// 
		tagDrawProductPara *pPara = (tagDrawProductPara *)pParaIn;
		ProductViewT< T, TPro > *pThis = (ProductViewT< T, TPro > *)pPara->pThis;

		pThis->m_bIsDrawing = true;

		// 生成产品文件名（不要区别码）
		CProductName name( pPara->pPro->GetDataName() );
		pThis->m_szName = name.GetName();
		pThis->m_szMCode = name.GetMCode();

		if( pPara->pT->DrawProduct(
			pPara->hdc,
			pPara->pDPara,
			pPara->pPro,
			pThis->HasData()?true:false) )
		{
			// 检测缩放绘制线程是否正在绘制产品。
			//   是则终止缩放绘制线程，取其缩放系数,设置标记；
			//   SwitchDC()
			//   是否设置标记
			//     是，启动缩放绘制线程
			bool bIsScaleDrawing = false;
			
			TPro *pPro = pThis->GetLayerBltData();
			CDrawRadialT *pDrawRadT = pThis->GetLayerBltPara();
			HDC hdc = pThis->GetLayerBltDC();
			pDrawRadT->s_kilo = pPara->pDPara->s_kilo;
			pDrawRadT->s_iRadius = pPara->pDPara->s_iRadius;
	
			if( pThis->IsScaleDrawing() )
			{
				bIsScaleDrawing = true;
				pthread_cancel( pThis->m_thrScaleID );
				pthread_join( pThis->m_thrScaleID, NULL );
			}

			pPara->pT->SwitchDC();
			pPara->pT->SetHasData( true );
			pPara->pT->m_pAdmin->Refresh( true );

			/////
			if( bIsScaleDrawing && pPro && pDrawRadT && hdc )
			{
				pThis->DrawScale( pPro, pDrawRadT, hdc, true );
			}
		}

		pThis->m_bIsDrawing = false;
		/////
		pthread_cleanup_pop( 1 ); // 无论如何都会调用 CB_CleanReceiveThr()函数，清理资源。
		return NULL;
	}

	bool DrawScale( TPro *pPro, CDrawRadialT *pDPara, HDC hdc, bool bKeepScale = true )
	{
		tagDrawProductPara *pPara = new tagDrawProductPara;
		pPara->pThis = this;
		pPara->pT = ( T* )this;
		pPara->iProId = GetProductNumber();
		pPara->bKeepScale = bKeepScale;
		pPara->pPro = pPro;
		pPara->hdc = hdc;
		pPara->pDPara = pDPara;


		if( IsScaleDrawing() )
		{
			pthread_cancel( m_thrScaleID );
			pthread_join( m_thrScaleID, NULL );
			delete pPara;
			return false;
		}

		m_bIsScaleDrawing = true;
		memset( &m_thrScaleID, 0, sizeof( pthread_t ));
		int Ruler = m_pAdmin->GetViewFrame()->GetFrameAdmin()->GetRulerS();
		SetRuler(Ruler);
		pthread_create( &m_thrScaleID, NULL, thrScaleDrawProduct, pPara );

		return true;
	}

	static void *CB_CleanScaleDrawProduct( void *pParaIn )
	{
		tagDrawProductPara *pPara = (tagDrawProductPara *)pParaIn;
		ProductViewT< T, TPro > *pThis = (ProductViewT< T, TPro > *)pPara->pThis;
		pThis->m_bIsScaleDrawing = false;
		delete pPara;
		//wxLogDebug( wxT("Stop Scale Draw" ));
		return NULL;
	}

	static void *thrScaleDrawProduct( void *pParaIn )
	{
	
		pthread_detach( pthread_self() );

		tagDrawProductPara *pPara = (tagDrawProductPara *)pParaIn;
		ProductViewT< T, TPro > *pThis = (ProductViewT< T, TPro > *)pPara->pThis;
		pThis->m_bIsScaleDrawing = true;
		pthread_cleanup_push( CB_CleanScaleDrawProduct, pParaIn );

		/////
		
		pPara->pT->DrawProduct( pPara->hdc,
			pPara->pDPara,
			pPara->pPro,
			pPara->bKeepScale);
		
		pThis->m_pAdmin->Refresh( true );

		//wxMilliSleep(500);
		pThis->m_bIsScaleDrawing = false;
		pthread_cleanup_pop( 1 ); // 参数为1时，无论如何都会调用 CB_CleanReceiveThr()函数，清理资源。

		return NULL;
	}

	//! 
	int GetData( unsigned char **pData, int &lLen )
	{
		return -1;
	};

	char *GetProductFileHeader( )
	{
		return NULL;
	}

	//! 
	inline bool HasData()
	{
		return m_bHasData;
	};

	void SetHasData( bool bHasData )
	{
		m_bHasData = bHasData;
	}


	//////////////////////////////////////////////////////////////////////////
	//! 
	void OnSuperDraw( void *pDC, void *pMemDC )
	{

	};

	void OnDrawInfo( void *pDC, void *pMemDC )
	{

	}

	void *GetBltDC(  )
	{
		return (void*)GetLayerBltDC();
	}

	void *GetDrawDC(  )
	{
		return (void*)GetLayerDrawDC();
	}

	//! 
	void RefreshView( bool bFresh )
	{

	};

	//////////////////////////////////////////////////////////////////////////
	//// 命令类处理

	//! 获取工具栏定义字符串
	char *GetToolbarString(  )
	{
		return NULL;
	}

	//! 
	bool OnCommandHandle( unsigned short /*wNotifyCode*/, unsigned short wID )
	{

		return false;
	};

	//! 
	void SupportMapCircle( bool &bMap, bool &bCircle )
	{
		bMap = false;
		bCircle = false;
	}

	//! 
	void Initialize( IN IViewAdmin *pAdmin )
	{

	}

	//! 获取产品索引
	unsigned int GetProductNumber()
	{
		return m_iProNumber;
	}

	bool SetProductNumber( unsigned int iProNum )
	{
		m_iProNumber = iProNum;
		return false;
	}

	//! 获取产品视图层当前打开产品的区别码
	const char *GetMCode()
	{
		if( HasData() )
		{
			return m_szMCode.c_str();
		}

		return NULL;
	}

	//! 获取视图层的区别码，此区别码用于确定视图层接收数据的类型和参数
	const char *GetPVMCode()
	{
		if( m_bPVMCode  )
			return m_szPVMCode.c_str();

		return NULL;
	}

	const char *GetMCodeString( )
	{
		return m_szMCodeStr.c_str();
	}

	const char *GetDateTimeString( )
	{
		return m_szProDateTime.c_str();
	}

	void SetPVMCode( const char *mcode )
	{
		string szMCode;
		if( mcode != NULL )
		{
			szMCode = mcode;
		}

		if( szMCode.empty() )
		{
			m_bPVMCode = false;
		}
		else
		{
			m_bPVMCode = true;
			m_szPVMCode = mcode;
		}
	}

	bool IsSupportProduct( const char *szMCode, bool bReal )
	{
		if( !IsReceiveData() ) return false;
		if( m_bReal != bReal ) return false;

		string sMCode( szMCode );
		CMCodePhrase mcode( szMCode );

		if( GetProductNumber() == mcode.GetProductNumber() )
		{
			if( m_bPVMCode )
			{
				if( m_szPVMCode == sMCode )
					return true;
				else
					return false;
			}
			return true;	
		}

		return false;
	}


	//! 是否能够接收数据
	bool IsReceiveData()
	{
		return m_bReceData;
	}

	//! 设置数据接收标记
	void SetReceiveData( bool bRece )
	{
		m_bReceData = bRece;
	}

	//! 是否能够接收实时数据
	bool IsReceiveReal( )
	{	
		return m_bReal;
	}

	//! 设置数据接收标记
	void SetReceiveReal( bool bReal )
	{
		m_bReal = bReal;
	}


	//! 设置/取消产品视图层为 Host 视图层属性
	inline void SetHostProduct( bool bHost )
	{
		m_bHost = bHost;
	}

	//! 产品视图层是否为Host视图层
	inline bool IsHostProduct()
	{
		return m_bHost;
	}

	//! 获取产品是图层名称
	const char* GetName()
	{
		return m_szName.c_str();
	}

	//! 获取产品是图层注释信息
	const char* GetNote()
	{
		return "";
	}

	//! 获取产品视图层版本信息
	const char* GetVersion()
	{
		return "";
	}

	//! 获取产品视图层
	const char* GetAuthor()
	{
		return "lava_sdb";
	};

	/*! 通信处理函数
		接收来自视图管理层的通信信息
	*/
	void OnViewComm( tagViewAdminComm &comm )
	{
		switch( comm.funComm.iType )
		{
			case COMM_TYPE_POLAR:
			{
				T *pT = (T*)this;
				CDrawRadialT *pDPara = GetLayerBltPara();
				/////
				pDPara->_detM = comm.funComm.polarCoord.detM;
				pDPara->_det = pDPara->_detM / pDPara->_iKuChang;
				pDPara->_iRadius = (float)pDPara->_kilo * 1000./pDPara->_detM;

				float fDet = float(pDPara->_width)/comm.funComm.polarCoord.iWinWid;

				pDPara->_X0 = comm.funComm.polarCoord.X * fDet;
				pDPara->_Y0 = comm.funComm.polarCoord.Y * fDet;

				DrawScale( GetLayerBltData(), pDPara, GetLayerBltDC(), true );
			}
			break;
			case COMM_TYPE_GET_PRODUCTVIEW_STATUS:
			{
				m_bTer = true;
			}
			break;
			case COMM_TYPE_VA_RESIZE:
				{
					ClearLayer();  //! 重新创建分层
					DrawScale( GetLayerBltData(), GetLayerBltPara(), GetLayerBltDC(), false );
				}
				break;
		}
	};

	void OnViewCommCoord( tagViewAdminComm &comm, float fLenOfWin )
	{
		switch( comm.funComm.iType )
		{
		case COMM_TYPE_POLAR:
			{
				if( !HasData() ) return;

				T *pT = (T*)this;
				CDrawRadialT *pDPara = GetLayerBltPara();
				TPro *pPro = GetLayerBltData();
				HDC hdc = GetLayerBltDC();
				
				/////
				pDPara->_detM = comm.funComm.polarCoord.detM;
				pDPara->_det = pDPara->_detM / pDPara->_iKuChang;
				pDPara->_iRadius = (float)pDPara->_kilo * 1000./pDPara->_detM;

				pT->m_fScale = fLenOfWin * 1000.f / pDPara->_detM; //_detM
				float fDet = float(pDPara->_width)/comm.funComm.polarCoord.iWinWid;

				pDPara->_X0 = comm.funComm.polarCoord.X * fDet;
				pDPara->_Y0 = comm.funComm.polarCoord.Y * fDet;

				DrawScale( pPro, pDPara, hdc, true );
			}
			break;
		case COMM_TYPE_GET_PRODUCTVIEW_STATUS:
			{
				m_bTer = true;
			}
			break;
		case COMM_TYPE_VA_RESIZE:
			{
				ClearLayer();  //! 重新创建分层
				DrawScale( GetLayerBltData(), GetLayerBltPara(), GetLayerBltDC(), false );
			}
		}

	};

	bool GetViewComm( tagComm &comm )
	{
		T *pT = (T*)this;
		
		if( comm.iType == COMM_TYPE_GET_PRODUCTVIEW_STATUS )
		{
			comm.pvStatus.status = ( IsScaleDrawing()||IsDrawing() )?1:0;
			return true;
		}else{
			/////
			if( !HasData() ) return false;

			/////
			CDrawRadialT *pDPara = GetLayerBltPara();

			comm.iType = COMM_TYPE_POLAR;
			comm.polarCoord.latitude = 0;
			comm.polarCoord.longitude = 0;
			comm.polarCoord.X = pDPara->_X0;
			comm.polarCoord.Y = pDPara->_Y0;
			comm.polarCoord.detM = pDPara->_detM;
			comm.polarCoord.iRadius = pDPara->_iRadius;
			comm.polarCoord.iWinWid = pDPara->_width;
		}
		/////
		return true;
	}


	IViewAdmin *GetViewAdmin()
	{
		return m_pAdmin;
	};

	/************************************************************************/
	///// 填充基本产品信息
	void FillBaseProductInfo( tagRealFile &he )
	{
		memset( m_szInfo, 0, 20 * 60 );

		IFrameAdmin* pFA = GetViewAdmin()->GetViewFrame()->GetFrameAdmin();

		unsigned char wavForm = he.obserSec.batch.wavForm[0];
		unsigned short int usProType = he.obserSec.iProductNumber;
		int iBinNumber = 0;
		int iRefBinLen = 0;

		switch( wavForm)
		{
		case 0:// LCS,R
			{
				iBinNumber = he.obserSec.usRefBinNumber[0];
				iRefBinLen = he.obserSec.iRefBinLen[0];
			}
			break;
		case 1: // LCD, V,W
			{
				iBinNumber = he.obserSec.batch.usDopBinNumber[0];
				iRefBinLen = he.obserSec.iDopBinLen[0];
			}
			break;
		case 2: //HCD, R, V,W
		case 4:
		case 5:
		case 6:
		case 8:
			{
				if( (usProType == PID_VEL) || 
					(usProType == PID_WID)
					) {
					iBinNumber = he.obserSec.batch.usDopBinNumber[0];
					iRefBinLen = he.obserSec.iDopBinLen[0];
				} else {
					iBinNumber = he.obserSec.usRefBinNumber[0];
					iRefBinLen = he.obserSec.iRefBinLen[0];

				}
			}
			break;
		}

		sprintf( m_szInfo[0], "%s(%s)",
			m_IProInfo->GetProductName( he.obserSec.iProductNumber ),
			m_IProInfo->GetProductEName( he.obserSec.iProductNumber));

		sprintf( m_szInfo[1],("雷达型号: %s") ,
			he.addSec.RadarType);

		sprintf( m_szInfo[2], "%s: %s", 
			pFA->GetText( ID_TXT_SITENAME ),
			he.addSec.Station );

		string szAddr = pFA->GetText( ID_TXT_ADDR );
		string szCate = pFA->GetText( ID_TXT_SITENUM );

		if( szCate != "" )
		{
			sprintf( m_szInfo[3],("%s: %s"),
				pFA->GetText( ID_TXT_SITENUM),
				he.addSec.StationNo);
		}

		CDYW::JWD jwd(he.addSec.LongitudeV/360000., he.addSec.LatitudeV/360000.);

		char Pos[50] = {0};
		if ( jwd.m_LoDeg>0 )
		{
			sprintf( Pos, "%s", "E"  );
		}else{
			sprintf( Pos, "%s", "W"  );
		}
		sprintf( m_szInfo[4],("经度: %s %d°%d′%.2f″"), 
			/*he.addSec.Longitude[0]*/Pos,
			(int)jwd.m_LoDeg, (int)jwd.m_LoMin, jwd.m_LoSec);

		if ( jwd.m_LaDeg>0 )
		{
			sprintf( Pos, "%s", "N"  );
		}else{
			sprintf( Pos, "%s", "S"  );
		}

		sprintf( m_szInfo[5],("纬度: %s %d°%d′%.2f″ "), 
			/*he.addSec.Latitude[0]*/Pos,
			(int)jwd.m_LaDeg , (int)jwd.m_LaMin , jwd.m_LaSec);

		sprintf( m_szInfo[6],("天线海拔: %.f m "),he.addSec.Height/1000.0f);

		/////////
		//int mode = he.obserSec.batch.scanmode;

		//sprintf( m_szInfo[7],( "扫描模式: %s"),
		//	m_IScanMode->GetWeatherModeClassName(mode));
		if (he.addSec.Type[0] == 1)
		{
			sprintf(m_szInfo[7],("扫描模式: %s"),he.addSec.ScanModeName);
		}
		else
		{
			sprintf( m_szInfo[7],( "扫描模式: %s"),"未知");
		}
		
		//////
		int iMaxKm = ceil(iBinNumber * iRefBinLen*1.0f / 1000 );
		sprintf( m_szInfo[8],("最大测距: %d km ") ,iMaxKm);
		sprintf( m_szInfo[9],( "距离分辨率: %d m "), iRefBinLen);

		sprintf( m_szInfo[10],("日期: %02d/%02d/%02d"), 
			he.obserSec.iObsStartTimeYear,
			he.obserSec.iObsStartTimeMonth,
			he.obserSec.iObsStartTimeDay);

		sprintf( m_szInfo[11],("时间: %02d:%02d:%02d"),
			he.obserSec.iObsStartTimeHour,
			he.obserSec.iObsStartTimeMinute,
			he.obserSec.iObsStartTimeSecond);

		// } 
	}

	inline bool IsDrawInfo() 
	{
		return m_bDrawInfo;
	}

	inline void  SetDrawInfo( bool bDraw )
	{
		m_bDrawInfo = bDraw;
	}

	//鼠标移动画映射鼠标
	//void DrawMouse( wxPoint pt)
	//{
	//	
	//	//HDC dc = GetLayerBltDC();
	//	//HPEN pen = ::CreatePen( PS_SOLID, 1 ,RGB( 255,255,255 ) );
	//	//HBRUSH oldBru = (HBRUSH)::SelectObject( dc, ::GetStockObject( BLACK_BRUSH ) );
	//	//HPEN oldPen = (HPEN)::SelectObject( dc, ::GetStockObject( NULL_PEN ) );

	//	//::MoveToEx( dc, pt.x-10, pt.y, NULL );
	//	//::LineTo( dc, pt.x+10, pt.y );
	//	//::MoveToEx( dc, pt.x, pt.y-10, NULL );
	//	//::LineTo( dc, pt.x, pt.y+10 );

	//	//::SelectObject( dc, oldPen );
	//	//::SelectObject( dc, oldBru );
	//	//::DeleteObject( pen );

	//}

	// 绘制产品信息
	void DrawInfoT( HDC hdc,int cntInfoItems, const char *szT,int dataType=1, int iPre=2, bool bIsHost = true)
	{
		T *pPro = (T*)this;
		///// 
		int iWid = pPro->GetViewAdmin()->GetWidth();
		int iHei = pPro->GetViewAdmin()->GetHeight();
		int iLeft = 0;

		HPEN penCir = ::CreatePen( PS_SOLID, 1, RGB(77,77,77) );

		if( iWid > iHei )
		{
			//HFONT font = ::CreatePointFont(100 ,"仿宋_GB2312", 0, true );
			HFONT font = ::CreatePointFont(100 ,"宋体", 0, true );
			HFONT oldFont = (HFONT)::SelectObject( hdc, font);

			///// 
			iWid = iWid - iHei;
			iLeft = iHei + 5;
			
			HBRUSH oldBru = (HBRUSH)::SelectObject( hdc, ::GetStockObject( BLACK_BRUSH ) );
			HPEN oldPen = (HPEN)::SelectObject( hdc, ::GetStockObject( NULL_PEN ) );
			
			::Rectangle( hdc, iLeft - 5, 0, iLeft + iWid, iHei );

			if( bIsHost ) 
			{
				///// 绘制文字信息
				COLORREF oldClr = ::SetTextColor( hdc, RGB(255, 125, 50) );
				int x0 = iLeft;
				int widTxt = -1;	//! 文字宽度
				RECT reT = { iLeft, 5, iLeft + iWid, 3 };

				if( iWid > 100 ){
					// 显示产品基本信息

					/// 获取文本高度
					::DrawTextA( hdc,  m_szInfo[0], strlen(m_szInfo[0]), &reT, DT_CALCRECT | DT_LEFT );
					int hTxt = reT.bottom - reT.top;
					int hTxt2 = hTxt * 0.5;

					reT.left = x0; reT.right = iLeft + iWid; reT.bottom = reT.top + hTxt;

					::DrawTextA(  hdc, m_szInfo[0], (int)strlen(m_szInfo[0]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[1], (int)strlen(m_szInfo[1]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					
					::DrawTextA(  hdc, m_szInfo[2], (int)strlen(m_szInfo[2]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					
					int len = (int)strlen( m_szInfo[3] );
					if( len > 0 )
					{
						::DrawTextA(  hdc, m_szInfo[3], (int)strlen(m_szInfo[3]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					}
					::DrawTextA(  hdc, m_szInfo[4], (int)strlen(m_szInfo[4]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[5], (int)strlen(m_szInfo[5]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[6], (int)strlen(m_szInfo[6]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt; reT.bottom = reT.top + hTxt;
					
					::SetTextColor( hdc, RGB(75, 175, 250));
					::DrawTextA(  hdc, m_szInfo[7], (int)strlen(m_szInfo[7]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;

					SetTextColor( hdc, RGB(75, 175, 150));
					::DrawTextA(  hdc, m_szInfo[8], (int)strlen(m_szInfo[8]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[9], (int)strlen(m_szInfo[9]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[10], (int)strlen(m_szInfo[10]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					::DrawTextA(  hdc, m_szInfo[11], (int)strlen(m_szInfo[11]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					
					// $ $ $ $ $ $ {  显示产品特有信息， 不同产品不同
					SetTextColor( hdc, RGB(205, 235, 220));
					int curIdx = INFO_BASE_CNT + 1;
					while ( curIdx <= cntInfoItems )
					{
						::DrawTextA(  hdc, m_szInfo[curIdx], (int)strlen(m_szInfo[curIdx]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
						curIdx++;
					}

					int i = 0;
					for( i=0; i<=cntInfoItems; i++ )
					{
						::DrawTextA( hdc,  m_szInfo[i], (int)strlen(m_szInfo[i]), &reT, DT_CALCRECT | DT_LEFT | DT_NOCLIP );
						int wT = reT.right - reT.left;
						if( wT > widTxt )
							widTxt = wT;
					}

					// $ $ $ $ $ $ {  显示产品特有信息， 不同产品不同

					/*SetTextColor( hdc, RGB(205, 235, 220));
					int curIdx = INFO_BASE_CNT + 1;
					while ( curIdx <= cntInfoItems )
					{
						::DrawTextA(  hdc, m_szInfo[curIdx], (int)strlen(m_szInfo[curIdx]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
						curIdx++;

						::DrawTextA( hdc,  m_szInfo[i], (int)strlen(m_szInfo[curIdx]), &reT, DT_CALCRECT );
						int wT = reT.right - reT.left;
						if( wT > widTxt )
							widTxt = wT;
					}*/

					//// 绘制矩形框
					::SelectObject( hdc, ::GetStockObject( NULL_BRUSH ) );
					::SelectObject( hdc, penCir );

					reT.right = reT.left + widTxt;
					::RoundRect( hdc, reT.left-3, 3, reT.right, reT.top,6,6 );
				}

				//////////////////////////////////////////////////////////////////////////
				///// 绘制色标
				RECT rc = { iLeft, reT.top + 5, reT.right, iHei - 5 };

				HFONT fontTinyMark = ::CreatePointFont(80 ,"宋体");
				HFONT fontMark = ::CreatePointFont(100 ,"宋体");
				::SelectObject( hdc, ( iWid<=100)?fontTinyMark:fontMark );
				::SelectObject( hdc, fontTinyMark );

				pPro->DrawColorTable( hdc, rc, szT,dataType, iPre);

				::SetTextColor( hdc, oldClr );
				::DeleteObject( fontTinyMark );
				::DeleteObject( fontMark );

				//// 绘制色表的矩形框
				//::SelectObject( hdc, ::GetStockObject( NULL_BRUSH ) );
				//::SelectObject( hdc, penCir );

				//reT.right = reT.left + widTxt + 30;
				//::Rectangle( hdc, rc.left-3, rc.top, rc.right, rc.bottom );
				//::SelectObject( hdc, oldPen );
				
			}

			::SelectObject( hdc, oldFont );
			::SelectObject( hdc, oldBru );
			::SelectObject( hdc, oldPen );
			::DeleteObject( font );
		}

		::DeleteObject( penCir );
	}


	// 绘制产品信息
	tagYWRect DrawInfoT2( HDC hdc,int cntInfoItems, tagYWRect &rect)
	{
		tagYWRect rectRet = rect;

		///// 
		int iWid = rect.right - rect.left;
		int iHei = rect.bottom - rect.top;
		int iLeft = rect.left;

		HPEN penCir = ::CreatePen( PS_SOLID, 1, RGB(77,77,77) );

		//HFONT font = ::CreatePointFont(100 ,"仿宋_GB2312", 0, true );
		HFONT font = ::CreatePointFont(100 ,"宋体", 0, true );
		HFONT oldFont = (HFONT)::SelectObject( hdc, font);

		//
		iLeft += 5;

		HBRUSH oldBru = (HBRUSH)::SelectObject( hdc, ::GetStockObject( BLACK_BRUSH ) );
		HPEN oldPen = (HPEN)::SelectObject( hdc, ::GetStockObject( NULL_PEN ) );

		::Rectangle( hdc, iLeft - 5, 0, iLeft + iWid, iHei );

		{
			///// 绘制文字信息
			COLORREF oldClr = ::SetTextColor( hdc, RGB(255, 125, 50) );
			int x0 = iLeft;
			int widTxt = -1;	//! 文字宽度
			RECT reT = { iLeft, 5, iLeft + iWid, 3 };

			if( iWid > 100 ){
				/// 获取文本高度
				::DrawTextA( hdc,  m_szInfo[0], strlen(m_szInfo[0]), &reT, DT_CALCRECT | DT_LEFT );
				int hTxt = reT.bottom - reT.top;
				int hTxt2 = hTxt * 0.5;

				reT.left = x0; reT.right = iLeft + iWid; reT.bottom = reT.top + hTxt;

				::DrawTextA(  hdc, m_szInfo[0], strlen(m_szInfo[0]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[1], strlen(m_szInfo[1]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;

				::DrawTextA(  hdc, m_szInfo[2], strlen(m_szInfo[2]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;

				int len = strlen( m_szInfo[3] );
				if( len > 0 )
				{
					::DrawTextA(  hdc, m_szInfo[3], strlen(m_szInfo[3]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				}
				::DrawTextA(  hdc, m_szInfo[4], strlen(m_szInfo[4]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[5], strlen(m_szInfo[5]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[6], strlen(m_szInfo[6]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt; reT.bottom = reT.top + hTxt;

				::SetTextColor( hdc, RGB(75, 175, 250));
				::DrawTextA(  hdc, m_szInfo[7], strlen(m_szInfo[7]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[8], strlen(m_szInfo[8]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt; reT.bottom = reT.top + hTxt;

				SetTextColor( hdc, RGB(75, 175, 150));
				::DrawTextA(  hdc, m_szInfo[9], strlen(m_szInfo[9]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[10], strlen(m_szInfo[10]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[11], strlen(m_szInfo[11]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				::DrawTextA(  hdc, m_szInfo[12], strlen(m_szInfo[12]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;

				// $ $ $ $ $ $ {  显示产品特有信息， 不同产品不同
				SetTextColor( hdc, RGB(205, 235, 220));
				int curIdx = INFO_BASE_CNT + 1;
				while ( curIdx <= cntInfoItems )
				{
					::DrawTextA(  hdc, m_szInfo[curIdx], (int)strlen(m_szInfo[curIdx]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
					curIdx++;
				}

				int i = 0;
				for( i=0; i<=cntInfoItems; i++ )
				{
					::DrawTextA( hdc,  m_szInfo[i], (int)strlen(m_szInfo[i]), &reT, DT_CALCRECT | DT_LEFT | DT_NOCLIP );
					int wT = reT.right - reT.left;
					if( wT > widTxt )
						widTxt = wT;
				}

				//// $ $ $ $ $ $ {  显示产品特有信息， 不同产品不同

				//SetTextColor( hdc, RGB(205, 235, 220));
				//int curIdx = INFO_BASE_CNT + 1;
				//while ( curIdx <= cntInfoItems )
				//{
				//	::DrawTextA(  hdc, m_szInfo[curIdx], strlen(m_szInfo[curIdx]), &reT, DT_LEFT ); reT.top = reT.bottom + hTxt2; reT.bottom = reT.top + hTxt;
				//	curIdx++;

				//	::DrawTextA( hdc,  m_szInfo[i], strlen(m_szInfo[curIdx]), &reT, DT_CALCRECT );
				//	int wT = reT.right - reT.left;
				//	if( wT > widTxt )
				//		widTxt = wT;
				//}

				//// 绘制矩形框
				::SelectObject( hdc, ::GetStockObject( NULL_BRUSH ) );
				::SelectObject( hdc, penCir );

				reT.right = reT.left + widTxt;
				::RoundRect( hdc, reT.left-3, 3, reT.right, reT.top, 6 , 6 );
				rectRet.left = reT.left;
				rectRet.top = reT.top;
				rectRet.right = reT.right;
				rectRet.bottom = reT.bottom;
			}
		}

		::SelectObject( hdc, oldFont );
		::SelectObject( hdc, oldBru );
		::SelectObject( hdc, oldPen );
		::DeleteObject( font );


		::DeleteObject( penCir );

		return rectRet;
	}

	/************************************************************************/
	///// 一些平滑或其他算法调用

	typedef bool (*MedianVaLFiltChar)(unsigned char *pData,int radCnt, int radLen, int dataOff ,int dataLen ,int LenOfWin);
	typedef bool (*MedianVaLFiltShort)(unsigned short *pData,int radCnt, int radLen, int dataOff ,int dataLen ,int LenOfWin);
	typedef bool (*MedianVaLFiltInt)(unsigned int *pData,int radCnt, int radLen, int dataOff ,int dataLen ,int LenOfWin);
	typedef bool (*MedianValFiltNormalC)(unsigned char *pData,int height, int width,int LenOfWin );

	void MedianFitChar(unsigned char *pData,
		int radCnt, int radLen, 
		int dataOff, int RBNumber,
		int LenOfWin)
	{
		T *pThis = ( T* )this;

		string szDllPath = pThis->m_IComm->GetWorkPath();
		szDllPath += "\\Products\\YWArith.dll";

		MedianVaLFiltChar pFun = NULL;
		HMODULE hLib = (HMODULE)::LoadLibraryA( szDllPath.c_str() );
		if( hLib) {
			pFun = (MedianVaLFiltChar)::GetProcAddress( hLib, "MedianFilt" );
			if( pFun && pData )
			{
				pFun( pData, radCnt, radLen, dataOff, RBNumber, LenOfWin);
			}
			::FreeLibrary( hLib );
		}
	}

	void MedianFitShort(unsigned short *pData,
		int radCnt, int radLen, 
		int dataOff, int RBNumber,
		int LenOfWin)
	{
		T *pThis = ( T* )this;

		string szDllPath = pThis->m_IComm->GetWorkPath();
		szDllPath += "\\Products\\YWArith.dll";

		MedianVaLFiltShort  pFun = NULL;
		HMODULE hLib = (HMODULE)::LoadLibraryA( szDllPath.c_str() );
		if( hLib) {
			pFun = (MedianVaLFiltShort)::GetProcAddress( hLib, "MedianFiltShort" );
			if( pFun && pData )
			{
				pFun( pData, radCnt, radLen, dataOff, RBNumber, LenOfWin);
			}
			::FreeLibrary( hLib );
		}
	}

	void MedianFitInt(unsigned int *pData,
		int radCnt, int radLen, 
		int dataOff, int RBNumber,
		int LenOfWin)
	{
		T *pThis = ( T* )this;

		string szDllPath = pThis->m_IComm->GetWorkPath();
		szDllPath += "\\Products\\YWArith.dll";

		MedianVaLFiltInt  pFun = NULL;
		HMODULE hLib = (HMODULE)::LoadLibraryA( szDllPath.c_str() );
		if( hLib) {
			pFun = (MedianVaLFiltInt)::GetProcAddress( hLib, "MedianFiltInt" );
			if( pFun && pData )
			{
				pFun( pData, radCnt, radLen, dataOff, RBNumber, LenOfWin);
			}
			::FreeLibrary( hLib );
		}
	}

	//!< 中值滤波
	void MedianFitNormalC(
			unsigned char *pData,
			int height, 
			int width,
			int LenOfWin 
		)
	{
		T *pThis = ( T* )this;

		string szDllPath = pThis->m_IComm->GetWorkPath();
		szDllPath += "\\Products\\YWArith.dll";
		// 读入 SS 算法
		MedianValFiltNormalC  pFun = NULL;

		HMODULE hLib = (HMODULE)::LoadLibraryA( szDllPath.c_str() );
		if( hLib) {
			pFun = (MedianValFiltNormalC)::GetProcAddress( hLib,"MedianFiltNormalC" );
			if( pFun && pData )
			{
				pFun( pData, 
					height,
					width,
					3
					);
			}

			::FreeLibrary( hLib );
		}
	}

	
	//////////////////////////////////////////////////////////////////////////
	void EnableToolBarCommand(  unsigned long id, bool bEnable)
	{
		tagViewComm comm;
		comm.pPView = (IProductView *)this;
		comm.pView = NULL;
		comm.pFrm = NULL;
		comm.funComm.iType = COMM_TYPE_TB_CMD;
		comm.funComm.tbCMD.id = id;
		comm.funComm.tbCMD.bVal = bEnable;
		comm.funComm.tbCMD.IsEnable = true;
		GetViewAdmin()->FireComm( comm );
	}

	//! Check 指定 id 的命令
	void CheckToolBarCommand( unsigned long id, bool bCheck )
	{
		tagViewComm comm;
		comm.pPView = (IProductView *)this;
		comm.pView = NULL;
		comm.pFrm = NULL;
		comm.funComm.iType = COMM_TYPE_TB_CMD;
		comm.funComm.tbCMD.id = id;
		comm.funComm.tbCMD.bVal = bCheck;
		comm.funComm.tbCMD.IsEnable = false;
		GetViewAdmin()->FireComm( comm );
	}


protected:
	IViewAdmin* GetVA() { return m_pAdmin; };

	void OnDrawMap( TPro *pPro, CDrawRadialT *pDPara, HDC hdc )
	{
		QueryInterGISM();

		if( m_IGISM )
		{
			tagDrawMapWin tDMW;
			tDMW.det = pDPara->_detM;
			tDMW.hdc = hdc;
			tDMW.X0 = pDPara->_X0;
			tDMW.Y0 = pDPara->_Y0;
			//// 使用实际的经纬度来画地图
			tDMW.Longitude = pPro->m_Header.addSec.LongitudeV;
			tDMW.Latitude = pPro->m_Header.addSec.LatitudeV;
			tDMW.elevation = 0;//pPro->GetElevation() *M_PI / 180.0f;
			
			tagYWRect rect = GetViewAdmin()->GetViewRect();
			tDMW.width = rect.right - rect.left;
			tDMW.height = rect.bottom - rect.top;

			int len = sizeof( tDMW );
			m_IGISM->DrawData( (unsigned char *)&tDMW, len );
		}
	}
	void OnDrawMap( TPro *pPro, CDrawRadialT *pDPara, HDC hdc,float fEle )
	{
		QueryInterGISM();

		if( m_IGISM )
		{
			tagDrawMapWin tDMW;
			tDMW.det = pDPara->_detM;
			tDMW.hdc = hdc;
			tDMW.X0 = pDPara->_X0;
			tDMW.Y0 = pDPara->_Y0;
			//// 使用实际的经纬度来画地图
			tDMW.Longitude = pPro->m_Header.addSec.LongitudeV;
			tDMW.Latitude = pPro->m_Header.addSec.LatitudeV;
			tDMW.elevation = 0;//fEle *M_PI / 180.0f;			
			tagYWRect rect = GetViewAdmin()->GetViewRect();
			tDMW.width = rect.right - rect.left;
			tDMW.height = rect.bottom - rect.top;

			int len = sizeof( tDMW );
			m_IGISM->DrawData( (unsigned char *)&tDMW, len );
		}
	}


public:
	//! 持久化，获取 Layer的相关属性
	const char *Persistence( int iPos )
	{
		char szLayerInfo[ 1024 ] = {0};
		
		string szPVMCode;
		if( GetPVMCode() != NULL )
			szPVMCode = GetPVMCode();

		int iViewSize = m_pAdmin->GetViewSize();

		sprintf( szLayerInfo,
			"\n\t\t\t\t<Layer index=\"%d\" productIndex=\"%d\" real=\"%d\" rece=\"%d\" x=\"0\" y=\"0\" width=\"1\" height=\"1\" mcode=\"%s\" data=\"%s\" />",
			iPos,
			GetProductNumber(),
			IsReceiveReal()?1:0,
			IsReceiveData()?1:0,
			szPVMCode.c_str(),
			GetName()
			);
		
		m_szPersistence = szLayerInfo;
		return m_szPersistence.c_str();
	}
	void *GetHistoGramParam(){return NULL;}
	void CalcRectRain(int StartX,int StartY,int EndX,int EndY) {};
};



#endif // CProductViewT_h__

