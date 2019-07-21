#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangXiPanel.h"

GuangXiPanel::GuangXiPanel( wxWindow* parent): 
	GuangXiPackPanel( parent )
{

}

GuangXiPanel::~GuangXiPanel() 
{
}

/* 实现 IDlgParapage*/
void GuangXiPanel::OnApply()
{
  wxMessageBox(wxT("GRS GuangXiPanel"));
};

