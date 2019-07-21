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
// ����:	SpinCtrl
// ��:		CDYW::spinctrl
// ����:	SetValue
//			SetFLength			//С���㳤��
//			SetRange			//���뷶Χ��Ĭ��ȫ�����䣩
// ���:	GetValue
// ����:	1.��������ؼ����ȣ��������56
//			2.�ɿ���С�����λ,���ô���10
//			3.���������λ�ã��ж�����1����0.n1
//			4.���뷶Χ����ѡ�񿪱�����
//			5.�ƿ�����ʱ������ֵ���ж�
// ע�⣺	1.���û�����Ʒ�Χ������������ʱ��Ĭ������Ϊ0
//			2.Ĭ�ϵ�С���㱣��1λ
//************************************
enum SPINCTRLRANGEMODE
{
	SPINCTRLALLCLOSE=0,			//ȫ������
	SPINCTRLLEFTCLOSE=1,		//����ҿ�
	SPINCTRLRIGHTCLOSE=2,		//�ұ���
	SPINCTRLNOCLOSE=3,			//ȫ��
};
namespace CDYW
{
	class spinctrl : public wxPanel 
	{
	protected:
		wxTextCtrl*			m_textCtrl;
		wxSpinButton*		m_spinBtn;
		int					m_iLength;					//С�����λ��
		float				m_fDecStep;					//С������λ��������m_iLength�ж�
		wxString			m_strFormat;				//��ʽ���ı�
		double				m_MinVal;
		double				m_MaxVal;
		SPINCTRLRANGEMODE	m_RangeMode;				//���չ���
	public:

		spinctrl::spinctrl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
		{
			wxBoxSizer* bSizer200;
			bSizer200 = new wxBoxSizer( wxHORIZONTAL );
			wxSize sizetextCtrl(-1,-1);			//��ʼ����ؼ�����
			wxSize sizeSpinctrl(16,25);			//��ʼSpin����
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
		//�������
		double GetValue() const
		{
			return wxAtof( m_textCtrl->GetValue() );
		};
		//���뺯��
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
			double iStepLen = 1;									//Ĭ�ϲ�������Ϊ1
			wxString CurStr = m_textCtrl->GetValue();
			bool bInsertionDecRight = false;										//����ΪС�����ұ�
			if (!CurStr.IsEmpty() && CurStr.Contains(wxT(".")))					//�������С���㣬���ж����ӵ�����1����0��1
			{
				int idecimalindex = CurStr.Find(wxT("."));
				if (idecimalindex != -1)										//�ҵ�С��λ��
				{
					int iCurInputIndex = m_textCtrl->GetInsertionPoint();
					if (iCurInputIndex > idecimalindex)							//����С���㣬��������0.1
					{
						iStepLen = m_fDecStep;
						bInsertionDecRight = true;
					}
				}
			}
			if (bUp)			//����
			{
				float fvlue = wxAtof(CurStr)+iStepLen;
				if (InRange(fvlue))
				{
					m_textCtrl->SetValue(wxString::Format(m_strFormat,fvlue));
				}
			}
			else				//��С 
			{
				float fvlue = wxAtof(CurStr)-iStepLen;
				if (InRange(fvlue))
				{
					m_textCtrl->SetValue(wxString::Format(m_strFormat,fvlue));
				}
			}

			if (bInsertionDecRight)											//��С�����ұߣ���Ҫ��ԭ
			{
				m_textCtrl->SetInsertionPointEnd();							//Ĭ�����ú�������ͷ
			}
		};
	};
};
#endif 
