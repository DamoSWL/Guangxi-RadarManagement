
/********************************************************************
	created:	2009/11/13
	created:	13:11:2009   15:48
	filename: 	E:\Work\通用驱动程序开发\GenDriver\GenDriver\InputFilter.h
	file path:	E:\Work\通用驱动程序开发\GenDriver\GenDriver
	file base:	InputFilter
	file ext:	h
	author:		王文明
	
	purpose:	wxTextCtrl 文本框输入字符过滤,可定义仅输入正浮点、正负浮点、数字、正负数字、大小写字符、16进制字符、8进制字符。
	example:	
				m_textCtrl->SetValidator( CInputFliter::Filter(CInputFliter::FILTERTYPE_FLOAT) );
*********************************************************************/

#ifndef INPUT_FILTER
#define INPUT_FILTER

class CInputFliter 
{
public:
	enum VALIDTYPE{
		FILTERTYPE_PFLOAT=0,	//仅正浮点
		FILTERTYPE_FLOAT=1,		//正负浮点
		FILTERTYPE_PNUM=2,		//正数字
		FILTERTYPE_NUM=3,		//正负数字
		FILTERTYPE_CHAR=4,		//大小写字符
		FILTERTYPE_CHAR16=5,	//16进制字符
		FILTERTYPE_CHAR8=6};		//8进制字符
	
public:
	static wxTextValidator Filter(VALIDTYPE type)
	{
		wxArrayString filterval;	
		switch(type)
		{
			case FILTERTYPE_PFLOAT://仅输入正浮点					
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxT("."));
				}				
				break;
			case FILTERTYPE_FLOAT://仅输入正负浮点
				{				
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxT("."));
					filterval.Add(wxT("-"));
				}
				break;
			case FILTERTYPE_PNUM://仅输入正数字
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
				}
				break;
			case FILTERTYPE_NUM://仅输入正负数字
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxString::Format(wxT("-")));
				}
				break;
			case FILTERTYPE_CHAR://仅输入字符
				{
					for(int i=0;i<26;i++)
					{
						filterval.Add(wxString::Format(wxT("%c"),'a'+i));
						filterval.Add(wxString::Format(wxT("%c"),'A'+i));
					}
				}
				break;
			case FILTERTYPE_CHAR16://仅输入16进制字符
				{				
					for(int i=0;i<26;i++)
					{
						filterval.Add(wxString::Format(wxT("%c"),'a'+i));
						filterval.Add(wxString::Format(wxT("%c"),'A'+i));		
					}
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
				}
				break;
			case FILTERTYPE_CHAR8://仅输入8进制字符
				{
					for(int i=0;i<8;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
				}
				break;
		}
		
		wxString tem(wxT(""));
		wxTextValidator vad(wxFILTER_INCLUDE_CHAR_LIST, &tem );
		vad.SetIncludes(filterval);

		return vad;
	}
};
#endif