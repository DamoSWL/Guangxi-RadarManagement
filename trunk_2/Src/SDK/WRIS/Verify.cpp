#include "../../include/WRIS/verify.h"

bool TxtVerifyLongRange( wxTextCtrl *pCtrl, long &iVal, long iMin, long iMax, bool bQuite, wxString szTitle )
{
	long iValT;
	if( pCtrl->GetValue().ToLong( &iValT ) )
	{
		// 进行下一步处理，如范围判断；
		if( iValT >= iMin && iValT <= iMax ){
			iVal = iValT;
			return true;
		}

		wxMessageBox( 
			wxString::Format( wxT("输入的数据超出范围! [ %d - %d ]"), iMin, iMax ),
			szTitle );

	}else{
		// 错误提示，输入无效数据；
		if( !bQuite )
			wxMessageBox( wxT("输入的数据无效!"), szTitle );
	}

	pCtrl->SetValue( wxString::Format( wxT("%d"), iVal) );
	return false;
}

