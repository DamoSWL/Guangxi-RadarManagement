#ifndef grsgdi_h__
#define grsgdi_h__


HFONT CreatePointFontIndirect(const LOGFONTA* lpLogFont, HDC hDC = NULL);
HFONT CreatePointFont(int nPointSize, const char *lpszFaceName, HDC hDC = NULL, bool bBold = false, bool bItalic = false);


#endif // grsgdi_h__

