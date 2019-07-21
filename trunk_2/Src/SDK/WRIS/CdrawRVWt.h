
#ifndef _TEMPLATE_DRAWRVW_20081231
#define _TEMPLATE_DRAWRVW_20081231

#define TPOINT POINT
#define TRECT RECT

#include "../../include/WRIS/ywmath.h"
#include "../../interface/ProductDisplay/drawfactor.h"
#include <include/WRIS/WRISGDI.h>
#include <pthread.h>
#include "../../include/WRIS/JWD.h"

struct tagPointMes
{
	short int x,y;		//ֱ������ �� ������İ뾶
	short int iRad;		//�������
	short int iRB;		//�������
};

struct XYCoor
{
	double x;
	double y;
};

namespace CDYW 
{
namespace RadarSystem 
{

class CDrawProductT
{
public:
	int _width;			// ��ʾ������
	int _height;		// ��ʾ����߶�

	int s_wid;
	int s_hei;

	int _action;		// ����

public:
	TRECT	 _Rect;
	TPOINT   _down;
	TPOINT   _last;  
	TPOINT   _zoom_in_first;
	TPOINT   _zoom_in_last;
	TRECT	_rectOld;

public:


	CDrawProductT()
	{
		_action = -1;
	}

	inline int TGetAction() { return _action; };
	inline int TSetAction(int action) { _action = action; return _action;};

};

class CDrawRadialT:public CDrawProductT
{
private:
	float _fEl;
public:
	int _X0,_Y0;		// ������ԭ���Ӧ�Ķ�ά����
	int _long,_lati;	// ������ľ�γ��
	int _iRadius;		// ���Ʋ�Ʒ����󼫰뾶����λ�����أ�
	int s_iRadius;

	float _det;			// ѹ�����ӣ�ÿ���ش���ľ������
	float _detM;		// ѹ�����ӣ�ÿ���ش���ľ��루��λ���ף�
	int _nRangeBin;		// ÿ������ľ��������
	int _iKuChang;		// ÿ�������ĳ���(��λ����)

	float _detMax8;		// �������8������ʱ����ѹ�����ӣ�ÿ���ش���ľ������
	float _detMMax8;	// �������8������ʱ����ѹ�����ӣ�ÿ���ش���ľ������
	float _kilo;			// ���ɲ����       //���̽������ͶӰ����
	float s_kilo;			//����࣬���ڼ�¼�����������Ƿ����仯�������̷����仯ʱ��
						//��֤��ͼ���򲻱䣬���¼�������ϵ��, ���뵵���ÿһ���ش���ľ�������


	float _detOut,s_detOut; // ��С������ϵ��
	int s_X0, s_Y0;
private:
	int _radCnt;			// Cut�ľ������
	float s_det;
public:
	tagPointMes *_pPointMes;  //���е������ת����
	int m_PointsSize;
	float _bufDet;
	float _bufRadius;
	int _bufX;
	int _bufY;

public:
	//Coordinate_Trans *pCDTrans;
	double RadarMecX;//�״ﾭ������Ӧ��ī���о��룬��λ����
	double RadarMecY;//�״�γ������Ӧ��ī���о��룬��λ����

	double RadarLon;//�״ﾭ�ȣ���λ����
	double RadarLat;//�״�γ�ȣ���λ����

	//	CRDGEODETIC RadarBHL;//�״����ĵĴ������
	//CRDGEODETIC Center;//�״����Ķ�Ӧ�Ĵ������

	XYCoor Merc_T;
	XYCoor Merc_L;

	//mercator��Χ
	double Merc_Right;
	double Merc_Left;
	double Merc_Top;
	double Merc_Bottom;

	//��γ�ȷ�Χ
	double m_Right;
	double m_Left;
	double m_Top;
	double m_Bottom;

	//sin��cos������
	double *m_pSinTable;
	double *m_pCosTable;

private:
	void NormalizeRect(TRECT &rect)
	{
		int tmp;
		if( rect.left > rect.right ) 
		{
			tmp = rect.left;
			rect.left = rect.right;
			rect.right = tmp;
		}

		if( rect.top > rect.bottom )
		{
			tmp = rect.top;
			rect.top = rect.bottom;
			rect.bottom = tmp;
		}
	}

public:
	CDrawRadialT()
	{
		_X0 = 0;_Y0 = 0;
		_fEl = 0;
		_pPointMes = NULL;
		m_PointsSize = 0;

		_det = _detM = -1.0f;
		_kilo = s_kilo  = 0;

		RadarMecX = 0;
		RadarMecY = 0;
		RadarLon = 0;
		RadarLat = 0;

		m_pSinTable = new double[360000];
		m_pCosTable = new double[360000];
		for(int i=0;i<360000;i++)
		{
			m_pSinTable[i]=sin(i*0.001*M_PI/180);
			m_pCosTable[i]=cos(i*0.001*M_PI/180);
		}
	};


	~CDrawRadialT()
	{
		if( _pPointMes != NULL )
		{
			delete []_pPointMes;
		}

		if( m_pSinTable != NULL )
		{
			delete []m_pSinTable;
			m_pSinTable = NULL;
		}
		if( m_pCosTable != NULL )
		{
			delete []m_pCosTable;
			m_pCosTable = NULL;
		}
	}
	//�������ǣ���λ����
	void SetEL(float fEL)
	{
		if ( _fEl != fEL )
		{
			_fEl = fEL;
		}
	}
	bool init(int wid,unsigned int iRangeBin, unsigned int nRangeBin, unsigned int nRadial, bool bKeepScale  )
	{	
		_width = _height = s_wid = s_hei = wid;

		if( !bKeepScale )
		{
			_X0 = _Y0 = s_X0 = s_Y0 =  _width / 2;
		}
		_radCnt = nRadial;
		_kilo = nRangeBin * iRangeBin / 1000.;
		_nRangeBin = nRangeBin;
		_iKuChang = iRangeBin;
		_detOut = s_detOut = 0.5f;

		float fdetKilo = 1.f;
		bool bChangeKilo = false; // �����Ƿ�ı�

		if( s_kilo == 0 ) 
		{
			s_kilo = _kilo;
		}
		else
		{
			//if( (int)_kilo != (int)s_kilo  )
			{
				fdetKilo = _kilo / float(s_kilo);
				s_kilo = _kilo;
				bChangeKilo = true;

				_detM = _detM * fdetKilo;
				_det = _detM/_iKuChang;							// ���̸ı䣬 _iRadial �������ز��䣬���¼�������ϵ��
				//wxLogDebug( wxT("00 %d %f %d "), _kilo, _detM, _iRadius );
			}
		}

		if( !bKeepScale )
		{
			_iRadius = s_iRadius = _width / 2;
			_detM = _kilo * 1000./(float)_iRadius;
			_det = _detM/_iKuChang;
		}
		else
		{
			_iRadius = _kilo * 1000. / _detM;				// ����û�иı䣬����ϵ�����䣬���¼��㾶������
			//wxLogDebug( wxT("12 %d %f %d "),_kilo, _detM, _iRadius );
		}
		{
			int iMaxRadius = s_iRadius * 8;			// ���� Somebody
			_detMMax8 = _kilo * 1000./(float)iMaxRadius;
			_detMax8 = _detMMax8/ _iKuChang;
		}
		if( _pPointMes != NULL && m_PointsSize != _width*_width)
		{
			delete []_pPointMes;
			m_PointsSize = _width * _width;
			_pPointMes = new tagPointMes [ m_PointsSize ];

		}
		else if (_pPointMes==NULL)
		{
			m_PointsSize = _width * _width;
			_pPointMes = new tagPointMes [ m_PointsSize ];
		}
		memset(_pPointMes,0,sizeof(tagPointMes)*m_PointsSize);
		return true;
	}

	bool init2( tagDrawFactor *pFac, tagDrawStyle *pStyle = NULL )
	{
		int wid = (pFac->iWidth > pFac->iHeight)?pFac->iHeight:pFac->iWidth;

		_width = _height = s_wid = s_hei = wid;


		_X0 = s_X0 = pFac->iX;
		_Y0 = s_Y0 = pFac->iY;

		_radCnt = pFac->iRadialCnt;
		_kilo = pFac->iRB * pFac->iRBLen / 1000.;
		_nRangeBin = pFac->iRB;
		_iKuChang = pFac->iRBLen;
		_detOut = s_detOut = 0.5f;
	
		//detM��һ�����ص�����ʵ�ʾ���
		if( pFac->detM < 0.f )
		{
			_iRadius = s_iRadius = _width / 2;
			_detM = (float)_kilo * 1000./(float)_iRadius;
			_det = _detM / _iKuChang;
			_X0 = s_X0 = _iRadius;
			_Y0 = s_Y0 = _iRadius;
		}
		else
		{
			_detM = pFac->detM;
			_det = _detM / _iKuChang;
			_iRadius = (float)_kilo * 1000. / _detM;
		}

		float fdetKilo = 1.f;
		bool bChangeKilo = false; // �����Ƿ�ı�

		if( _pPointMes != NULL )
		{
			delete []_pPointMes;
		}
		_pPointMes = new tagPointMes [ _width * _width ];

		return true;
	}

	bool initMercator( tagDrawFactor *pFac, tagDrawStyle *pStyle = NULL)
	{
		int wid = (pFac->iWidth > pFac->iHeight)?pFac->iHeight:pFac->iWidth;

		_width = _height = s_wid = s_hei = wid;

		XYCoor Merc_Radar = BL2Mercator(RadarLon,RadarLat);
		RadarMecX = Merc_Radar.x;
		RadarMecY = Merc_Radar.y;

		_X0 = s_X0 = pFac->iX;
		_Y0 = s_Y0 = pFac->iY;

		_radCnt = pFac->iRadialCnt;
		//_kilo = pFac->iRB * pFac->iRBLen / 1000.;
		_nRangeBin = pFac->iRB;
		_iKuChang = pFac->iRBLen;
		_detOut = s_detOut = 0.5f;

		int maxDis = ceil(_nRangeBin * _iKuChang * m_pCosTable[(int)(_fEl*1000)]/1000.);//��λ��ǧ��
		CDYW::JWD jwd_top = CDYW::CJWDHelper::GetJWDB( RadarLon, RadarLat,maxDis, 0);//���ͼ�����ϱ߽��γ��
		CDYW::JWD jwd_left = CDYW::CJWDHelper::GetJWDB( RadarLon, RadarLat,maxDis, 270);//���ͼ������߽�ľ���
		Merc_T = BL2Mercator(jwd_top.m_Longitude,jwd_top.m_Latitude);
		Merc_L = BL2Mercator(jwd_left.m_Longitude,jwd_left.m_Latitude);
		CDYW::JWD jwd_bottom = CDYW::CJWDHelper::GetJWDB(RadarLon, RadarLat, maxDis, 180);//���ͼ�����ϱ߽��γ��
		CDYW::JWD jwd_right = CDYW::CJWDHelper::GetJWDB(RadarLon, RadarLat, maxDis, 90);//���ͼ������߽�ľ���
		XYCoor Merc_B = BL2Mercator(jwd_bottom.m_Longitude, jwd_bottom.m_Latitude);
		XYCoor Merc_R = BL2Mercator(jwd_right.m_Longitude, jwd_right.m_Latitude);
		//��ͼ����߽��mercator����
		Merc_Left = Merc_L.x;
		//Merc_Right = Merc_L.x+(RadarMecX-Merc_L.x)*2;
		Merc_Right = Merc_R.x;
		Merc_Top = Merc_T.y;
		//Merc_Bottom = Merc_T.y-(Merc_T.y-RadarMecY)*2;//�ü��㷽������x�������һ�µģ�����y�����ϴ󣬽�������ͼ��ƫ��
		Merc_Bottom = Merc_B.y;

		double maxMerc = 0;
		bool flag = false;
		//��վͼƬ����ʱ���뱣֤�״������ڻ�ͼ��������
		double MercX = ((RadarMecX - Merc_Left) > (Merc_Right- RadarMecX)) ? (RadarMecX - Merc_Left) : (Merc_Right - RadarMecX);
		double MercY = ((Merc_Top - RadarMecY) > (RadarMecY - Merc_Bottom)) ? (Merc_Top - RadarMecY) : (RadarMecY - Merc_Bottom);

		if (MercX < MercY)
		{
			maxMerc = MercY*2.0;
			flag = true;
		}
		else
			maxMerc = MercX*2.0;
		_kilo = maxMerc / 2. / 1000.;
		//detM��һ�����ص�����mercator����
		if( pFac->detM < 0.f )
		{
			_iRadius = s_iRadius = _width / 2;
			_detM = (float)_kilo * 1000./(float)_iRadius;
			_det = _detM / _iKuChang;
			_X0 = s_X0 = _iRadius;
			_Y0 = s_Y0 = _iRadius;
		}
		else
		{
			_detM = pFac->detM;
			_det = _detM / _iKuChang;
			_iRadius = (float)_kilo * 1000. / _detM;
		}

		float fdetKilo = 1.f;
		bool bChangeKilo = false; // �����Ƿ�ı�

		if( _pPointMes != NULL )
		{
			delete []_pPointMes;
		}
		_pPointMes = new tagPointMes [ _width * _width ];

		//m_Left = jwd_left.m_Longitude;
//		m_Top = jwd_top.m_Latitude;
// 		m_Right = jwd_right.m_Longitude;
// 		m_Bottom = jwd_bottom.m_Latitude;
		//��ʱ�������ϰ������� ��������(Merc_Top - RadarMecY) < (RadarMecY - Merc_Bottom)�����
		if (flag == true)//˵��������״����ģ�Y���ϰ���mercator�������,���״����ľ໭���ĸ��ߵ�mercator�����Ϊ��Merc_Top - RadarMecY��
		{
			m_Top = jwd_top.m_Latitude;
			//��ȡ�����ױ߶�Ӧ��γ��
			XYCoor LBofDraw = Mercator2BL(RadarMecX, (Merc_Bottom-((Merc_Top - RadarMecY)-(RadarMecY - Merc_Bottom))));
			//XYCoor LBofDC = Mercator2BL(Merc_Left, Merc_Bottom);
			double JWDdis = abs(LBofDraw.y - jwd_bottom.m_Latitude);
			m_Bottom = jwd_bottom.m_Latitude - JWDdis;

			//��ȡ�������ұ߶�Ӧ�ľ���
			LBofDraw = Mercator2BL((Merc_Left - ((Merc_Top - RadarMecY) - (RadarMecX - Merc_Left))), RadarMecX);
			JWDdis = abs(LBofDraw.x - jwd_left.m_Longitude);
			m_Left = jwd_left.m_Longitude - JWDdis;
			m_Right = m_Left + (RadarLon - m_Left)*2;//�����ϼ���ǶԳƵ�
		}
		else
		{
			m_Left = jwd_left.m_Longitude;
			m_Right = jwd_right.m_Longitude;

			//��ȡ�������±߶�Ӧ��γ��
			XYCoor LBofDraw = Mercator2BL(RadarMecX, (Merc_Top - ((RadarMecX - Merc_Left) - (Merc_Top - RadarMecY))));
			//XYCoor LBofDC = Mercator2BL(Merc_Right, Merc_Top);
			double JWDdis = abs(LBofDraw.y - jwd_top.m_Latitude);
			m_Top = jwd_left.m_Latitude + JWDdis;
			LBofDraw = Mercator2BL(RadarMecX, (Merc_Bottom - ((RadarMecX - Merc_Left) - (RadarMecY - Merc_Bottom))));
			JWDdis = abs(LBofDraw.y - jwd_bottom.m_Latitude);
			m_Bottom = jwd_bottom.m_Latitude - JWDdis;
		}
		
		if (pStyle->clrTable.bDraw || pStyle->info.bDraw)
		{
			XYCoor temp = Mercator2BL(Merc_Right + (pFac->iWidth - pFac->iHeight)*_detM, Merc_Bottom);
			m_Right = temp.x;
		}

		return true;
	}

	bool initJWD( tagDrawFactor *pFac, tagDrawStyle *pStyle  = NULL )
	{
		int wid = (pFac->iWidth > pFac->iHeight)?pFac->iHeight:pFac->iWidth;

		_width = _height = s_wid = s_hei = wid;

		_X0 = s_X0 = pFac->iX;
		_Y0 = s_Y0 = pFac->iY;

		_radCnt = pFac->iRadialCnt;
		//_kilo = pFac->iRB * pFac->iRBLen / 1000.;
		_nRangeBin = pFac->iRB;
		_iKuChang = pFac->iRBLen;
		_detOut = s_detOut = 0.5f;

		int maxDis = ceil(_nRangeBin * _iKuChang * m_pCosTable[(int)(_fEl*1000)]/1000.);//��λ����
		CDYW::JWD jwd_top = CDYW::CJWDHelper::GetJWDB( RadarLon, RadarLat,maxDis, 0);//���ͼ�����ϱ߽��γ��
		CDYW::JWD jwd_left = CDYW::CJWDHelper::GetJWDB( RadarLon, RadarLat,maxDis, 270);//���ͼ������߽�ľ���

		//��ͼ����߽羭γ������
		m_Left = jwd_left.m_Longitude;
		m_Right = m_Left+(RadarLon-m_Left)*2;
		m_Top = jwd_top.m_Latitude;
		m_Bottom = m_Top-(m_Top-RadarLat)*2;

		double maxMerc = 0;
		if (RadarLon-m_Right > m_Top-RadarLat)
		{
			maxMerc = m_Right - m_Left;
		}
		else
			maxMerc = m_Top - m_Bottom;
		_kilo = maxMerc / 2.;//�����ͼ����ľ�γ��ֵ

		//detM��һ�����ص����ľ�γ��ֵ
		if( pFac->detM < 0.f )
		{
			_iRadius = s_iRadius = _width / 2;
			_detM = (float)_kilo/(float)_iRadius;
			_det = _detM / _iKuChang;
			_X0 = s_X0 = _iRadius;
			_Y0 = s_Y0 = _iRadius;
		}
		else
		{
			_detM = pFac->detM;
			_det = _detM / _iKuChang;
			_iRadius = (float)_kilo / _detM;
		}

		float fdetKilo = 1.f;
		bool bChangeKilo = false; // �����Ƿ�ı�

		if( _pPointMes != NULL )
		{
			delete []_pPointMes;
		}
		_pPointMes = new tagPointMes [ _width * _width ];

		if (pStyle->clrTable.bDraw || pStyle->info.bDraw)
		{
			m_Right = m_Right + (pFac->iWidth - pFac->iHeight)*_detM;
		}

		return true;
	}

	bool SetCenter( int x, int y , int vWid, int vHei )					// ��ָ������Ϊ��ͼ����
	{
		int dx = vWid*0.5 - x;
		int dy = vHei*0.5 - y;
		_X0 += dx;
		_Y0 += dy;
		return true;
	}

	bool panTo( int sX, int sY, int dX, int dY )			// �ƶ�ͼ
	{
		// ReCalc the new parame
		int dx = dX - sX;
		int dy = dY - sY;

		_X0 += dx;
		_Y0 += dy;

		//wxLogDebug( wxString::Format(wxT("pan: %d, %d"), _X0, _Y0));
		return true;
	}

	//! �Ŵ���
	/*!
		����ָ�������Ŵ�ز�ͼ
	*/
	bool ZoomScale(int scale,int x, int y,  int offsetX, int offsetY)
	{	
		int x1 = x + offsetX;
		int y1 = y + offsetY;
		
		float kuDsX = _det * ( x1 - _X0 );  // ѡ��ĵ���뼫���X����������
		float kuDsY = _det * ( y1 - _Y0 );  // ѡ��ĵ���뼫���Y����������
		
		//////////////////////////////////////////////////////////////////////////
		_detOut = s_detOut;
		_iRadius = s_iRadius * scale;

		_detM = _kilo * 1000./(float)_iRadius;
		_det = _detM / _iKuChang;

		//////////////////////////////////////////////////////////////////////////
		_X0 = -( kuDsX / _det - x1 );
		_Y0 = -( kuDsY / _det - y1 );

		//wxLogDebug( wxString::Format(wxT("Scale: %d, %d"), _X0, _Y0));
		return true;
	}

	//! �Ŵ���
	/*! ��ȱʡ��С�Ŵ�ز�ͼ
		\param x ����X����
		\param y ����Y����
		\param vWid ��ͼ����������
		\param vHei ��ͼ��������߶�
		\param offseX ��ͼXƫ��
		\param offsetY ��ͼYƫ��
	*/
	bool ZoomIn( int x, int y, int vWid, int vHei, int offsetX=0, int offsetY=0 )
	{
		if(int(_det * 10) <= 1)
			return false;

		int tmpX = x;	// ��¼��ԭʼλ��
		int tmpY = y;

		if( vWid <= 0 ) vWid = _width;
		if( vHei <= 0 ) vHei = _height;

		float fDet = _det * _detOut;

		if( fDet >= _detMax8 ) 
		{
			_det = fDet;

			_iRadius = _iRadius / _detOut;
			_detM = _kilo * 1000./(float)_iRadius;

			_X0 = vWid * 0.5 + (_X0 - x) / _detOut - offsetX;
			_Y0 = vHei * 0.5 + (_Y0 - y) / _detOut - offsetY;

			// �ƶ�
			int dx = tmpX - vWid * 0.5;
			int dy = tmpY - vHei * 0.5;

			_X0 += dx;
			_Y0 += dy;
			//wxLogDebug( wxString::Format(wxT("In: %d, %d"), _X0, _Y0));
		}

		return true;
	}


	//! ��С����
	/*!
		int vWid ����������
		int vHei ��������߶�
	*/
	bool ZoomOut( int x, int y, int vWid, int vHei, int offsetX=0, int offsetY=0 )
	{
		_down.x = _down.y = 0;
		_last.x = _last.y = 0;
		
		if(int(_det * 10) >= 200)
			return 0;
		
		int tmpX = x;
		int tmpY = y;


		if( vWid <= 0 ) vWid = _width;
		if( vHei <= 0 ) vHei = _height;

		float fDet = _det/_detOut;

		//if(fDet > 20)
		//	_det = 20;
		//else
			_det = fDet;
		_iRadius = _iRadius * _detOut;
		_detM = _kilo* 1000./(float)_iRadius;

		_X0 = vWid * 0.5 + (_X0 - x) * _detOut + offsetX * _detOut;
		_Y0 = vHei * 0.5 + (_Y0 - y) * _detOut + offsetY * _detOut;

		// �ƶ�
		int dx = tmpX - vWid * 0.5;
		int dy = tmpY - vHei * 0.5;

		_X0 = _X0 + dx;
		_Y0 = _Y0 + dy;

		//wxLogDebug( wxString::Format(wxT("Out: %d, %d"), _X0, _Y0));

		return true;
	}

	bool ZoomDef( )
	{
		if( (_iRadius == s_iRadius) && ( _X0 == s_X0 ) && ( _Y0 == s_Y0 )) 
		{
			return false;
		}
		_width = s_wid;
		_height = s_hei;

		_detOut = s_detOut;
		_iRadius = s_iRadius;
		_X0 = s_X0;
		_Y0 = s_Y0;

		_detM = _kilo * 1000./(float)_iRadius;
		_det = _detM / _iKuChang;
		return true;
	}

	// ������
	//CacuSinCos()���ݵ�ǰ���ã������µĿռ�ֱ������ͼ������ӳ�����

	int CacuSinCos()
	{
		int x,y,r;
		int TotalPointNum=0;
		// �Ѹ������ص�ת��Ϊ������
		double f180PI = 180.f/M_PI;
		float fAzi = 0.0f;
		double x_X0 = 0.,y_Y0 = 0.,xx=0.,yy=0.;
		int seta = 0;
		//float radCnt = _radCnt;
		float rad360 = _radCnt /360.0f;

		for(x=0; x < _width; x++)
		{
			x_X0 = x - _X0;
			xx = x_X0 * x_X0;

			for(y=0; y < _height; y++) 
			{
				y_Y0 = _Y0 - y;
				yy = y_Y0 * y_Y0;

				fAzi = atan2( x_X0, y_Y0 ) * f180PI;

				if( fAzi < 0 ) fAzi += 360.0f;
				seta = fAzi * rad360;

				//r = sqrt(xx+yy);
				r = CDYW::MATH::SquareRootFloat( xx+yy );
				r = r / cos(_fEl*M_PI/180.0);
				if( r < _iRadius) 
				{
					_pPointMes[ TotalPointNum ].x = x;
					_pPointMes[ TotalPointNum ].y = y;
					_pPointMes[ TotalPointNum ].iRad = seta;
					_pPointMes[ TotalPointNum ].iRB = int( r * _det );
					TotalPointNum++;
				}
			}
		}

		return TotalPointNum;
	}

	int CacuSinCosofMercator()//mercator
	{
		// �Ѹ������ص�ת��Ϊ������
		int x,y,r;
		int TotalPointNum=0;// ͳ����Ч�ز����ݶ�Ӧ�����ص���
		double x_X0 = 0.,y_Y0 = 0.;//��ǰ���ص��mercator����ֵ
		int seta = 0;
		const float& rad360 = _radCnt/360.0f;//һ���������Ķ���
		// �״����Ķ�Ӧ��γ�ȵ�������
		const int& radLat1 = RadarLat *1000;
		const double& radLat1_sin = m_pSinTable[radLat1];
		const double& radLat1_cos = m_pCosTable[radLat1];
		//��ǰ���ص��Ӧ��γ�ȼ���������
		int curLat2;
		double curLat2_sin;
		double curLat2_cos;
		// ��ǰ���ص����״����ĵľ��Ȳ��������
		int b;
		double b_sin;
		double b_cos;

		XYCoor CurPosBL;
		double s = 0.0f;
		double fAz = 0.0f;

		double temp = m_pCosTable[(int)(_fEl * 1000)] * _iKuChang;

// 		int minW=0;
// 		int maxW=0;
// 		int minH=0;
// 		int maxH=0;

		//����mercator�ں����ϴ��ڶԳƹ�ϵ��ֻ�����״��������ص�һ��
		//�����״ﷶΧ�����أ�
		int minW = _X0-(RadarMecX - Merc_L.x)/_detM;
		int maxW = _X0+(RadarMecX - Merc_L.x)/_detM;

		int minH = _Y0-(Merc_T.y - RadarMecY)/_detM;
		int maxH = _Y0+(Merc_T.y - RadarMecY)/_detM;

		//��֤������Χ�ڻ�����Χ��
		if (minH < 0)
		{
			minH = 0;
		}
		if (maxH > _height)
		{
			maxH = _height;
		}
		//�����״���������ڻ������ģ�ѡ�������Χ
		if (_X0<_width/2)
		{
			minW = _X0;
		}
		else
		{
			maxW = _X0;
		}
		//��֤������Χ�ڻ�����Χ��
		if (minW < 0)
		{
			minW = 0;
		}
		if (maxW >= _width)
		{
			maxW = _width-1;
		}
		int symx=0;//symxΪ����ʱx������״����ĵĶԳƵ�
		//����=maxW�ᵼ���״���������ȱһ��������,���ǵ��ڻ��ڷŴ��ʱ��������(�������Ϊ�����Ҳ�ʱ������˱߽�_width��һ��)
		for(x=minW; x <=maxW; x++)
		{
			x_X0 = RadarMecX + (x - _X0)*_detM;
			symx = _X0*2-x;

			for(y=minH; y < maxH; y++) 
			{
				y_Y0 = RadarMecY + (_Y0 - y)*_detM;

				//CurPosBL = Mercator2BL(x_X0,y_Y0);//��mercator����ת��Ϊ��γ��
// 				CurPosBL.x = x_X0/20037508.34*180;
// 				CurPosBL.y= 180/M_PI*(2*atan(exp(y_Y0/20037508.34*M_PI))-M_PI/2);
				CurPosBL = Mercator2BL(x_X0, y_Y0);
				

				//����Google�������ķ���
				//r = CDYW::CJWDHelper::getDistance(RadarLon,RadarLat,CurPosBL.x,CurPosBL.y, fAzi);
				curLat2 = CurPosBL.y*1000;   
				if (curLat2<0)
				{
					curLat2 += 360000;
				}
				//curLat2 =(int)((CurPosBL.y+360)*1000)%360000;  
				curLat2_sin = m_pSinTable[curLat2];
				curLat2_cos = m_pCosTable[curLat2];

				b =(CurPosBL.x-RadarLon)*1000 ;
				if (b<0)
				{
					b+=360000;
				}
				//b =(int)((CurPosBL.x-RadarLon+360)*1000) %360000; 

				b_sin = m_pSinTable[b];
				b_cos = m_pCosTable[b];
				s = acos(curLat2_sin*radLat1_sin + radLat1_cos*curLat2_cos*b_cos)* 6378137.;//����γ��֮����������

				//��������������Ϊˮƽ���룬��ͶӰ�������ϣ���ö�Ӧ�ľ������
				r = s / temp;
				if( r < _nRangeBin) 
				{
					fAz = atan2(b_sin*curLat2_cos, radLat1_cos*curLat2_sin-radLat1_sin*curLat2_cos*b_cos);//��ǰ��������״����ĵķ�λ
					if (fAz < 0)
					{
						fAz += M_PI*2;
					}
					seta = fAz*180./M_PI* rad360;//��λ�Ƕ�Ӧ�ľ����
					_pPointMes[ TotalPointNum ].x = x;
					_pPointMes[ TotalPointNum ].y = y;
					_pPointMes[ TotalPointNum ].iRad = seta;
					//_pPointMes[ TotalPointNum ].iRB = int( r * _det );
					_pPointMes[ TotalPointNum ].iRB =  r;
					TotalPointNum++;
					//�Գƹ�ϵ�����Ӧ�״���Ϣ
					if (symx!=x && symx>=0 && symx<_width )
					{
						_pPointMes[ TotalPointNum ].x = symx;
						_pPointMes[ TotalPointNum ].y = y;
						//�״ﾶ�����0~359��������360-seta�Ļ������״���������ڻ������Ŀ����ʱ���ϰ벿�ֶ������ֵ���°벿��ȱ�پ���ֵ
						_pPointMes[ TotalPointNum ].iRad = (_radCnt-1 - seta);
						_pPointMes[ TotalPointNum ].iRB = r;
						TotalPointNum++;
					}

				}
			}
		}
		return TotalPointNum;
	}

	int CacuSinCosofJWD()//�Ⱦ�γ��
	{
		// �Ѹ������ص�ת��Ϊ������
		int x,y,r;
		int TotalPointNum=0;// ͳ����Ч�ز����ݶ�Ӧ�����ص���
		double x_X0 = 0.,y_Y0 = 0.;//��ǰ���ص��mercator����ֵ
		int seta = 0;
		const float& rad360 = _radCnt/360.0f;//һ���������Ķ���
		// �״����Ķ�Ӧ��γ�ȵ�������
		const int& radLat1 = RadarLat *1000;//RadarLat�״�γ�ȣ���λ��
		const double& radLat1_sin = m_pSinTable[radLat1];
		const double& radLat1_cos = m_pCosTable[radLat1];
		//��ǰ���ص��Ӧ��γ�ȼ���������
		double curLat2_sin;
		double curLat2_cos;
		// ��ǰ���ص����״����ĵľ��Ȳ��������
		int b;
		double b_sin;
		double b_cos;

		double s = 0.0f;
		double fAz = 0.0f;

		for(x=0; x < _width; x++)
		{
			x_X0 = RadarLon + (x - _X0)*_detM;
			for(y=0; y < _height; y++) 
			{
				y_Y0 = RadarLat + (_Y0 - y)*_detM;//��x_X0��y_Y0���ǵ�ǰ���ص��Ӧ�ľ�γ�ȣ���λ��

				curLat2_sin = m_pSinTable[(int)(y_Y0*1000)];
				curLat2_cos = m_pCosTable[(int)(y_Y0*1000)];

				b =(x_X0-RadarLon)*1000 ;
				if (b<0)
				{
					b+=360000;
				}
				b_sin = m_pSinTable[b];
				b_cos = m_pCosTable[b];
				s = acos(curLat2_sin*radLat1_sin + radLat1_cos*curLat2_cos*b_cos)* 6378137.;//����γ��֮����������

				//��������������Ϊˮƽ���룬��ͶӰ�������ϣ���ö�Ӧ�ľ������
				r = s / m_pCosTable[(int)(_fEl*1000)] / _iKuChang;
				if( r < _nRangeBin) 
				{
					fAz = atan2(b_sin*curLat2_cos, radLat1_cos*curLat2_sin-radLat1_sin*curLat2_cos*b_cos);//��ǰ��������״����ĵķ�λ
					if (fAz < 0)
					{
						fAz += M_PI*2;
					}
					seta = fAz*180./M_PI* rad360;//��λ�Ƕ�Ӧ�ľ����
					_pPointMes[ TotalPointNum ].x = x;
					_pPointMes[ TotalPointNum ].y = y;
					_pPointMes[ TotalPointNum ].iRad = seta;
					//_pPointMes[ TotalPointNum ].iRB = int( r * _det );
					_pPointMes[ TotalPointNum ].iRB =  r;
					TotalPointNum++;

				}
			}
		}
		return TotalPointNum;
	}

	int InitBuffVariant(int wid )					/*��ʼ�� ����ͼ��Ҫ�õĲ���  */
	{

	}

	

	void TLButtonDown( int x, int y, int Offx = 0, int Offy = 0 )				/* ��궯�� */
	{
		_down.x = x; _down.y = y;
		_last.x = x; _last.y = y;
		_zoom_in_first.x = x + Offx;        //���������������Ͻ�����
		_zoom_in_first.y = y + Offy;        //���������������Ͻ�����
	}

	bool TLButtonUp( int x, int y, int Offx = 0, int Offy = 0)
	{
		_zoom_in_last.x = x + Offx;
		_zoom_in_last.y = y + Offy; 

		_down.x = _down.y = 0;
		_last.x = _last.y = 0;

		// ReCalc the new parame
		if( ( _zoom_in_first.x != _zoom_in_last.x ) ||
			( _zoom_in_first.y != _zoom_in_last.y )
			)
		{
			panTo( _zoom_in_first.x, _zoom_in_first.y,
				_zoom_in_last.x, _zoom_in_last.y);
			
			return true;
		}

		return false;
	}

	void TRButtonDown( int x, int y )
	{

	}

	/*  */
	void TMouseMove( int x, int y )
	{

	}

	void OnDrawCircle( CDrawRadialT *pDPara, HDC hdc, int RBOfRad, COLORREF clrCir, int kiloInv, int degInv)
	{
		unsigned int i = 0, j = 0;
		int maxKilo = RBOfRad;
		int iRefBinLen = pDPara->_iKuChang;
		int X0 = pDPara->_X0;
		int Y0 = pDPara->_Y0;
		float det = pDPara->_detM/iRefBinLen;
		HPEN penCir, penOld;
		penCir = CreatePen( PS_SOLID, 1, clrCir );

		LOGBRUSH nullBrush;
		nullBrush.lbStyle = BS_NULL;
		HBRUSH bruNULL, bruOld;
		bruNULL = ::CreateBrushIndirect( &nullBrush );

		bruOld = (HBRUSH)::SelectObject( hdc, bruNULL );
		penOld = (HPEN)::SelectObject( hdc, penCir );

		int r =0;
		float Xr = 0.0f, Yr = 0.0f;

		unsigned int iMaxKilo = maxKilo * iRefBinLen/1000.f; 
		int iKilo = kiloInv;
		float RangeBin = 1000.f/iRefBinLen;
		//��25�������Ȧ
		float  Kil25= iKilo*0.5;
		r = (int)(Kil25* RangeBin /det);
		::Ellipse( hdc, X0 - r, Y0 - r,X0 + r,Y0 + r );

		for(j = iKilo; j<iMaxKilo; j+=iKilo)
		{
			float fKilo = j ;
			r = (int)(fKilo * RangeBin /det);
			::Ellipse( hdc, X0 - r, Y0 - r,X0 + r,Y0 + r );
		}

		r = (int)(maxKilo / det);
		::Ellipse( hdc, X0 - r, Y0 - r,X0 + r, Y0 + r );

		r = maxKilo/det;//(int)(100 *8.0f/_det);
		int iSeta = degInv;

		for(j=0; j<360; j+=iSeta)
		{
			Xr = r*sin(j * M_PI / 180.f);
			Yr = r*cos(j * M_PI / 180.f);
			Xr = X0 + Xr;
			Yr = Y0 + Yr;
			::MoveToEx( hdc, X0, Y0, NULL );
			::LineTo( hdc, Xr, Yr);
		}
		///// �ͷ�GDI��Դ
		::SelectObject( hdc, penOld );
		::SelectObject( hdc, bruOld );

		::DeleteObject( penCir );
		::DeleteObject( bruNULL );
	}

	void DrawCircle( CDrawRadialT *pDPara, int RBOfRad, tagDrawCanvas *pCanvas,tagDrawStyleCircle *pCircle)
	{
		unsigned int i = 0, j = 0;
		int maxKilo = RBOfRad;
		int iRefBinLen = pDPara->_iKuChang;
		int X0 = pDPara->_X0;
		int Y0 = pDPara->_Y0;
		float det = pDPara->_det;
	
		HDC hdc = (HDC)pCanvas->pCanvas;
		int iOffX = pCanvas->iOffX;
		int iOffY = pCanvas->iOffY;

		HPEN penCir, penOld;
		penCir = CreatePen( PS_SOLID, 1, pCircle->clr );

		HBRUSH bruOld = (HBRUSH)::SelectObject( hdc, ::GetStockObject( NULL_BRUSH) );
		penOld = (HPEN)::SelectObject( hdc, penCir );

		int r =0;
		float Xr = 0.0f, Yr = 0.0f;

		unsigned int iMaxKilo = maxKilo * iRefBinLen/1000.f; 
		int iKilo = pCircle->iDisInv;
		float RangeBin = 1000.f/iRefBinLen;

		for(j = iKilo; j<iMaxKilo; j+=iKilo)
		{
			pthread_testcancel();
			float fKilo = j ;
			r = (int)(fKilo * RangeBin /det);
			::Ellipse( hdc, X0 - r + iOffX, Y0 - r+iOffY, X0 + r + iOffX, Y0 + r + iOffY );
		}

		r = (int)(maxKilo /det);
		::Ellipse( hdc, X0 - r + iOffX, Y0 - r+iOffY, X0 + r + iOffX, Y0 + r+iOffY );

		r = maxKilo/det;//(int)(100 *8.0f/_det);
		int iSeta = pCircle->iDegInv;

		for(j=0; j<360; j+=iSeta)
		{
			pthread_testcancel();
			Xr = r*sin(j * M_PI / 180.f);
			Yr = r*cos(j * M_PI / 180.f);
			Xr = X0 + Xr;
			Yr = Y0 + Yr;
			::MoveToEx( hdc, X0 + iOffX, Y0 + iOffY, NULL );
			::LineTo( hdc, Xr +i + iOffX, Yr + iOffY );
		}

		bool m_bMain = false;
		if ( r> 50)
			m_bMain = true;

		//// ������

		if( pCircle->bCircleRuler )
		{
			float Xr1 = 0.0f, Yr1 = 0.0f;
			int Xr2 =0,Yr2 = 0;

			//// ��������ĸ߶ȺͿ��
			int fWidth = 10;
			int fHeight = 120;
			if ( r>100 && r<400 )
			{
				fWidth = 5;
				fHeight = 90;
			}

			RECT rectFont;
			float fTemp = M_PI / 180.f;
			COLORREF clrOld = ::SetTextColor( hdc, RGB( 70,70,70 ) );

			HFONT fontNew = ::CreatePointFont( fHeight, "����" );
			HGDIOBJ fontOld =  SelectObject( hdc, fontNew );

			for(j=0; j<360; j+=2)
			{
				pthread_testcancel();
				float fP = j*fTemp;
				float sinFp = sin(fP);
				float cosFp = cos(fP);
				////
				Xr = r*sinFp;
				Yr = r*cosFp;
				Xr = X0 + Xr;
				Yr = Y0 + Yr;

				if ( m_bMain )//// >50
				{
					if ( j % 10 == 0 ){
						if ( r>199 ){
							Xr1 = (r-15)*sinFp;
							Yr1 = (r-15)*cosFp;

							Xr2 = (r-30) * sinFp;
							Yr2 = (r-30) * cosFp;

							int iVal = 360-j + 180;
							iVal = iVal > 360? iVal-360:iVal;
							char szOut[30] = {0};
							sprintf( szOut,"%d", iVal );

							if ( r>200 ){

								//// д������
								::DrawTextA( hdc, szOut, strlen(szOut), &rectFont, DT_CALCRECT);
								::TextOutA( hdc,
									X0 + Xr2-(rectFont.right-rectFont.left)/2 + iOffY,
									Y0 + Yr2-(rectFont.bottom-rectFont.top)/2 + iOffY,
									szOut,
									strlen(szOut)
									);
							} 
							else{

								//// дС����
								::DrawTextA( hdc, szOut, strlen(szOut), &rectFont, DT_CALCRECT);
								::TextOutA( hdc,
									X0 + Xr2-(rectFont.right-rectFont.left)/2 + iOffX,
									Y0 + Yr2-(rectFont.bottom-rectFont.top)/2 + iOffY,
									szOut,
									strlen(szOut)
									);

							}
						}else{
							Xr1 = (r-4)*sinFp;
							Yr1 = (r-4)*cosFp;
						}

						Xr1 = X0 + Xr1;
						Yr1 = Y0 + Yr1;
					}else{
							if ( r>199 ){
							Xr1 = (r-8)*sinFp;
							Yr1 = (r-8)*cosFp;
							Xr1 = X0 + Xr1;
							Yr1 = Y0 + Yr1;
							} 
							else{
							Xr1 = Xr;
							Yr1 = Yr;
							}
					}
				}else{
					Xr1 = Xr;
					Yr1 = Yr;
				}

				::MoveToEx( hdc, Xr1 + iOffX, Yr1 + iOffY, NULL );
				::LineTo( hdc, Xr + iOffX, Yr + iOffY );
			}

			::SetTextColor( hdc, clrOld );
			::SelectObject( hdc, fontOld );
			::DeleteObject( fontNew );
		}

		///// �ͷ�GDI��Դ
		::SelectObject( hdc, penOld );
		::SelectObject( hdc, bruOld );

		::DeleteObject( penCir );
	}

	XYCoor BL2Mercator( double Lon, double Lat)//��γ��(��)תΪmercator����
	{
		XYCoor Mec;
		Mec.x = 0 + 6378137.0*(Lon*M_PI/180.0-0);
		Mec.y =  0 + 6378137.0*log(tan(M_PI/4+Lat/2*M_PI/180.0));
		return Mec;
	}
	inline XYCoor Mercator2BL( double x, double y)//mercator����תΪ��γ��(��)
	{
		XYCoor pos_XY;
		pos_XY.x = x/20037508.34*180;
		pos_XY.y = y/20037508.34*180;
		pos_XY.y= 180/M_PI*(2*atan(exp(pos_XY.y*M_PI/180))-M_PI/2);
		return pos_XY;
	}
	
};
}; //end namespace RadarSystem
}; // end namespace CDYW

#endif
