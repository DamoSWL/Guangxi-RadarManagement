/*!  \file wxDlgPack.h  
    \author shudingbo
	\date 2010-11-25
*/
#ifndef __wxdlgPack_GUI__20101125
#define __wxdlgPack_GUI__20101125

#include <wx/panel.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////
/*! \class wxDlgPack    
	本类用于将单一控件封装进一个模式对话框。 这样在需要到一些简单界面，如颜色的选取， 
	文字的输入等，就不用从头写一个类，直接使用即可。    
	本类通过重载 -> 操作符，实现了对控件的直接访问。
*/
template< class T >
class wxDlgPack : public wxDialog 
{
private:
protected:    
	wxPanel* m_panelCon;    
	wxButton* m_buttonOK;    
	wxButton* m_buttonCancel;    
	T *m_pControl;   //!< 控件
public:    

	wxDlgPack( wxWindow* hostWin, long controlStyle, const wxString& controlLabel = wxEmptyString, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 505,354 ), const wxString& dlgLabel = wxEmptyString  )        
		:wxDialog( NULL, wxID_ANY, dlgLabel, pos, size, wxSTAY_ON_TOP )    
	{        
		this->SetSizeHints( wxDefaultSize, wxDefaultSize );        
		wxBoxSizer* bSizer5;        
		bSizer5 = new wxBoxSizer( wxVERTICAL );        
		m_panelCon = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );        
		wxBoxSizer* m_sizer;        
		m_sizer = new wxBoxSizer( wxVERTICAL );        
		m_pControl = new T( m_panelCon, wxID_ANY, controlLabel, wxDefaultPosition, wxDefaultSize, controlStyle );        
		m_sizer->Add( m_pControl, 1, wxALL|wxEXPAND, 5 );        
		m_panelCon->SetSizer( m_sizer );        
		m_panelCon->Layout();        
		m_sizer->Fit( m_panelCon );        
		bSizer5->Add( m_panelCon, 1, wxEXPAND | wxALL, 5 );        
		wxBoxSizer* bSizer6;        
		bSizer6 = new wxBoxSizer( wxHORIZONTAL );       
		bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );        
		m_buttonOK = new wxButton( this, wxID_OK, wxT("确定"), wxDefaultPosition, wxDefaultSize, 0 );        
		bSizer6->Add( m_buttonOK, 0, wxALL, 5 );        
		m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("取消"), wxDefaultPosition, wxDefaultSize, 0 );      
		bSizer6->Add( m_buttonCancel, 0, wxALL, 5 );        
		bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );        
		this->SetSizer( bSizer5 );        
		this->Layout();  

		if( hostWin )
		{
			wxPoint ps1 = hostWin->ClientToScreen( wxPoint(0,0) );        
			wxSize sizeHost = hostWin->GetSize();        
			Move(ps1.x, ps1.y + sizeHost.GetHeight() );   
		}
		else
		{
			SetWindowStyle(  GetWindowStyle() |  wxDEFAULT_DIALOG_STYLE  );
		}
	};   

	/*! 重载指针操作符    */    
	T* operator->()
	{           
		return m_pControl;       
	}; 
};
#endif //__wxdlgPack_GUI__20101125