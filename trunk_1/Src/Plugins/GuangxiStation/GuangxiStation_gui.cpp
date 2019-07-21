///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May 15 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxipctrl.hpp"

#include "GuangxiStation_gui.h"

///////////////////////////////////////////////////////////////////////////

GuangxiStationPackPanel::GuangxiStationPackPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	
	mainSizer->Add( 0, 0, 1, 0, 0 );
	
	m_OK = new wxButton( this, ID_MAIN_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_OK, 0, wxALL|wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );
	
	
	this->SetSizer( mainSizer );
	this->Layout();
}

GuangxiStationPackPanel::~GuangxiStationPackPanel()
{
}

GuangxiStationPanelSet::GuangxiStationPanelSet( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer4->Add( 0, 10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, wxT("IP配置") ), wxVERTICAL );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("RPG："), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer51->Add( m_staticText1, 0, wxALL, 5 );
	
	m_panel_RPGIP = new wxipctrl( m_panel1, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), wxTAB_TRAVERSAL );
	bSizer51->Add( m_panel_RPGIP, 0, wxEXPAND | wxALL, 5 );
	
	
	sbSizer1->Add( bSizer51, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer511;
	bSizer511 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( m_panel1, wxID_ANY, wxT("PUP:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText11->Wrap( -1 );
	bSizer511->Add( m_staticText11, 0, wxALL, 5 );
	
	m_panel_PUPIP = new wxipctrl( m_panel1, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), wxTAB_TRAVERSAL );
	bSizer511->Add( m_panel_PUPIP, 0, wxEXPAND | wxALL, 5 );
	
	
	sbSizer1->Add( bSizer511, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer512;
	bSizer512 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText12 = new wxStaticText( m_panel1, wxID_ANY, wxT("PUP2:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText12->Wrap( -1 );
	bSizer512->Add( m_staticText12, 0, wxALL, 5 );
	
	m_panel_PUP2IP = new wxipctrl( m_panel1, wxID_ANY, wxDefaultPosition, wxSize( 180,-1 ), wxTAB_TRAVERSAL );
	bSizer512->Add( m_panel_PUP2IP, 0, wxEXPAND | wxALL, 5 );
	
	
	sbSizer1->Add( bSizer512, 0, wxEXPAND, 5 );
	
	
	bSizer4->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, wxT("网络配置"), false );
	
	bSizer5->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer5 );
	this->Layout();
}

GuangxiStationPanelSet::~GuangxiStationPanelSet()
{
}
