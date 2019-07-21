///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May 15 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GuangXi_gui.h"

///////////////////////////////////////////////////////////////////////////

GuangXiPackPanel::GuangXiPackPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	
	mainSizer->Add( 0, 0, 1, 0, 0 );
	
	m_OK = new wxButton( this, ID_MAIN_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_OK, 0, wxALL|wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );
	
	
	this->SetSizer( mainSizer );
	this->Layout();
}

GuangXiPackPanel::~GuangXiPackPanel()
{
}

GuangXiPanelSet::GuangXiPanelSet( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetFont( wxFont( 11, 74, 90, 90, false, wxT("微软雅黑") ) );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer9->Add( 0, 10, 0, wxEXPAND, 5 );
	
	m_scrolledWindow2 = new wxScrolledWindow( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("基数据文件路径:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( 9, 74, 90, 90, false, wxT("微软雅黑") ) );
	m_staticText2->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer3->Add( 10, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_textCtrlJishu = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrlJishu, 1, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer3->Add( 30, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_buttonJishu = new wxButton( m_scrolledWindow2, wxID_ANY, wxT("浏览"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonJishu, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer10->Add( bSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText21 = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("产品文件路径:"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_staticText21->Wrap( -1 );
	m_staticText21->SetFont( wxFont( 9, 74, 90, 90, false, wxT("微软雅黑") ) );
	
	bSizer31->Add( m_staticText21, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer31->Add( 10, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_textCtrlErchi = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_textCtrlErchi, 1, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer31->Add( 30, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_buttonErchi = new wxButton( m_scrolledWindow2, wxID_ANY, wxT("浏览"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_buttonErchi, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer10->Add( bSizer31, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText22 = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("状态文件路径:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	m_staticText22->SetFont( wxFont( 9, 74, 90, 90, false, wxT("微软雅黑") ) );
	m_staticText22->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer32->Add( m_staticText22, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer32->Add( 10, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_textCtrlZhuangtai = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_textCtrlZhuangtai, 1, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer32->Add( 30, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_buttonZhuangtai = new wxButton( m_scrolledWindow2, wxID_ANY, wxT("浏览"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer32->Add( m_buttonZhuangtai, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer10->Add( bSizer32, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText25 = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("拼图文件路径:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	m_staticText25->SetFont( wxFont( 9, 74, 90, 90, false, wxT("微软雅黑") ) );
	m_staticText25->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer35->Add( m_staticText25, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer35->Add( 10, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_textCtrlPingtu = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_textCtrlPingtu, 1, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer35->Add( 30, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_buttonPingtu = new wxButton( m_scrolledWindow2, wxID_ANY, wxT("浏览"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_buttonPingtu, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer10->Add( bSizer35, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText23 = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("报警文件路径:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	m_staticText23->SetFont( wxFont( 9, 74, 90, 90, false, wxT("微软雅黑") ) );
	m_staticText23->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer33->Add( m_staticText23, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer33->Add( 10, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_textCtrlBaojing = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_textCtrlBaojing, 1, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer33->Add( 30, 0, 0, wxFIXED_MINSIZE, 5 );
	
	m_buttonBaojing = new wxButton( m_scrolledWindow2, wxID_ANY, wxT("浏览"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_buttonBaojing, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer10->Add( bSizer33, 0, wxEXPAND, 5 );
	
	
	m_scrolledWindow2->SetSizer( bSizer10 );
	m_scrolledWindow2->Layout();
	bSizer10->Fit( m_scrolledWindow2 );
	bSizer9->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer9 );
	m_panel1->Layout();
	bSizer9->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, wxT("文件目录配置"), false );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer11->Add( 0, 10, 0, wxEXPAND, 5 );
	
	m_scrolledWindow21 = new wxScrolledWindow( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow21->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 20, 0, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText6 = new wxStaticText( m_scrolledWindow21, wxID_ANY, wxT("FTP日志类型： 1. 基数据文件  2.产品文件  3.状态文件  4.拼图文件  "), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_CENTRE );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetFont( wxFont( 9, 74, 90, 90, false, wxT("宋体") ) );
	
	bSizer15->Add( m_staticText6, 0, wxALL, 10 );
	
	m_textCtrlFtpType = new wxTextCtrl( m_scrolledWindow21, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	bSizer15->Add( m_textCtrlFtpType, 0, wxALL, 5 );
	
	
	bSizer14->Add( bSizer15, 0, wxEXPAND, 5 );
	
	m_listCtrl_Ftp = new wxListCtrl( m_scrolledWindow21, wxID_HIGHEST, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT|wxLC_VRULES );
	m_listCtrl_Ftp->SetMinSize( wxSize( 520,-1 ) );
	
	bSizer14->Add( m_listCtrl_Ftp, 1, wxALL, 5 );
	
	
	bSizer12->Add( bSizer14, 1, wxEXPAND, 5 );
	
	
	bSizer12->Add( 20, 0, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer13->Add( 0, 20, 0, wxEXPAND, 5 );
	
	m_buttonFtpAdd = new wxButton( m_scrolledWindow21, wxID_ANY, wxT("添加"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonFtpAdd, 0, wxALL, 5 );
	
	m_buttonFtpDel = new wxButton( m_scrolledWindow21, wxID_ANY, wxT("删除"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonFtpDel, 0, wxALL, 5 );
	
	m_buttonFtpType = new wxButton( m_scrolledWindow21, wxID_ANY, wxT("设置"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonFtpType, 0, wxALL, 5 );
	
	
	bSizer12->Add( bSizer13, 0, wxEXPAND, 5 );
	
	
	m_scrolledWindow21->SetSizer( bSizer12 );
	m_scrolledWindow21->Layout();
	bSizer12->Fit( m_scrolledWindow21 );
	bSizer11->Add( m_scrolledWindow21, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer11 );
	m_panel2->Layout();
	bSizer11->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, wxT("FTP日志目录配置"), true );
	
	bSizer5->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer5 );
	this->Layout();
}

GuangXiPanelSet::~GuangXiPanelSet()
{
}
