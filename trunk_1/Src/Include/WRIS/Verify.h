#ifndef verify_h__20090829
#define verify_h__20090829


#include <wx/textctrl.h>
#include <wx/msgdlg.h>


template< typename T >
bool TxtVerifyRange( wxTextCtrl *pCtrl, T &dVal, T dMin, T dMax, bool bQuite=false, wxString szTitle=wxT("") )
{
	double dValT;
	if( pCtrl->GetValue().ToDouble( &dValT ) )
	{
		// ������һ�������緶Χ�жϣ�
		if( dValT > dMin && dValT < dMax ){
			dVal = dValT;
			return true;
		}

		wxMessageBox( 
			wxString::Format( wxT("��������ݳ�����Χ! [ %.2f - %.2f ]"), T(dMin), T(dMax) ),
			szTitle );

	}else{
		// ������ʾ��������Ч���ݣ�
		if( !bQuite )
			wxMessageBox( wxT("�����������Ч!"), szTitle );
	}

	return false;
}


bool TxtVerifyLongRange( wxTextCtrl *pCtrl, long &iVal, long iMin, long iMax, bool bQuite=false, wxString szTitle=wxT("") );


#endif // verify_h__20090829



