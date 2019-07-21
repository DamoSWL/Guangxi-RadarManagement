#ifndef SPINCTRL_H_20131018 
#define SPINCTRL_H_20131018

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/spinbutt.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <math.h>
//************************************
// 名称:	SpinCtrl
// 类:		CDYW::spinctrl
// 输入:	SetValue
//			SetFLength			//小数点长度
//			SetRange			//输入范围（默认全闭区间）
// 输出:	GetValue
// 功能:	1.可以输入控件长度，必须大于56
//			2.可控制小数点后几位,不得大于10
//			3.更具输入的位置，判断增加1或者0.n1
//			4.输入范围可以选择开闭区间
//			5.移开焦点时，进行值的判断
// 注意：	1.如果没有限制范围，当输入错误的时候，默认设置为0
//			2.默认的小数点保留1位
//************************************
enum SPINCTRLRANGEMODE
{
	SPINCTRLALLCLOSE=0,			//全闭区间
	SPINCTRLLEFTCLOSE=1,		//左闭右开
	SPINCTRLRIGHTCLOSE=2,		//右闭左开
	SPINCTRLNOCLOSE=3,			//全开
};
namespace CDYW
{
	class spinctrl : public wxPanel 
	{
	protected:
		wxTextCtrl*			m_textCtrl;
		wxSpinButton*		m_spinBtn;
		int					m_iLength;					//小数点后位数
		float				m_fDecStep;					//小数点后的位数，按照m_iLength判断
		wxString			m_strFormat;				//格式化文本
		double				m_MinVal;
		double				m_MaxVal;
		SPINCTRLRANGEMODE	m_RangeMode;				//开闭功能
	public:

		spinctrl::spinctrl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
		{
			wxBoxSizer* bSizer200;
			bSizer200 = new wxBoxSizer( wxHORIZONTAL );
			wxSize sizetextCtrl(-1,-1);			//初始输入控件长度
			wxSize sizeSpinctrl(16,25);			//初始Spin长度
			if (size.x >= 56)
			{
				sizetextCtrl.x = (size.x - sizeSpinctrl.x);
			}
			m_textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, sizetextCtrl, 0 );
			bSizer200->Add( m_textCtrl, 0, wxALIGN_CENTER_VERTICAL, 5 );
			
			m_spinBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, sizeSpinctrl, wxSP_VERTICAL );
			bSizer200->Add( m_spinBtn, 0, wxALIGN_CENTER_VERTICAL, 5 );

			this->SetSizer( bSizer200 );
			this->Layout();
			bSizer200->Fit( this );

			// Connect Events
			m_textCtrl->Connect( wxEVT_CHAR, wxKeyEventHandler( spinctrl::SpinCtrlOnChar ), NULL, this );
			m_textCtrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( spinctrl::SpinCtrlOnKillFocus ), NULL, this );
			m_spinBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( spinctrl::SpinCtrlOnSpinDown ), NULL, this );
			m_spinBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( spinctrl::SpinCtrlOnSpinUp ), NULL, this );
			//Init
			m_MaxVal = 0;
			m_MinVal = 0;
			m_iLength = 1;
			FormatStr(m_iLength);
		};

		~spinctrl()
		{
			m_textCtrl->Disconnect( wxEVT_CHAR, wxKeyEventHandler( spinctrl::SpinCtrlOnChar ), NULL, this );
			m_textCtrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( spinctrl::SpinCtrlOnKillFocus ), NULL, this );
			m_spinBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( spinctrl::SpinCtrlOnSpinDown ), NULL, this );
			m_spinBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( spinctrl::SpinCtrlOnSpinUp ), NULL, this );
		};
		//输出函数
		double GetValue() const
		{
			return wxAtof( m_textCtrl->GetValue() );
		};
		//输入函数
		void SetValue(int ivalue)
		{
			double fvalue = 1.0*ivalue;
			InputValue(fvalue);
		};
		void SetValue(double fvalue)
		{
			InputValue(fvalue);
		};
		void SetFLength(const int iLen)
		{
			if (iLen>=0 && iLen<10)
			{
				FormatStr(iLen);
			}
		};
		bool SetRange(double min,double max,SPINCTRLRANGEMODE Mode = SPINCTRLALLCLOSE)
		{
			if (min < max)
			{
				m_MinVal = min;
				m_MaxVal = max;
				m_RangeMode = Mode;
				return true;
			}
			return false;
		};
	protected:
		void InputValue(double fValue)
		{
			if (!InRange(fValue))
			{
				if (fValue >= m_MaxVal)
				{
					fValue = m_MaxVal;
				}
				else
				{
					fValue = m_MinVal;
				}
			}
			wxString strval = wxString::Format(m_strFormat,fValue);
			m_textCtrl->SetValue(strval);
		}
		void SpinCtrlOnKillFocus( wxFocusEvent& event ) 
		{
			wxString strValue = m_textCtrl->GetValue();
			double fValue=0;
			if (!strValue.ToDouble(&fValue) || !InRange(fValue))
			{
				if (fValue >= m_MaxVal)
				{
					fValue = m_MaxVal;
				}
				else
				{
					fValue = m_MinVal;
				}
			}
			wxString strval = wxString::Format(m_strFormat,fValue);
			m_textCtrl->SetValue(strval);
			event.Skip();
		}
		bool InRange(double fVal)
		{
			if (m_MinVal == m_MaxVal)
			{
				return true;
			}
			else
			{
				if (m_RangeMode == SPINCTRLALLCLOSE)
				{
					if (fVal >= m_MinVal && fVal <= m_MaxVal)
					{
						return true;
					}
				}
				else if (m_RangeMode == SPINCTRLNOCLOSE)
				{
					if (fVal > m_MinVal && fVal < m_MaxVal)
					{
						return true;
					}
				}
				else if (m_RangeMode == SPINCTRLLEFTCLOSE)
				{
					if (fVal >= m_MinVal && fVal < m_MaxVal)
					{
						return true;
					}
				}
				else if (m_RangeMode == SPINCTRLRIGHTCLOSE)
				{
					if (fVal > m_MinVal && fVal <= m_MaxVal)
					{
						return true;
					}
				}
			}
			return false;
		}
		void SpinCtrlOnChar( wxKeyEvent& event ) 
		{ 
			bool bt = true;
			int KeyCode = event.GetKeyCode();
			if (KeyCode == 315)			//UP
			{
				bt = false;
				ChangeValue(true);
			}
			else if (KeyCode == 317)	//Down
			{
				bt = false;
				ChangeValue(false);
			}
			event.Skip(bt);
		};
		void SpinCtrlOnSpinDown( wxSpinEvent& event ) 
		{ 
			ChangeValue(false);
			event.Skip();
		};
		void SpinCtrlOnSpinUp( wxSpinEvent& event ) 
		{ 
			ChangeValue(true);
			event.Skip();
		};
		void FormatStr(int ilen)
		{
			m_iLength = ilen;
			m_fDecStep = pow(10.0,(-1.0*m_iLength));
			m_strFormat =wxT("%.") + wxString::Format(wxT("%df"),ilen);
		};
		void ChangeValue(bool bUp)
		{
			double iStepLen = 1;									//默认步进增加为1
			wxString CurStr = m_textCtrl->GetValue();
			bool bInsertionDecRight = false;										//增加为小数点右边
			if (!CurStr.IsEmpty() && CurStr.Contains(wxT(".")))					//如果包含小数点，则判断增加的数字1或者0，1
			{
				int idecimalindex = CurStr.Find(wxT("."));
				if (idecimalindex != -1)										//找到小数位数
				{
					int iCurInputIndex = m_textCtrl->GetInsertionPoint();
					if (iCurInputIndex > idecimalindex)							//大于小数点，增加整数0.1
					{
						iStepLen = m_fDecStep;
						bInsertionDecRight = true;
					}
				}
			}
			if (bUp)			//增加
			{
				float fvlue = wxAtof(CurStr)+iStepLen;
				if (InRange(fvlue))
				{
					m_textCtrl->SetValue(wxString::Format(m_strFormat,fvlue));
				}
			}
			else				//减小 
			{
				float fvlue = wxAtof(CurStr)-iStepLen;
				if (InRange(fvlue))
				{
					m_textCtrl->SetValue(wxString::Format(m_strFormat,fvlue));
				}
			}

			if (bInsertionDecRight)											//在小数点右边，需要还原
			{
				m_textCtrl->SetInsertionPointEnd();							//默认设置后输入在头
			}
		};
	};
};
#endif 
