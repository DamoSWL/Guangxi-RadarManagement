
/********************************************************************
	created:	2009/11/13
	created:	13:11:2009   15:48
	filename: 	E:\Work\ͨ���������򿪷�\GenDriver\GenDriver\InputFilter.h
	file path:	E:\Work\ͨ���������򿪷�\GenDriver\GenDriver
	file base:	InputFilter
	file ext:	h
	author:		������
	
	purpose:	wxTextCtrl �ı��������ַ�����,�ɶ�������������㡢�������㡢���֡��������֡���Сд�ַ���16�����ַ���8�����ַ���
	example:	
				m_textCtrl->SetValidator( CInputFliter::Filter(CInputFliter::FILTERTYPE_FLOAT) );
*********************************************************************/

#ifndef INPUT_FILTER
#define INPUT_FILTER

class CInputFliter 
{
public:
	enum VALIDTYPE{
		FILTERTYPE_PFLOAT=0,	//��������
		FILTERTYPE_FLOAT=1,		//��������
		FILTERTYPE_PNUM=2,		//������
		FILTERTYPE_NUM=3,		//��������
		FILTERTYPE_CHAR=4,		//��Сд�ַ�
		FILTERTYPE_CHAR16=5,	//16�����ַ�
		FILTERTYPE_CHAR8=6};		//8�����ַ�
	
public:
	static wxTextValidator Filter(VALIDTYPE type)
	{
		wxArrayString filterval;	
		switch(type)
		{
			case FILTERTYPE_PFLOAT://������������					
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxT("."));
				}				
				break;
			case FILTERTYPE_FLOAT://��������������
				{				
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxT("."));
					filterval.Add(wxT("-"));
				}
				break;
			case FILTERTYPE_PNUM://������������
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
				}
				break;
			case FILTERTYPE_NUM://��������������
				{					
					for(int i=0;i<10;i++)
						filterval.Add(wxString::Format(wxT("%d"),i));
					filterval.Add(wxString::Format(wxT("-")));
				}
				break;
			case FILTERTYPE_CHAR://�������ַ�
				{
					for(int i=0;i<26;i++)
					{
						filterval.Add(wxString::Format(wxT("%c"),'a'+i));
						filterval.Add(wxString::Format(wxT("%c"),'A'+i));
					}
				}
				break;
			case FILTERTYPE_CHAR16://������16�����ַ�
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
			case FILTERTYPE_CHAR8://������8�����ַ�
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