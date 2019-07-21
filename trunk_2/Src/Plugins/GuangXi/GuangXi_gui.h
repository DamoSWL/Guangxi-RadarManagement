///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May 15 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUANGXI_GUI_H__
#define __GUANGXI_GUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_MAIN_OK 1000

///////////////////////////////////////////////////////////////////////////////
/// Class GuangXiPackPanel
///////////////////////////////////////////////////////////////////////////////
class GuangXiPackPanel : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_OK;
	
	public:
		
		GuangXiPackPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~GuangXiPackPanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GuangXiPanelSet
///////////////////////////////////////////////////////////////////////////////
class GuangXiPanelSet : public wxPanel 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxScrolledWindow* m_scrolledWindow2;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlJishu;
		wxButton* m_buttonJishu;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_textCtrlErchi;
		wxButton* m_buttonErchi;
		wxStaticText* m_staticText22;
		wxTextCtrl* m_textCtrlZhuangtai;
		wxButton* m_buttonZhuangtai;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textCtrlPingtu;
		wxButton* m_buttonPingtu;
		wxStaticText* m_staticText23;
		wxTextCtrl* m_textCtrlBaojing;
		wxButton* m_buttonBaojing;
		wxPanel* m_panel2;
		wxScrolledWindow* m_scrolledWindow21;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrlFtpType;
		wxListCtrl* m_listCtrl_Ftp;
		wxButton* m_buttonFtpAdd;
		wxButton* m_buttonFtpDel;
		wxButton* m_buttonFtpType;
	
	public:
		
		GuangXiPanelSet( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 695,405 ), long style = wxTAB_TRAVERSAL ); 
		~GuangXiPanelSet();
	
};

#endif //__GUANGXI_GUI_H__
