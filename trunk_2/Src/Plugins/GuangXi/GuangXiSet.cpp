#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangXiSet.h"

#define wxID_LISTCRTL wxID_HIGHEST

BEGIN_EVENT_TABLE(GuangXiSet,wxEvtHandler)
	EVT_LIST_ITEM_SELECTED(wxID_LISTCRTL,GuangXiSet::onSelectedItem)
END_EVENT_TABLE()


GuangXiSet::GuangXiSet( wxWindow* parent): 
	  GuangXiPanelSet( parent ),m_selectedItemIndex(0) 
{
	init();
}

GuangXiSet::~GuangXiSet()
{

};

void GuangXiSet::OnApply()
{

}

/* 重载自IDlgParapage*/
int GuangXiSet::OnCheckApply()
{
	fileDirConfig dirConfig;
	dirConfig.JishuDir = m_textCtrlJishu->GetValue();
	dirConfig.ProductDir = m_textCtrlErchi->GetValue();
	dirConfig.ZhuangtaiDir = m_textCtrlZhuangtai->GetValue();
	dirConfig.PingtuDir = m_textCtrlPingtu->GetValue();
	dirConfig.BaojingDir = m_textCtrlBaojing->GetValue();


	if(!dirConfig.JishuDir.IsEmpty())
	{
		if(!::wxDirExists(dirConfig.JishuDir))
		{
			::wxMessageBox(wxT("基数据文件目录异常，请重新选择目录"),wxT("报警"));
			return EXIT_FAILURE;
		}
	}

	if(!dirConfig.ProductDir.IsEmpty())
	{
		if(!::wxDirExists(dirConfig.ProductDir))
		{
			::wxMessageBox(wxT("产品文件目录异常，请重新选择目录"),wxT("报警"));
			return EXIT_FAILURE;
		}
	}

	if(!dirConfig.ZhuangtaiDir.IsEmpty())
	{
		if(!::wxDirExists(dirConfig.ZhuangtaiDir))
		{
			::wxMessageBox(wxT("状态文件目录异常，请重新选择目录"),wxT("报警"));
			return EXIT_FAILURE;
		}
	}

	if(!dirConfig.PingtuDir.IsEmpty())
	{
		if(!::wxDirExists(dirConfig.PingtuDir))
		{
			::wxMessageBox(wxT("拼图文件目录异常，请重新选择目录"),wxT("报警"));
			return EXIT_FAILURE;
		}
	}

	if(!dirConfig.BaojingDir.IsEmpty())
	{
		if(!::wxDirExists(dirConfig.BaojingDir))
		{
			::wxMessageBox(wxT("报警文件目录异常，请重新选择目录"),wxT("报警"));
			return EXIT_FAILURE;
		}
	}

	GetPlug()->setFileDirConfig(dirConfig);


	FTPDirConfig ftpDir;

	for(auto index = 0; index < m_listCtrl_Ftp->GetItemCount(); index++)
	{
		auto fileType = m_listCtrl_Ftp->GetItemData(index);

		if((fileType < 1) || (fileType > 4))
		{
			::wxMessageBox(wxT("FTP文件类型值异常,不在1-4范围内，请重新设置"),wxT("错误"));
			return EXIT_FAILURE;
		}
		
		switch(fileType)
		{
			case 1:
				ftpDir.FTPJishuDir = m_listCtrl_Ftp->GetItemText(index);
			break;

			case 2:
				ftpDir.FTPProductDir = m_listCtrl_Ftp->GetItemText(index);
			break;

			case 3:
				ftpDir.FTPZhuangtaiDir = m_listCtrl_Ftp->GetItemText(index);
			break;

			case 4:
				ftpDir.FTPPintuDir = m_listCtrl_Ftp->GetItemText(index);
			break;
			
			default:
			break;
		}
	}

	GetPlug()->setFTPDirConfig(ftpDir);
	return EXIT_SUCCESS;
};



void GuangXiSet::init()
{
	m_JishuButtonID = m_buttonJishu->GetId();
	m_ErchiButtonID = m_buttonErchi->GetId();
	m_ZhuangtaiButtonID = m_buttonZhuangtai->GetId();
	m_PingtuID =  m_buttonPingtu->GetId();
	m_BaojingButtonID = m_buttonBaojing->GetId();

	m_FtpAddButtonID = m_buttonFtpAdd->GetId();
	m_FtpDelButtonID = m_buttonFtpDel->GetId();
	m_FtpTypeButtonID = m_buttonFtpType->GetId();

	auto dirConfig = GetPlug()->getFileDirConfig();

	m_textCtrlJishu->SetValue(dirConfig.JishuDir);
	m_textCtrlErchi->SetValue(dirConfig.ProductDir);
	m_textCtrlZhuangtai->SetValue(dirConfig.ZhuangtaiDir);
	m_textCtrlBaojing->SetValue(dirConfig.BaojingDir);
	m_textCtrlPingtu->SetValue(dirConfig.PingtuDir);

	m_listCtrl_Ftp->InsertColumn(0,wxT("FTP日志目录"),wxLIST_FORMAT_CENTER,415);
	m_listCtrl_Ftp->InsertColumn(1,wxT("FTP日志类型"),wxLIST_FORMAT_CENTER,100);

	auto ftpDir = GetPlug()->getFTPDirConfig();
	UINT index = 0;

	if(!ftpDir.FTPJishuDir.IsEmpty())
	{
		m_listCtrl_Ftp->InsertItem(index,ftpDir.FTPJishuDir);
		m_listCtrl_Ftp->SetItemData(index,1);
		m_listCtrl_Ftp->SetItem(index,1,wxT("1"));
		index++;
	}
	if(!ftpDir.FTPProductDir.IsEmpty())
	{
		m_listCtrl_Ftp->InsertItem(index,ftpDir.FTPProductDir);
		m_listCtrl_Ftp->SetItemData(index,2);
		m_listCtrl_Ftp->SetItem(index,1,wxT("2"));
		index++;
	}
	if(!ftpDir.FTPZhuangtaiDir.IsEmpty())
	{
		m_listCtrl_Ftp->InsertItem(index,ftpDir.FTPZhuangtaiDir);
		m_listCtrl_Ftp->SetItemData(index,3);
		m_listCtrl_Ftp->SetItem(index,1,wxT("3"));
		index++;
	}
	if(!ftpDir.FTPPintuDir.IsEmpty())
	{
		m_listCtrl_Ftp->InsertItem(index,ftpDir.FTPPintuDir);
		m_listCtrl_Ftp->SetItemData(index,4);
		m_listCtrl_Ftp->SetItem(index,1,wxT("4"));
		index++;
	}	

	Connect(m_JishuButtonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuangXiSet::getDir));
	Connect(m_ErchiButtonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuangXiSet::getDir));
	Connect(m_ZhuangtaiButtonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuangXiSet::getDir));
	Connect(m_PingtuID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuangXiSet::getDir));
	Connect(m_BaojingButtonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GuangXiSet::getDir));

	Connect(m_FtpAddButtonID,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(GuangXiSet::getFtpDir));
	Connect(m_FtpDelButtonID,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(GuangXiSet::delFtpDir));
	Connect(m_FtpTypeButtonID,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(GuangXiSet::getFtpType));
	
}

void GuangXiSet::getDir(wxCommandEvent& event)
{
	wxString path;
	wxDirDialog dialog(this,wxT("选择文件路径"),wxT("."),wxDD_NEW_DIR_BUTTON);
	dialog.Center();
	if(dialog.ShowModal() == wxID_OK)
	{
		path = dialog.GetPath();
		auto id = event.GetId();
		if(id == m_JishuButtonID)
		{
			m_textCtrlJishu->SetValue(path);
		}
		else if(id == m_ErchiButtonID)
		{
			m_textCtrlErchi->SetValue(path);
		}
		else if(id == m_ZhuangtaiButtonID)
		{
			m_textCtrlZhuangtai->SetValue(path);
		}
		else if(id == m_PingtuID)
		{
			m_textCtrlPingtu->SetValue(path);
		}
		else if(id == m_BaojingButtonID)
		{
			m_textCtrlBaojing->SetValue(path);
		}
		else{}
		
	}
}

void GuangXiSet::getFtpDir(wxCommandEvent& event)
{
	wxString path;
	wxDirDialog dialog(this,wxT("选择文件路径"),wxT("."),wxDD_NEW_DIR_BUTTON);
	dialog.Center();
	if(dialog.ShowModal() == wxID_OK)
	{
		path = dialog.GetPath();
		auto id = event.GetId();
		if(id == m_FtpAddButtonID)
		{
			auto totalNum = m_listCtrl_Ftp->GetItemCount();
			m_listCtrl_Ftp->InsertItem(totalNum,path);
			m_listCtrl_Ftp->SetItemData(totalNum,1);
			m_listCtrl_Ftp->SetItem(totalNum,1,wxT("1"));
		}
	}

	
}

void GuangXiSet::delFtpDir(wxCommandEvent& event)
{
	if(m_selectedItemIndex >= 0)
	{
		m_listCtrl_Ftp->DeleteItem(m_selectedItemIndex);
	}

}

void GuangXiSet::getFtpType(wxCommandEvent& event)
{
	auto fileType = m_textCtrlFtpType->GetValue();
	long typeNum = 0;
	fileType.ToLong(&typeNum);
	if((typeNum < 1) || (typeNum > 4))
	{
		::wxMessageBox(wxT("文件类型输入错误，请按要求输入正确类型值"),wxT("错误"));
		return;
	}
	else
	{
		m_listCtrl_Ftp->SetItem(m_selectedItemIndex,1,fileType);
		m_listCtrl_Ftp->SetItemData(m_selectedItemIndex,typeNum);
	}
}

void GuangXiSet::onSelectedItem(wxListEvent& event)
{
	m_selectedItemIndex = event.GetIndex();
}

