#ifndef HEADER_ywDlgPack_2010_4_2
#define HEADER_ywDlgPack_2010_4_2


/*! \class YWDlgPack
	本模板类用于封装一个对话框

*/
template< class T >
class YWDlgPack:
	public wxDialog
{
	T* m_tControl;

	wxButton* m_butOK;
	wxButton* m_butCancel;

	wxBoxSizer* m_sz;
public:
	
	YWDlgPack( wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxT("产品查询"), 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSYSTEM_MENU )
		: wxDialog( parent, wxID_ANY, title, pos, size, style )
	{
		SetMinSize( size );
		SetMaxSize( size );

		m_sz = new wxBoxSizer( wxVERTICAL );
		m_tControl = new T( (wxWindow*)this, id );
		m_sz->Add( m_tControl, 1, wxEXPAND|wxALL, 5 );

		/// 确定、取消按钮
		wxBoxSizer* bSizer2;
		bSizer2 = new wxBoxSizer( wxHORIZONTAL );
		bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

		m_butOK = new wxButton( this, wxID_OK, wxT("确定"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( m_butOK, 0, wxALL, 5 );

		m_butCancel = new wxButton( this, wxID_CANCEL, wxT("取消"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( m_butCancel, 0, wxALL, 5 );

		m_sz->Add( bSizer2, 0, wxEXPAND, 5 );

		this->SetSizer( m_sz );
		this->Layout();

		if( pos == wxDefaultPosition )
		{
			wxPoint pt = ::wxGetMousePosition();
			this->Move( pt );
			
		}else{
			this->Centre( wxBOTH );
		}
	}


	T* operator ->()
	{
		return m_tControl;
	}
};

#endif // HEADER_ywDlgPack_2010_4_2

