#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangxiStationPanel.h"

GuangxiStationPanel::GuangxiStationPanel( wxWindow* parent): 
	GuangxiStationPackPanel( parent )
{

}

GuangxiStationPanel::~GuangxiStationPanel() 
{
}

/* 实现 IDlgParapage*/
void GuangxiStationPanel::OnApply()
{
  wxMessageBox(wxT("GRS GuangxiStationPanel"));
};

