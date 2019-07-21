#ifndef __CDYWCOMBOBOXINPUT_20140910_H__
#define __CDYWCOMBOBOXINPUT_20140910_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bmpcbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
//************************************
// 名称:	cdywComboBox
// 类:		CDYW::cdywComboBox
// 输入:	SetDefaultSelect	默认显示的文本信息
//			SetRange			输入范围（全闭区间）
//			SetValue			设置显示值
// 输出:	GetValue
//			IsShowName			是否显示名称
//************************************

typedef struct _CDYWCOMBOBOXINPUT_
{
	char	name[24];		//显示的名称
	float	value;			//读取的数值
}tagCDYWComboBoxInput;

namespace CDYW
{
	class cdywComboBox : public wxPanel 
	{
	private:

	protected:
		wxComboBox* m_comboBoxInput;

	public:
		cdywComboBox::cdywComboBox( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
		{
			m_bSizer = new wxBoxSizer( wxHORIZONTAL );
			m_sizetextCtrl.x = -1;
			m_sizetextCtrl.y = -1;			//初始输入控件长度
			m_sizetextCtrl = size;
			if (size.x!=-1 && size.x<50)
			{
				m_sizetextCtrl.x = 50;
			}			
			m_comboBoxInput = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, m_sizetextCtrl, 0, NULL, 0 ); 
			m_bSizer->Add( m_comboBoxInput, 0, wxALIGN_CENTER_VERTICAL, 5 );


			this->SetSizer( m_bSizer );
			this->Layout();

			// Connect Events
			m_comboBoxInput->Connect( wxEVT_CHAR, wxKeyEventHandler( cdywComboBox::OnChar ), NULL, this );
			m_comboBoxInput->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywComboBox::OnKillFocus ), NULL, this );
			m_comboBoxInput->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywComboBox::OnSetFocus ), NULL, this );
			//
			m_bShowName = false;
			m_vecSelBuf.clear();
			m_Range[0]=0;
			m_Range[1]=0;
			m_OldValue = 0;
			m_Dif = 0.00001;
			m_floatLen = 0;
		}
		~cdywComboBox()
		{
			// Disconnect Events
			m_comboBoxInput->Disconnect( wxEVT_CHAR, wxKeyEventHandler( cdywComboBox::OnChar ), NULL, this );
			m_comboBoxInput->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( cdywComboBox::OnKillFocus ), NULL, this );
			m_comboBoxInput->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( cdywComboBox::OnSetFocus ), NULL, this );
		}

	public:


		//************************************
		// Method:    SetDefaultSelect
		// FullName:  CDYW::cdywComboBox::SetDefaultSelect
		// Access:    public 
		// Returns:   void
		// Qualifier: 设置默认的选择和显示方式
		// Parameter: const vector <tagCDYWComboBoxInput> & InputSelect
		// Parameter: bool bShowName
		// Parameter: bool bEnableInput
		//************************************
		void SetDefaultSelect(const vector <tagCDYWComboBoxInput> &InputSelect,bool bShowName)
		{
			if (bShowName)
			{
				int flags = 0;
				flags |= wxCB_READONLY;
				if (m_comboBoxInput!=NULL)
				{
					m_bSizer->Detach(m_comboBoxInput);
					delete m_comboBoxInput;
				}
				m_comboBoxInput = new wxComboBox(this, wxID_ANY, wxEmptyString,
					wxDefaultPosition, m_sizetextCtrl,
					0, NULL,
					flags);
				m_bSizer->Add(m_comboBoxInput, 0, wxALIGN_CENTER_VERTICAL, 5 );
				m_bSizer->Layout();
			}
			m_bShowName = bShowName;
			if (InputSelect.size()>0)
			{
				m_vecSelBuf = InputSelect;				
				InitShow(m_bShowName);
			}
		}
		void SetRange(float iMin,float iMax,int floatLen)
		{
			if (iMin<iMax)
			{
				m_Range[0]=iMin;
				m_Range[1]=iMax;
			}
			else
			{
				m_Range[1]=iMin;
				m_Range[0]=iMax;
			}
			if (floatLen>0)
			{
				m_floatLen = floatLen;
			}			
		}
		//************************************
		// Method:    SetValue
		// FullName:  CDYW::MyPanel1::SetValue
		// Access:    public 
		// Returns:   void
		// Qualifier: 设置显示的值
		// Parameter: const int iVal
		//************************************
		void SetValue(const float iVal)
		{
			if (!m_bShowName)
			{
				wxString strFrm = wxT("%.")+ wxString::Format(wxT("%df"),m_floatLen);
				wxString strValue = wxString::Format(strFrm,iVal);
				m_comboBoxInput->SetValue(strValue);
			}
			else
			{
				wxString strValue = GetNameFormVal(iVal);
				if (strValue.IsEmpty())
				{
					strValue = wxT("无效");
				}
				m_comboBoxInput->SetValue(strValue);
			}		
		}
// 		void SetValue(const wxString &strVal)
// 		{
// 			wxString str = strVal;
// 			if (GetValFormName(str))
// 			{
// 				m_comboBoxInput->SetValue(strVal);
// 			}
// 			else
// 			{
// 				if (m_vecSelBuf.size()>0)
// 				{
// 					m_comboBoxInput->SetSelection(0);
// 				}				
// 			}			
// 		}
		float GetValue() 
		{
			wxString strvalu = m_comboBoxInput->GetValue();
			if (m_bShowName)
			{
				GetValFormName(strvalu);
			}
			float OutValue = wxAtof(strvalu);
			return OutValue;
		}
		bool IsShowName()
		{
			return m_bShowName;
		}
	private:
		int m_floatLen;
		float m_Dif;
		wxBoxSizer* m_bSizer;
		wxSize m_sizetextCtrl;
		float m_OldValue;
		bool m_bShowName;								//显示文字，输出时候必须转换
		vector <tagCDYWComboBoxInput> m_vecSelBuf;		//初始化选择的容器
		float m_Range[2];									//范围
		bool IsInRange(int iVal)
		{
			if (m_Range[0]!=0 || m_Range[1]!=0 )
			{
				if (iVal<m_Range[0])
				{
					return false;
				}
				if (iVal>m_Range[1])
				{
					return false;
				}
			}			
			return true;
		}
		wxString GetNameFormVal(float iValue)
		{
			wxString strtmp = wxEmptyString;
			for (int i=0;i<m_vecSelBuf.size();i++)
			{
				if ((m_vecSelBuf.at(i).value-m_Dif) < iValue &&
					(m_vecSelBuf.at(i).value+m_Dif) > iValue )	//相同就返回值
				{
					strtmp = wxString(m_vecSelBuf.at(i).name,wxConvLocal);
					return strtmp;
				}
			}
			return strtmp;
		}
		bool GetValFormName(wxString &str)
		{
			wxString strtmp = wxEmptyString;
			for (int i=0;i<m_vecSelBuf.size();i++)
			{
				strtmp = wxString(m_vecSelBuf.at(i).name,wxConvLocal);
				if (str.IsSameAs(strtmp))	//相同就返回值
				{
					wxString strFrm = wxT("%.")+ wxString::Format(wxT("%df"),m_floatLen);
					str = wxString::Format(strFrm,m_vecSelBuf.at(i).value);
					return true;
				}
			}
			return false;
		}
		void InitShow(bool bShowName)
		{
			m_comboBoxInput->Clear();
			if (m_vecSelBuf.size()>0)
			{
				wxArrayString strappenditems;
				for (unsigned int i=0;i<m_vecSelBuf.size();i++)
				{
					if (bShowName)
					{
						strappenditems.Add(wxString(m_vecSelBuf.at(i).name,wxConvLocal));
					}
					else
					{
						wxString strFrm =wxT("%.")+ wxString::Format(wxT("%df"),m_floatLen);
						strappenditems.Add(wxString::Format(strFrm,m_vecSelBuf.at(i).value));
					}					
				}
				m_comboBoxInput->Append(strappenditems);
				m_comboBoxInput->SetSelection(0);
			}
		}
	protected:
		// Virtual event handlers, overide them in your derived class
		virtual void OnChar( wxKeyEvent& event ) 
		{
// 			if (m_bShowName)
// 			{
// 				event.Skip(false);
// 				return;
// 			}
			int KeyCode = event.GetKeyCode();
			switch (KeyCode)								//常用按键
			{
			case 46:
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
				event.Skip(true);
				return;
			default:break;
			}
			if (KeyCode>='0' && KeyCode<='9')
			{
				event.Skip(true);
				return;
			}
			if (KeyCode>=WXK_NUMPAD0 && KeyCode<=WXK_NUMPAD9)
			{
				event.Skip(true);
				return;
			}
			event.Skip(false); 
		}
		virtual void OnSetFocus(wxFocusEvent& event)
		{
			wxString strvalu = m_comboBoxInput->GetValue();
			m_OldValue = wxAtof(strvalu);
			event.Skip();
		}
		virtual void OnKillFocus( wxFocusEvent& event ) 
		{
			if (!m_bShowName)
			{
				wxString strvalu = m_comboBoxInput->GetValue();
				float iValue = wxAtof(strvalu);
				if (!IsInRange(iValue))
				{
					iValue = m_OldValue;
				}
				wxString strFrm =wxT("%.")+ wxString::Format(wxT("%df"),m_floatLen);
				strvalu = wxString::Format(strFrm,iValue);
				m_comboBoxInput->SetValue(strvalu);
			}			
			event.Skip();
		}
	};
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyPanel1
///////////////////////////////////////////////////////////////////////////////


#endif //__COMBOBOXINPUT_H__
