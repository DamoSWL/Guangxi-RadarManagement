/*
IP¿Ø¼þ
*/



#ifndef WXIPCTRL_H_INCLUDED 
#define WXIPCTRL_H_INCLUDED

#include <wx/wx.h> 	
#include <wx/regex.h> 
#include <wx/msgdlg.h> 

//DECLARE_EVENT_TYPE(wxEVT_IPCTRL_CHANGED, wxID_ANY); 
//DEFINE_EVENT_TYPE(wxEVT_IPCTRL_CHANGED); 

#define wxIPCTRL_D_CLASS         1		//×é²¥
#define wxIPCTRL_ALL_CLASSES     2 
#define wxIPCTRL_SUBNET_MASK     wxIPCTRL_ALL_CLASSES 
#define wxIPCTRL_DEFAULT_STYLE   0 


	class wxipctrl : public wxPanel 
	{ 
		long          m_ipStyle; 
		wxArrayString m_defaultValue; 
		bool bSetFoucs;
		wxBoxSizer* m_sizer; 
		wxTextCtrl* m_editIP[4];
		wxStaticText* m_dot[3];
	public: 

		wxipctrl(wxWindow *parent, wxWindowID id, const wxPoint &Pos,const wxSize &Size,long style )
			:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL) 
		{ 
			m_ipStyle = wxIPCTRL_DEFAULT_STYLE;
			CreateGui(); 

			m_defaultValue.Add( wxT("127") ); 
			m_defaultValue.Add( wxT("0") ); 
			m_defaultValue.Add( wxT("0") ); 
			m_defaultValue.Add( wxT("1") ); 

			SetValue(wxT("127.0.0.1")); 
			Connect( wxEVT_CHAR, wxKeyEventHandler( wxipctrl::OnChar ) ); 
			for(int i=0;i<4;i++)
			{
				m_editIP[i]->Connect( wxEVT_CHAR, wxKeyEventHandler( wxipctrl::OnChar ), NULL, this ); 
				m_editIP[i]->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxipctrl::OnTextUpdated ), NULL, this ); 
				m_editIP[i]->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( wxipctrl::OnKillFocus ), NULL, this );
			}			
		} 
		~wxipctrl()
		{
			Disconnect( wxEVT_CHAR, wxKeyEventHandler( wxipctrl::OnChar ), NULL, this ); 
			for(int i=0;i<4;i++)
			{
				m_editIP[i]->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxipctrl::OnTextUpdated ), NULL, this ); 
				m_editIP[i]->Disconnect( wxEVT_CHAR, wxKeyEventHandler( wxipctrl::OnChar ), NULL, this ); 
			}	
		}
		void OnKillFocus( wxFocusEvent& event )
		{
			wxTextCtrl* evtObject = (wxTextCtrl*)event.GetEventObject();
			const wxString str = evtObject->GetValue(); 

			if (str.Length() == 0) 
			{ 
				evtObject->SetSelection(-1, -1);
				evtObject->SetFocus(); 
				return; 
			} 
			int min=0,max=255;
			if(m_ipStyle & wxIPCTRL_D_CLASS){min=224;max=239;}
			else if(m_ipStyle & wxIPCTRL_ALL_CLASSES){min=0;max=255;}
			else {min=1;max=255;} 
			
			for(int i=0;i<4;i++)
			{
				if(i>0){min=0;max=255;}
				if (evtObject == m_editIP[i] ) 
				{ 
					if (CheckRange(str, min, max)) 
					{ 
						if (str.Length() == 3 && i<3 && evtObject->IsModified()) 
						{ 
							m_editIP[i+1]->SetFocus(); 
							m_editIP[i+1]->SetSelection(-1, -1); 
						} 	
					} 
					else 
					{ 
						m_editIP[i]->SetValue(wxString::Format(wxT("%d"),min)); 
						m_editIP[i]->SetFocus(); 
						m_editIP[i]->SetSelection(-1, -1); 
					} 
				}
			}
	

			/*if (str0.Length() == 0 ) 
			{ 
			int ival=0;
			for(int i=0;i<4;i++)
			{
			if(evtObject==m_editIP[i]) {ival=i;break;}
			}
			evtObject->SetValue(m_defaultValue[ival]);
			evtObject->SetSelection(-1,-1);
			} 
			wxString strvalue= wxString::Format(wxT("%d"),wxAtoi(evtObject->GetValue()));
			evtObject->SetValue(strvalue);*/
			event.Skip();
		}
		void OnSetFocus( wxFocusEvent& event )
		{
			event.Skip();
		}
		wxString GetValue() const
		{
			return  m_editIP[0]->GetValue() + wxT(".") + m_editIP[1]->GetValue() + wxT(".") + m_editIP[2]->GetValue() + wxT(".") + m_editIP[3]->GetValue(); 
		}
		bool SetValue(const wxString& ipaddr)
		{
			wxArrayString ipParts; 
			if (CheckIp(ipaddr, ipParts) == false) 
			{ 
				DoSetDefaultValue(); 
				return false; 
			} 
			DoSetValue(ipParts); 
			return true; 
		} 

		void SetStyle(long style) 
		{ 
			m_ipStyle = style; 
		}
		long GetStyle() 
		{ 
			return m_ipStyle; 
		} 

		bool Enable(bool enable)
		{
			wxColour bkColour = enable ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) : wxSystemSettings::GetColour(wxSYS_COLOUR_MENU); 
			SetBackgroundColour(bkColour); 
			for(int i=0;i<4;i++)
			{
				m_editIP[0]->SetBackgroundColour(bkColour); 
				m_editIP[0]->SetEditable(enable); 
			}
			for(int i=0;i<3;i++)
				m_dot[i]->SetBackgroundColour(bkColour); 

			Refresh(); 
			return true; 
		}; 

	protected: 
		void CreateGui()
		{
			SetBackgroundColour(*wxWHITE); 

			m_sizer = new wxBoxSizer(wxHORIZONTAL); 

			for(int i=0;i<4;i++)
			{
				m_editIP[i]=CreateTextCtrl(); 
				if(i<3)
					m_dot[i]=CreateDot(); 
			}			

			SetSizer(m_sizer); 
			Layout(); 
			m_sizer->Fit(this); 
		}
		wxTextCtrl* CreateTextCtrl()
		{
			wxTextCtrl* txtCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(26, 15), wxTE_CENTRE|wxNO_BORDER); 
			txtCtrl->SetMaxLength(3); 
			m_sizer->Add(txtCtrl, 0, wxALIGN_BOTTOM|wxTOP, 5); 
			return txtCtrl; 
		}
		wxStaticText* CreateDot()
		{
			wxStaticText* staticTxt = new wxStaticText( this, wxID_ANY, _("."), wxDefaultPosition, wxSize(3,15), 0 ); 
			staticTxt->Wrap(-1); 
			m_sizer->Add(staticTxt, 0, wxALIGN_BOTTOM, 5); 
			return staticTxt; 
		}

		bool CheckIp(const wxString& ipaddr, wxArrayString& ipParts)
		{
			wxRegEx strregex(wxT("([0-9]{1,3})([.])([0-9]{1,3})([.])([0-9]{1,3})([.])([0-9]{1,3})")); 
			if (!strregex.Matches(ipaddr)) 
			{ 
				return false; 
			} 

			ipParts.clear(); 
			ipParts.Add( strregex.GetMatch(ipaddr, 1) ); 
			ipParts.Add( strregex.GetMatch(ipaddr, 3) ); 
			ipParts.Add( strregex.GetMatch(ipaddr, 5) ); 
			ipParts.Add( strregex.GetMatch(ipaddr, 7) ); 

			unsigned long ip1, ip2, ip3, ip4; 
			ipParts[0].ToULong(&ip1); 
			ipParts[1].ToULong(&ip2); 
			ipParts[2].ToULong(&ip3); 
			ipParts[3].ToULong(&ip4); 

			if (m_ipStyle & wxIPCTRL_D_CLASS) 
			{ 
				if (!InRange(ip1, 224, 239)) 
				{ 
					return false; 
				} 
			} 
			else if (m_ipStyle & wxIPCTRL_ALL_CLASSES) 
			{ 
				if (!InRange(ip1, 0, 255))
				{ 
					return false; 
				} 
			} 
			else 
			{ 
				if (!InRange(ip1, 0, 223)) 
				{ 
					return false; 
				} 
			} 

			if ( !InRange(ip2, 0, 255) || !InRange(ip3, 0, 255) || !InRange(ip4, 0, 255) ) 
			{ 
				return false; 
			} 
			return true; 
		}

		bool CheckRange(const wxString& value, unsigned long min, unsigned long max)
		{
			unsigned long v = 0; 
			value.ToULong(&v, 10); 
			return InRange(v, min, max);  
		}

		bool InRange(unsigned long value, unsigned long min, unsigned long max)
		{
			return min <= value && value <= max; 
		}

		void DoSetValue(const wxArrayString& ipParts)
		{
			assert(ipParts.size() == 4); 
			for(int i=0;i<4;i++)
				m_editIP[i]->ChangeValue(ipParts[i]); 
		}

		void DoSetDefaultValue()
		{
			for(int i=0;i<4;i++)
				m_editIP[i]->ChangeValue(m_defaultValue[i]); 			 
		} 

		void OnChar(wxKeyEvent& event)
		{
			const wxObject* evtObject = event.GetEventObject(); 			
			const int keyCode = event.GetKeyCode(); 

			if (keyCode >= '0' && keyCode <= '9') 
			{ 
				event.Skip(); 
				return; 
			} 

			if (keyCode == '.') 
			{ 
				for(int i=0;i<3;i++)
				{
					if (evtObject == m_editIP[i] && m_editIP[i]->GetStringSelection().Length() !=m_editIP[i]->GetValue().Length())
					{
						m_editIP[i+1]->SetFocus(); 
						m_editIP[i+1]->SetSelection(-1, -1); 
						break;
					}
				}				
				event.Skip(false); 
			} 
			else if (keyCode == WXK_BACK) 
			{ 
				for(int i=1;i<4;i++)
				{
					if (evtObject == m_editIP[i] && m_editIP[i]->GetValue().IsEmpty())
					{
						m_editIP[i-1]->SetFocus(); 
						m_editIP[i-1]->SetInsertionPointEnd(); 
						break;
					}
				}				
				event.Skip(); 
			} 
			else if (keyCode == WXK_LEFT || keyCode == WXK_UP) 
			{ 
				for(int i=1;i<4;i++)
				{
					if (evtObject == m_editIP[i] && m_editIP[i]->GetInsertionPoint() == 0)
					{
						m_editIP[i-1]->SetFocus(); 
						m_editIP[i-1]->SetInsertionPointEnd(); 
						break;
					}
				}				
				event.Skip(); 
			} 
			else if (keyCode == WXK_RIGHT || keyCode == WXK_DOWN)   
			{ 
				for(int i=0;i<3;i++)
				{
					if (evtObject == m_editIP[i] && m_editIP[i]->GetInsertionPoint() == m_editIP[i]->GetLastPosition())
					{
						m_editIP[i+1]->SetFocus(); 
						m_editIP[i+1]->SetInsertionPoint(0);
						break;
					}
				}				
				event.Skip(); 
			} 
			else if (keyCode == WXK_TAB || keyCode == WXK_DELETE || keyCode == WXK_END || keyCode == WXK_HOME ) 
			{ 
				event.Skip(); 
			} 
		}

		void OnTextUpdated(wxCommandEvent& event)
		{
			//event.Skip();
			//return;
			/*const wxString str = event.GetString();
			bool bSendEvtChanged = false; 
			const wxObject* evtObject = event.GetEventObject(); 
			if (str.Length() == 0) 
			{ 
				((wxTextCtrl*)evtObject)->SetSelection(-1, -1);
				event.Skip();
				return; 
			} 
			int min=0,max=255;
			if(m_ipStyle & wxIPCTRL_D_CLASS){min=224;max=239;}
			else if(m_ipStyle & wxIPCTRL_ALL_CLASSES){min=0;max=255;}
			else {min=1;max=255;} 
			if (evtObject == m_editIP[0])
			{
				if (str.Length() == 3)
				{

				}
			}
			else
			{
				for(int i=1;i<4;i++)
				{
					if(i>0){min=0;max=255;}
					if (evtObject == m_editIP[i] && m_editIP[i]->IsModified()) 
					{ 
						if (CheckRange(str, min, max)) 
						{ 
							if (str.Length() == 3 && i<3) 
							{ 
								m_editIP[i+1]->SetFocus(); 
								m_editIP[i+1]->SetSelection(-1, -1); 
							} 	
						} 
						else 
						{ 
							m_editIP[i]->SetValue(wxString::Format(wxT("%d"),min)); 
							m_editIP[i]->SetFocus(); 
							m_editIP[i]->SetSelection(-1, -1); 
						} 
					}
				}
			}*/
			
			event.Skip(); 
		}
	};


#endif // WXIPCTRL_H_INCLUDED 
