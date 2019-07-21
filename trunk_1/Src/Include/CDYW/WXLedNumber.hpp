/*! \file wxLedNumber.hpp
	\brief �������ʾ�ؼ������ؼ��� wxStaticText ������ ������ FormBuilder��ʹ�� wxStaticText �ؼ����в���

	\version 1.0
	\author л�����涨��
	\date 2010-1-15 15:47
	\b �޸ļ�¼��
	\arg \c 2010/01/15, л���� ����
	\arg \c 2010/07/07, ת��Ϊ�ؼ�
	\bug 
	
	
	Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
All rights reserved.

	\ingroup

*/


#ifndef HEADER_Number_2010_7_8
#define HEADER_Number_2010_7_8


#include <wx/dc.h>
#include <wx/string.h>

#define	Y_MARGIN	2				//��ͼ������

namespace CDYW
{



/*! 

*/
class wxLedNumber:public wxStaticText
{
	float m_fVal; // ���Ƶ�ֵ
	int m_iSign; // С���������λ��
	int m_iWidInter;	// �������ֵĿ��
	wxColour m_clr;	// ������ɫ
	wxString m_Uint;		//��λ
	bool m_bDrawUint;

private:

	static void DrawSection1(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[4];
		point[0].x=nLeft+(int)(0.1*Width);
		point[0].y=Y_MARGIN;

		point[1].x=nLeft+(int)(0.9*Width);
		point[1].y=Y_MARGIN;

		point[2].x=nLeft+(int)(0.7*Width);
		point[2].y=(int)(0.2*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.3*Width);
		point[3].y=(int)(0.2*Width)+Y_MARGIN;

		dc.DrawPolygon(4, point);		
	}

	static void DrawSection2(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[5];
		point[0].x=nLeft+Width;
		point[0].y=(int)(0.1*Width)+Y_MARGIN;

		point[1].x=nLeft+Width;
		point[1].y=(int)(0.8*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.9*Width);
		point[2].y=(int)(0.9*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.8*Width);
		point[3].y=(int)(0.8*Width)+Y_MARGIN;

		point[4].x=nLeft+(int)(0.8*Width);
		point[4].y=(int)(0.3*Width)+Y_MARGIN;


		dc.DrawPolygon(5, point);
	}
	

	static void DrawSection3(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[5];
		point[0].x=nLeft+Width;
		point[0].y=(int)(1.1*Width)+Y_MARGIN;

		point[1].x=nLeft+Width;
		point[1].y=(int)(1.8*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.8*Width);
		point[2].y=(int)(1.6*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.8*Width);
		point[3].y=(int)(1.1*Width)+Y_MARGIN;

		point[4].x=nLeft+(int)(0.9*Width);
		point[4].y=Width+Y_MARGIN;

		dc.DrawPolygon(5, point);
	}


	static void DrawSection4(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[4];
		point[0].x=nLeft+(int)(0.1*Width);
		point[0].y=(int)(1.9*Width)+Y_MARGIN;

		point[1].x=nLeft+(int)(0.9*Width);
		point[1].y=(int)(1.9*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.7*Width);
		point[2].y=(int)(1.7*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.3*Width);
		point[3].y=(int)(1.7*Width)+Y_MARGIN;

		dc.DrawPolygon(4, point);
	};

	static void DrawSection5(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[5];
		point[0].x=nLeft;
		point[0].y=(int)(1.1*Width)+Y_MARGIN;

		point[1].x=nLeft;
		point[1].y=(int)(1.8*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.2*Width);
		point[2].y=(int)(1.6*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.2*Width);
		point[3].y=(int)(1.1*Width)+Y_MARGIN;

		point[4].x=nLeft+(int)(0.1*Width);
		point[4].y=Width+Y_MARGIN;

		dc.DrawPolygon(5, point);
	};


	static void DrawSection6(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[5];
		point[0].x=nLeft;
		point[0].y=(int)(0.1*Width)+Y_MARGIN;

		point[1].x=nLeft;
		point[1].y=(int)(0.8*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.1*Width);
		point[2].y=(int)(0.9*Width)+Y_MARGIN;

		point[3].x=nLeft+(int)(0.2*Width);
		point[3].y=(int)(0.8*Width)+Y_MARGIN;

		point[4].x=nLeft+(int)(0.2*Width);
		point[4].y=(int)(0.3*Width)+Y_MARGIN;

		dc.DrawPolygon(5, point);
	};


	static void DrawSection7(wxDC &dc,int nLeft,int Width)
	{
		wxPoint point[6];
		point[0].x=nLeft+(int)(0.3*Width);
		point[0].y=(int)(0.9*Width)+Y_MARGIN;

		point[1].x=nLeft+(int)(0.7*Width);
		point[1].y=(int)(0.9*Width)+Y_MARGIN;

		point[2].x=nLeft+(int)(0.8*Width);
		point[2].y=Width+Y_MARGIN;

		point[3].x=nLeft+(int)(0.7*Width);
		point[3].y=(int)(1.1*Width)+Y_MARGIN;

		point[4].x=nLeft+(int)(0.3*Width);
		point[4].y=(int)(1.1*Width)+Y_MARGIN;

		point[5].x=nLeft+(int)(0.2*Width);
		point[5].y=Width+Y_MARGIN;
		dc.DrawPolygon(5, point);
	}

	static void DrawSingleNumber(wxDC &dc,int nNum,int nLeft,int Width)
	{
		switch (nNum)
		{
		case 0:
			DrawSection1(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection5(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			break;
		case 1:		
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);		
			break;
		case 2:
			DrawSection1(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection5(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 3:
			DrawSection1(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 4:		
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 5:
			DrawSection1(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 6:
			DrawSection1(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection5(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 7:
			DrawSection1(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);		
			break;
		case 8:
			DrawSection1(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection5(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		case 9:
			DrawSection1(dc,nLeft,Width);
			DrawSection3(dc,nLeft,Width);
			DrawSection4(dc,nLeft,Width);
			DrawSection2(dc,nLeft,Width);
			DrawSection6(dc,nLeft,Width);
			DrawSection7(dc,nLeft,Width);
			break;
		default:
			;
		}
	}

public:

	/*! \fn DrawFloatData
		
		\brief 
		������������ܣ�����һλС��
		
		\public 

		\param wxDC & dc		dc
		\param float Data		��������
		\param int nLeft		������
		\param int Width		����ܿ��
		
		\return void
	*/
	static void DrawFloatData(wxDC &dc,float Data, int sign,int nLeft,int Width)
	{
		wxString strFormat = wxString::Format( wxT("%%.%df"), sign);

		wxString strVal = wxString::Format( 
			strFormat,
			Data
			);

		DrawStringData( dc, strVal.mb_str(wxConvLocal), nLeft, Width );
	}

	static void DrawStringData(wxDC &dc, const char *szNum,int nLeft,int Width)
	{
		int Space = .5f * Width;

		string strNum( szNum );

		size_t i = 0, size = strNum.size();

		int left = nLeft;

		for( i=0; i<size; i++ )
		{
			left = nLeft + i*( Width + Space );

			string szSNum( strNum.begin() + i, strNum.begin() + i + 1);

			if( szSNum == "-" ) {
				DrawSection7( dc, left, Width );
				continue;
			}

			if( szSNum == "." ) {
				dc.DrawCircle( left+Width/2, Width*2, 2 );
				continue;
			}

			if( szSNum == " " ) {
				continue;
			}

			unsigned long val = 0;
			wxString wxStr( szSNum.c_str(), wxConvLocal );

			if( wxStr.ToULong( &val ) )
				if( val >=0 && val <= 9 )
				{
					DrawSingleNumber( dc, val,left, Width );
				}
		}
	};

	~wxLedNumber(void)
	{
		this->Disconnect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( wxLedNumber::OnEraseEvent ), NULL, this );
		this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( wxLedNumber::OnPaint ), NULL, this );

	};

	wxLedNumber(wxWindow *parent,
		wxWindowID id,
		const wxString& label,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxStaticTextNameStr)
		:wxStaticText( parent, id, label, pos, size, style, name )
	{
		m_fVal=0.f;
		m_iSign = 2;
		m_iWidInter = 3;

		m_clr = wxColour( 155,225,155);
		m_Uint = wxEmptyString;
		m_bDrawUint = false;
		this->Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( wxLedNumber::OnEraseEvent ), NULL, this );
		this->Connect( wxEVT_PAINT, wxPaintEventHandler( wxLedNumber::OnPaint ), NULL, this );
	}
	
	void OnPaint( wxPaintEvent& event )
	{
		wxPaintDC dc(this);
		
		wxBrush oldBru = dc.GetBrush();
		wxPen pen = dc.GetPen();

		dc.SetBrush( wxBrush( GetBackgroundColour() ) );
		dc.SetPen( *wxTRANSPARENT_PEN );
		dc.DrawRectangle( wxPoint(0,0), GetSize()  );
		dc.SetPen( pen );

		int mode = dc.GetBackgroundMode();
		dc.SetBackgroundMode( wxTRANSPARENT );

		// ��ȡ�ַ���
		wxString szFormat = wxString::Format( wxT("%%%d.%df"), m_iWidInter+m_iSign+1, m_iSign );

		wxString strVal = wxString::Format( 
			szFormat,
			m_fVal
			);

		wxSize size = GetSize();
		int wid = size.GetHeight() / 2.5f;


		////// ������ʼλ��
		int left = 0;

		{
			long style = GetWindowStyle();
			if( style & wxALIGN_LEFT )
			{
				left = 0;
			}
			else if( style & wxALIGN_RIGHT )
			{
				left = size.GetWidth() - strVal.Length() * (wid + wid*0.5f );
			}
			else if( style & wxALIGN_CENTER )
			{
				left = (size.GetWidth() - strVal.Length() * (wid + wid*0.5f ))*0.5;
			}
		}


		///////

		dc.SetBrush( wxBrush( m_clr ) );
		DrawStringData( dc, strVal.mb_str(wxConvLocal), left, wid );
		dc.SetBrush( oldBru ) ;
		dc.SetBackgroundMode( mode );
		//
		if (m_bDrawUint)
		{
			wxSize Size = GetSize();
			dc.SetFont( wxFont( 14, 70, 90, 90, false, wxT("΢���ź�") ) );
			long String_Wid;
			long String_Hei;
			dc.GetTextExtent(m_Uint,&String_Wid,&String_Hei);
			dc.DrawText(m_Uint,(15+left+strVal.Length()*1.5*wid),(Size.GetHeight()-String_Hei)/2);
		}
	}


	void OnEraseEvent(wxEraseEvent& event)
	{
		event.Skip();
	}


	/*! ����ֵ
		\param fVal ������
		\param wid �������ֱ����Ŀ��
		\param sign С���㱣��λ��
	*/
	void SetValue( float fVal, int wid = 3, int sign = 2,const wxString &Uint = wxEmptyString)
	{
		m_fVal = fVal;
		m_iSign = sign;
		m_iWidInter = wid;

		if (!Uint.IsEmpty())
		{
			m_bDrawUint = true;
			m_Uint = Uint;				//m_bDrawUint�������ǰ��
		}
		Refresh();
	}

	float GetValue() { return m_fVal; };

	/*! �����������ɫֵ
		\param clr ��ɫֵ
	*/
	void SetColor( wxColour clr ) { m_clr = clr; };

}; // end class wxLedNumber

}; // end namespace CDYW

#endif // HEADER_Number_2010_7_8
