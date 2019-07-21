// PluginTest.cpp : Defines the entry point for the DLL application.
//
HFONT CreatePointFontIndirect(const LOGFONTA* lpLogFont, HDC hDC )
{
	HDC hDC1 = (hDC != NULL) ? hDC : ::GetDC(NULL);
	// convert nPointSize to logical units based on hDC
	LOGFONTA logFont = *lpLogFont;
	POINT pt = { 0, 0 };
	pt.y = ::MulDiv(::GetDeviceCaps(hDC1, LOGPIXELSY), logFont.lfHeight, 720);   // 72 points/inch, 10 decipoints/point
	::DPtoLP(hDC1, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC1, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);
	if(hDC == NULL)
		::ReleaseDC(NULL, hDC1);

	return CreateFontIndirectA(&logFont);
}



HFONT CreatePointFont(int nPointSize, const char *lpszFaceName, HDC hDC, bool bBold, bool bItalic)
{
	LOGFONTA logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize;
	
	strcpy_s( logFont.lfFaceName,sizeof(CHAR)*LF_FACESIZE, lpszFaceName );

	if(bBold)
		logFont.lfWeight = FW_BOLD;
		
	if(bItalic)
		logFont.lfItalic = (BYTE)TRUE;

	return CreatePointFontIndirect(&logFont, hDC);
}


