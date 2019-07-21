#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangxiStationSet.h"


GuangxiStationSet::GuangxiStationSet( wxWindow* parent): 
	  GuangxiStationPanelSet( parent )
{
	
	init();
}

GuangxiStationSet::~GuangxiStationSet()
{

};

/* ÷ÿ‘ÿ◊‘IDlgParapage*/
void GuangxiStationSet::OnApply()
{

};

int GuangxiStationSet::OnCheckApply()
{
	std::map<std::string,std::string> computersIP;
	computersIP["RPG"] = std::string(m_panel_RPGIP->GetValue().mb_str(wxConvLocal));
	computersIP["PUP"] = std::string(m_panel_PUPIP->GetValue().mb_str(wxConvLocal));
	computersIP["PUP2"] = std::string(m_panel_PUP2IP->GetValue().mb_str(wxConvLocal));
	GetPlug()->setComputersIP(computersIP);

	return 0;
}

void GuangxiStationSet::init()
{
	auto computersIP = GetPlug()->getComputersIP();
	for(auto item = computersIP.cbegin(); item != computersIP.cend(); item++)
	{
		if(item->first == "RPG")
		{
			m_panel_RPGIP->SetValue(wxString(item->second.c_str(),wxConvLocal));
		}
		else if(item->first == "PUP")
		{
			m_panel_PUPIP->SetValue(wxString(item->second.c_str(),wxConvLocal));
		}
		else if(item->first == "PUP2")
		{
			m_panel_PUP2IP->SetValue(wxString(item->second.c_str(),wxConvLocal));
		}

	}
}

