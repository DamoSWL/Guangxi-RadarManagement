#ifndef CDYWTIMEPANEL_H_20140508
#define CDYWTIMEPANEL_H_20140508

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/spinbutt.h>
#include <wx/sizer.h>
#include <wx/panel.h>

//************************************
// 名称:	TimePanel
// 类:		CDYW::TimePanel
// 提供时分秒的显示和输入
//************************************

namespace CDYW
{
	class cdywTimePanel : public wxPanel 
	{
#define wxID_H 1000
#define wxID_M 1001
#define wxID_S 1002

	protected:
		wxTextCtrl* m_textCtrl_H;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl_M;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrl_S;
		wxSpinButton* m_spinBtn1;
	public:
		cdywTimePanel::cdywTimePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
		{
			wxSize sizetextCtrl(-1,-1);			//初始输入控件长度
			wxSize sizeSpinctrl(16,25);			//初始Spin长度
			if (size.x >= 105)
			{
				sizetextCtrl.x = (size.x - sizeSpinctrl.x-17)/3;
			}

			wxBoxSizer* bSizer7;
			bSizer7 = new wxBoxSizer( wxHORIZONTAL );

			m_textCtrl_H = new wxTextCtrl( this, wxID_H, wxT("0"), wxDefaultPosition, sizetextCtrl, 0 );
			bSizer7->Add( m_textCtrl_H, 0, wxALIGN_CENTER_VERTICAL, 5 );

			m_staticText9 = new wxStaticText( this, wxID_ANY, wxT(":"), wxDefaultPosition, wxDefaultSize, 0 );
			m_staticText9->Wrap( -1 );
			bSizer7->Add( m_staticText9, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

			m_textCtrl_M = new wxTextCtrl( this, wxID_M, wxT("0"), wxDefaultPosition, sizetextCtrl, 0 );
			bSizer7->Add( m_textCtrl_M, 0, wxALIGN_CENTER_VERTICAL, 5 );

			m_staticText10 = new wxStaticText( this, wxID_ANY, wxT(":"), wxDefaultPosition, wxDefaultSize, 0 );
			m_staticText10->Wrap( -1 );
			bSizer7->Add( m_staticText10, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

			m_textCtrl_S = new wxTextCtrl( this, wxID_S, wxT("0"), wxDefaultPosition, sizetextCtrl, 0 );
			bSizer7->Add( m_textCtrl_S, 0, wxALIGN_CENTER_VERTICAL, 5 );

			m_spinBtn1 = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, sizeSpinctrl, 0 );
			bSizer7->Add( m_spinBtn1, 0, wxEXPAND, 5 );


			this->SetSizer( bSizer7 );
			this->Layout();
			bSizer7->Fit( this );

			// Connect Events
			m_textCtrl_H->Connect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_H->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_H->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_H->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_textCtrl_M->Connect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_M->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_M->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_M->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_textCtrl_S->Connect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_S->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_S->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_S->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_spinBtn1->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( cdywTimePanel::OnSpinDown ), NULL, this );
			m_spinBtn1->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( cdywTimePanel::OnSpinUp ), NULL, this );


			//
			m_pCurTextCtrl = m_textCtrl_H;
			m_time = wxDateTime::Now();
			
		}

		cdywTimePanel::~cdywTimePanel()
		{
			// Disconnect Events
			m_textCtrl_H->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_H->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_H->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_H->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_textCtrl_M->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_M->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_M->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_M->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_textCtrl_S->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cdywTimePanel::OnChar ), NULL, this );
			m_textCtrl_S->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywTimePanel::OnKillFocus ), NULL, this );
			m_textCtrl_S->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( cdywTimePanel::OnLeftDown ), NULL, this );
			m_textCtrl_S->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywTimePanel::OnSetFocus ), NULL, this );
			m_spinBtn1->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( cdywTimePanel::OnSpinDown ), NULL, this );
			m_spinBtn1->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( cdywTimePanel::OnSpinUp ), NULL, this );

		}
		// Virtual event handlers, overide them in your derived class
	private:
		void OnSetFocus( wxFocusEvent& event )	{	SetCurIndex(event.GetId());		event.Skip(); }
		void OnLeftDown( wxMouseEvent& event )	{	CheckRang();	SetCurIndex(event.GetId());		event.Skip(); }
		void OnKillFocus( wxFocusEvent& event ) {	CheckRang();		event.Skip(); 	}
		void CheckRang()
		{
			if (m_pCurTextCtrl!=NULL)
			{
				int id = m_pCurTextCtrl->GetId();
				//去除前面0
				int CurV = wxAtoi(m_pCurTextCtrl->GetValue());		//由于无效数也转换为0
				//范围过滤
				int iMaxV = 60;
				if (m_pCurTextCtrl->GetId() == wxID_H)			//小时的最大范围24
				{
					iMaxV = 24;
				}
				if (CurV < 0)
				{
					CurV = 0;
				}
				else if (CurV >= iMaxV)
				{
					CurV = iMaxV - 1;
				}
				wxString tp = wxString::Format(wxT("%d"),CurV);
				m_pCurTextCtrl->SetValue(tp);
			}
		}
		void SetCurIndex(int id)
		{
			m_pCurTextCtrl = NULL;
			switch(id)
			{
			case wxID_H:
				m_pCurTextCtrl = m_textCtrl_H;
				break;
			case wxID_M:
				m_pCurTextCtrl = m_textCtrl_M;
				break;
			case wxID_S:
				m_pCurTextCtrl = m_textCtrl_S;
				break;
			}
		}
		 void OnChar( wxKeyEvent& event ) 
		 { 
			 bool bIsOk = false;
			 int KeyCode = event.GetKeyCode();
			 switch (KeyCode)								//常用按键
			 {
			 case 1:						//Ctrl+A
			 case 3:						//Ctrl+C
			 case 22:					//Ctrl+V
			 case 24:					//Ctrl+X
			 case 26:					//Ctrl+Z
			 case WXK_SHIFT:
			 case WXK_ALT:
			 case WXK_PAUSE:
			 case WXK_PRINT:
			 case WXK_HELP:
			 case WXK_INSERT:
			 case WXK_CLEAR:
			 case WXK_BACK :
			 case WXK_LEFT :
			 case WXK_RIGHT :
			 case WXK_TAB :
			 case WXK_DELETE :
			 case WXK_END :
			 case WXK_HOME :
			 case WXK_NUMLOCK:
			 case WXK_NUMPAD_DECIMAL:
				 bIsOk = true;
				 break;
			 default:
				 {
					 if ((KeyCode>='0' && KeyCode<='9') || (KeyCode>=WXK_NUMPAD0 && KeyCode<=WXK_NUMPAD9))
					 {
						bIsOk = true;
					 }
				 }break;
			 }
			event.Skip(bIsOk);
		 }
		void OnSpinDown( wxSpinEvent& event ) { SpinChange(false);		event.Skip(); }
		void OnSpinUp( wxSpinEvent& event ) { 	SpinChange(true);		event.Skip(); }
		void SpinChange(bool Up)
		{
			if (m_pCurTextCtrl!=NULL)
			{
				int id = m_pCurTextCtrl->GetId();
				int CurData = wxAtoi(m_pCurTextCtrl->GetValue());
				if (Up)
				{
					CurData++;
				}
				else 
					CurData--;

				if (id != wxID_H)
				{
					CurData = (CurData+60)%60;
				}
				else 
				{
					CurData = (CurData+24)%24;
				}

				m_pCurTextCtrl->SetValue(wxString::Format(wxT("%d"),CurData));
			}
		}
		bool GetPanlTime()			//获取面板上的时间到m_time
		{
			m_time = wxDateTime::Now();
			int iH = wxAtoi(m_textCtrl_H->GetValue());
			int iM = wxAtoi(m_textCtrl_M->GetValue());
			int iS = wxAtoi(m_textCtrl_S->GetValue());
			if (iH >= 24 || iH < 0 || 
				iM >= 60 || iM < 0 || 
				iS >= 60 || iS < 0 )
			{
				return false;
			}
			m_time.SetHour(iH);
			m_time.SetMinute(iM);
			m_time.SetSecond(iS);
			return true;
		}
	public:
		wxDateTime GetTime()
		{
			if (!GetPanlTime())
			{
				m_time = wxInvalidDateTime;
			}
			return m_time;
		}
		bool SetTime(IN const wxDateTime &time)
		{
			if (time.IsValid())
			{
				m_textCtrl_H->SetValue(wxString::Format(wxT("%d"),time.GetHour()));
				m_textCtrl_M->SetValue(wxString::Format(wxT("%d"),time.GetMinute()));
				m_textCtrl_S->SetValue(wxString::Format(wxT("%d"),time.GetSecond()));
				return true;
			}
			return false;
		}
		bool SetHour(IN int iValue)
		{
			if (iValue >= 0 && iValue < 24)
			{
				m_textCtrl_H->SetValue(wxString::Format(wxT("%d"),iValue));
				return true;
			}
			return false;
		}
		bool SetMinute(IN int iValue)
		{
			if (iValue >= 0 && iValue < 60)
			{
				m_textCtrl_M->SetValue(wxString::Format(wxT("%d"),iValue));
				return true;
			}
			return false;
		}
		bool SetSecond(IN int iValue)
		{
			if (iValue >= 0 && iValue < 60)
			{
				m_textCtrl_S->SetValue(wxString::Format(wxT("%d"),iValue));
				return true;
			}
			return false;
		}
	private:
		wxDateTime m_time;			//时间
		wxTextCtrl *m_pCurTextCtrl;		//鼠标左键点击操作获取的指针
	};
};
#endif 
