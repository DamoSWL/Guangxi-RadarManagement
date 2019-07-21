///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May 15 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUANGXISTATION_GUI_H__
#define __GUANGXISTATION_GUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
class wxipctrl;

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_MAIN_OK 1000

///////////////////////////////////////////////////////////////////////////////
/// Class GuangxiStationPackPanel
///////////////////////////////////////////////////////////////////////////////
class GuangxiStationPackPanel : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_OK;
	
	public:
		
		GuangxiStationPackPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~GuangxiStationPackPanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GuangxiStationPanelSet
///////////////////////////////////////////////////////////////////////////////
class GuangxiStationPanelSet : public wxPanel 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxipctrl* m_panel_RPGIP;
		wxStaticText* m_staticText11;
		wxipctrl* m_panel_PUPIP;
		wxStaticText* m_staticText12;
		wxipctrl* m_panel_PUP2IP;
	
	public:
		
		GuangxiStationPanelSet( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~GuangxiStationPanelSet();
	
};

#endif //__GUANGXISTATION_GUI_H__
