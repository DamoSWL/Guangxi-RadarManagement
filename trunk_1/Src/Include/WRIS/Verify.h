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
		// 进行下一步处理，如范围判断；
		if( dValT > dMin && dValT < dMax ){
			dVal = dValT;
			return true;
		}

		wxMessageBox( 
			wxString::Format( wxT("输入的数据超出范围! [ %.2f - %.2f ]"), T(dMin), T(dMax) ),
			szTitle );

	}else{
		// 错误提示，输入无效数据；
		if( !bQuite )
			wxMessageBox( wxT("输入的数据无效!"), szTitle );
	}

	return false;
}


bool TxtVerifyLongRange( wxTextCtrl *pCtrl, long &iVal, long iMin, long iMax, bool bQuite=false, wxString szTitle=wxT("") );


#endif // verify_h__20090829



