#ifndef NEWINPUT_FILTER_20130328
#define NEWINPUT_FILTER_20130328

#include "wx/window.h"
#include <vector>
#include <wx/regex.h>
/********************************************************************
	created:	2013/3/28
	filename:	InputFilter.h
	file base:	CNewInputFliter
	file ext:	h
	author:		wzq
	purpose:	wxTextCtrl �ı��������ַ����ˡ�
	�ɶ��������
	�������㡢�����㡢�����㣻��������������������������
	��Сд�ַ���16�����ַ������볤�ȡ�
	ʹ�÷�����
	1.����һ��CNewInputFliter�Ķ���"m_InputFliter";(�����ھֲ�������
	2.������Ҫ���˵Ŀؼ���
	m_InputFliter.InputFilterConnect(m_textCtrl,FILTERTYPE_PFLOAT,3,0,2000,LeftOpenRightClosedInterval);
*********************************************************************/
using namespace std;
#define REGEXSTRLENGTH 100
enum VALIDTYPE{
	FILTERTYPE_FLOAT=0,				//��������
	FILTERTYPE_PFLOAT,				//������
	FILTERTYPE_NFLOAT,				//������
	FILTERTYPE_NUM,					//��������
	FILTERTYPE_PNUM,				//������
	FILTERTYPE_NNUM,				//������
	FILTERTYPE_CHAR,				//��Сд�ַ�
	FILTERTYPE_CHAR16,				//16�����ַ�
	FILTERTYPE_LONGLIMIT			//��������
	//FILTERTYPE_CHAR8=6
};
enum INTERVALTYPE{
	AllClosedInterval = 0,			//ȫ��
	AllOpenInterval,				//ȫ��
	LeftOpenRightClosedInterval,	//���ұ�����
	LeftClosedRightOpenInterval	//����ҿ�����
};
typedef struct _IDconnectTYPE_
{
	wxWindowID IDNum;				//�ؼ�ID
	wxTextCtrl *pTextCtrl;			//�ؼ�ָ��
	VALIDTYPE type;					//��������
	char RegExstr[REGEXSTRLENGTH];	//������ʽ
	size_t InputMaxLong;			//���볤������
	int FloatLong;					//�������
	double MinValue;				//��Сֵ
	double MaxValue;				//���ֵ
	INTERVALTYPE Interval;			//��������
}tagIDconnectTYPE;

class CNewInputFliter : public wxEvtHandler
{
public:
	CNewInputFliter(){m_CurID = -1;m_verIDconnectTYPE.clear();m_pCurTextCtrl = NULL;}
	~CNewInputFliter(){m_verIDconnectTYPE.clear();m_pCurTextCtrl = NULL;}

	//************************************
	// Method:    InputFilterConnect
	// FullName:  CNewInputFliter::InputFilterConnect
	// Access:    public 
	// Returns:   void
	// Qualifier: �����ı����������ͺͷ�Χ
	// Parameter: wxTextCtrl * pTextCtrl		//������Ҫע��Ŀؼ�
	// Parameter: const VALIDTYPE type			//��������
	// Parameter: const unsigned int FloatLong	//����С�����λ������������Ч��
	// Parameter: const double MinValue			//��Сֵ
	// Parameter: const double MaxValue			//���ֵ
	// Parameter: const INTERVALTYPE Interval	//��������
	//************************************
	void InputFilterConnect( wxTextCtrl *pTextCtrl,const VALIDTYPE type,const unsigned int FloatLong,const double MinValue,const double MaxValue,const INTERVALTYPE Interval)
	{
		assert(pTextCtrl!=NULL);
		if (pTextCtrl==NULL)
		{
			return;
		}
		tagIDconnectTYPE IDconTYPE;
		memset(&IDconTYPE,0,sizeof(tagIDconnectTYPE));
		IDconTYPE.IDNum = pTextCtrl->GetId();
		IDconTYPE.pTextCtrl = pTextCtrl;
		IDconTYPE.type = type;
		IDconTYPE.InputMaxLong = 0;
		assert(MinValue <= MaxValue);			//���뷴��������ʾ
		if (MinValue < MaxValue)//��ֹ�������
		{
			IDconTYPE.MinValue = MinValue;
			IDconTYPE.MaxValue = MaxValue;
		}
		else
		{
			IDconTYPE.MinValue = MaxValue;
			IDconTYPE.MaxValue = MinValue;
		}
		if (FloatLong<=10)				//С��������10λ
		{
			IDconTYPE.FloatLong = FloatLong;
		}
		else IDconTYPE.FloatLong = 10;
		
		IDconTYPE.Interval = Interval;
		SetRegExStr(IDconTYPE.RegExstr,type);
		m_verIDconnectTYPE.push_back(IDconTYPE);
		pTextCtrl->Connect( wxEVT_CHAR, wxKeyEventHandler( CNewInputFliter::OnChar ), NULL, this );
		pTextCtrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( CNewInputFliter::OnKillFocus ), NULL, this );
		pTextCtrl->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( CNewInputFliter::OnSetFocus ), NULL, this );
		//pTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CNewInputFliter::OnCharUpdate ), NULL, this );
	}

	//************************************
	// Method:    InputFilterConnect
	// FullName:  CNewInputFliter::InputFilterConnect
	// Access:    public 
	// Returns:   void
	// Qualifier: ֻ�����ı��ĳ���
	// Parameter: wxTextCtrl * pTextCtrl		//������Ҫע��Ŀؼ�
	// Parameter: const size_t InputMaxLong		//��󳤶�
	// Parameter: const VALIDTYPE type			//Ĭ�ϳ�������
	//************************************
	void InputFilterConnect( wxTextCtrl *pTextCtrl,const size_t InputMaxLong,const VALIDTYPE type = FILTERTYPE_LONGLIMIT)
	{
		assert(pTextCtrl!=NULL);
		if (pTextCtrl==NULL)
		{
			return;
		}
		tagIDconnectTYPE IDconTYPE;
		memset(&IDconTYPE,0,sizeof(tagIDconnectTYPE));
		IDconTYPE.IDNum = pTextCtrl->GetId();
		IDconTYPE.pTextCtrl = pTextCtrl;
		IDconTYPE.type = type;
		IDconTYPE.InputMaxLong = InputMaxLong;
		IDconTYPE.MinValue = 0;
		IDconTYPE.MaxValue = 0;
		IDconTYPE.FloatLong = 0;
		SetRegExStr(IDconTYPE.RegExstr,type);
		IDconTYPE.Interval = AllClosedInterval;
		m_verIDconnectTYPE.push_back(IDconTYPE);
		pTextCtrl->Connect( wxEVT_CHAR, wxKeyEventHandler( CNewInputFliter::OnChar ), NULL, this );
		pTextCtrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( CNewInputFliter::OnKillFocus ), NULL, this );
		pTextCtrl->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( CNewInputFliter::OnSetFocus ), NULL, this );
		//���ڲ���Ҫ��ɫ�����ѣ�ȡ�����¼�
		//pTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CNewInputFliter::OnCharUpdate ), NULL, this );
	}
	//************************************
	// Method:    ChangeInputRangeLimit
	// FullName:  CNewInputFliter::ChangeInputRangeLimit
	// Access:    public 
	// Returns:   bool
	// Qualifier: ע����޸��ı�����ֵ�ķ�Χ
	// Parameter: const wxTextCtrl * pTextCtrl	//������Ҫע��Ŀؼ�
	// Parameter: const unsigned int FloatLong	//����С�����λ��
	// Parameter: const float MinValue			//��Сֵ
	// Parameter: const float MaxValue			//���ֵ
	// Parameter: const INTERVALTYPE Interval	//��������
	//************************************
	bool ChangeInputRangeLimit(const wxTextCtrl *pTextCtrl,const unsigned int FloatLong,const float MinValue,const float MaxValue,const INTERVALTYPE Interval)
	{
		if (m_verIDconnectTYPE.size()==0)
		{
			return false;
		}
		if (pTextCtrl==NULL)
		{
			return false;
		}
		vector<tagIDconnectTYPE>::iterator vecCurPos = m_verIDconnectTYPE.begin();
		while( vecCurPos!=m_verIDconnectTYPE.end() && vecCurPos->pTextCtrl != pTextCtrl ) vecCurPos++;
		if (vecCurPos==m_verIDconnectTYPE.end())
		{
			return false;
		}
		if (vecCurPos->IDNum!=pTextCtrl->GetId())
		{
			return false;
		}
		assert(MinValue <= MaxValue);			//���뷴��������ʾ
		if (MinValue < MaxValue)//��ֹ�������
		{
			vecCurPos->MinValue = MinValue;
			vecCurPos->MaxValue = MaxValue;
		}
		else
		{
			vecCurPos->MinValue = MaxValue;
			vecCurPos->MaxValue = MinValue;
		}
		if (FloatLong<=10)				//С��������10λ
		{
			vecCurPos->FloatLong = FloatLong;
		}
		else vecCurPos->FloatLong = 10;
		vecCurPos->FloatLong = FloatLong;
		vecCurPos->Interval = Interval;
		return true;
	}
	//************************************
	// Method:    DisInputConnect
	// FullName:  CNewInputFliter::DisInputConnect
	// Access:    public 
	// Returns:   void
	// Qualifier: ɾ�������¼�
	// Parameter: wxTextCtrl * TextCtrl������Ҫ��ע��Ŀؼ�
	//************************************
	void DisInputConnect(wxTextCtrl *pTextCtrl)
	{
		assert(pTextCtrl!=NULL);
		if (pTextCtrl == NULL)
		{
			return;
		}
		vector<tagIDconnectTYPE>::iterator vecCurPos = m_verIDconnectTYPE.begin();
		while( vecCurPos!=m_verIDconnectTYPE.end() && vecCurPos->pTextCtrl != pTextCtrl ) vecCurPos++;
		if (vecCurPos==m_verIDconnectTYPE.end())
		{
			return;
		}
		if (vecCurPos->IDNum!=pTextCtrl->GetId())
		{
			return;
		}
		m_verIDconnectTYPE.erase(vecCurPos);
		pTextCtrl->Disconnect( wxEVT_CHAR, wxKeyEventHandler( CNewInputFliter::OnChar ), NULL, this );
		pTextCtrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( CNewInputFliter::OnKillFocus ), NULL,this );
		pTextCtrl->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( CNewInputFliter::OnSetFocus ),NULL,this );
		//���ڲ���Ҫ��ɫ�����ѣ�ȡ�����¼�
		//pTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CNewInputFliter::OnCharUpdate ), NULL, this );

	}
protected:
	//************************************
	// Method:    OnSetFocus
	// FullName:  CNewInputFliter::OnSetFocus
	// Access:    public 
	// Returns:   void
	// Qualifier: ����������ȷֵ
	// Parameter: wxFocusEvent & event
	//************************************
	void OnSetFocus( wxFocusEvent& event )
	{
		//m_bCheck = false;
		m_CurID = event.GetId();
		m_pCurTextCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(m_CurID);		//���Ҷ�Ӧ�Ŀؼ�

		m_vecCurPos = m_verIDconnectTYPE.begin();
		assert(m_vecCurPos != m_verIDconnectTYPE.end());
		while(m_vecCurPos!=m_verIDconnectTYPE.end() && m_vecCurPos->IDNum != m_CurID ) m_vecCurPos++;
		if (m_vecCurPos == m_verIDconnectTYPE.end() || m_pCurTextCtrl!=m_vecCurPos->pTextCtrl)
		{
			if (m_vecCurPos->pTextCtrl->GetId() == m_CurID)		//ID��ͬ
			{
				m_pCurTextCtrl = m_vecCurPos->pTextCtrl;
			}
			else
			{
				m_pCurTextCtrl = NULL;
				return;
			}
		}
		m_sOldValue = m_pCurTextCtrl->GetValue();
		event.Skip();
	}

	//************************************
	// Method:    OnKillFocus
	// FullName:  CNewInputFliter::OnKillFocus
	// Access:    public 
	// Returns:   void
	// Qualifier:	���㳷�������ж�������ȷ���
	// Parameter: wxFocusEvent & event
	//************************************
	void OnKillFocus( wxFocusEvent& event )
	{
		if (m_pCurTextCtrl==NULL)
		{
			return;
		}
		if (m_CurID!=event.GetId())
		{
			return;
		}
		/*if (m_bCheck)			//��ɫ����Ҫ��ԭ
		{
			m_pCurTextCtrl->SetValue(m_sOldValue);
			m_pCurTextCtrl->SetBackgroundColour(RGB(255,255,255));
			m_pCurTextCtrl->Refresh();
		}*/
		//ȥ��ǰ��0
		if (m_vecCurPos->type == FILTERTYPE_PNUM || m_vecCurPos->type == FILTERTYPE_NNUM||
			m_vecCurPos->type == FILTERTYPE_NUM)			//��ֹ����0001
		{
			long ivle=0 ;
			if (m_pCurTextCtrl->GetValue().ToLong(&ivle))
			{
				wxString tp = wxString::Format(wxT("%d"),ivle);
				m_pCurTextCtrl->SetValue(tp);
			}			
		}
		if (m_vecCurPos->type == FILTERTYPE_PFLOAT || m_vecCurPos->type == FILTERTYPE_FLOAT ||
			m_vecCurPos->type == FILTERTYPE_NFLOAT)			//��ֹ����0001
		{
			double dv=0;
			if (m_pCurTextCtrl->GetValue().ToDouble(&dv))
			{
				wxString Ft=wxT("%") + wxString::Format(wxT(".%df"),m_vecCurPos->FloatLong);
				wxString tp = wxString::Format(Ft,dv);
				m_pCurTextCtrl->SetValue(tp);
			}
		}
		//��ֹ����+
		if (m_vecCurPos->type == FILTERTYPE_PFLOAT || m_vecCurPos->type == FILTERTYPE_FLOAT||
			m_vecCurPos->type == FILTERTYPE_PNUM || m_vecCurPos->type == FILTERTYPE_NUM)		//��ֹ����+123.1
		{
			wxString tmp = m_pCurTextCtrl->GetValue();
			if ( tmp.Find('+')!=-1 )
			{
				wxString tp = tmp.After('+');
				m_pCurTextCtrl->SetValue(tp);
			}
		}
		
		bool IsReValue = false;

		if (m_vecCurPos->type != FILTERTYPE_LONGLIMIT)
		{
			//�������
			if (!InputRegEx(m_pCurTextCtrl->GetValue(),m_vecCurPos->RegExstr))
			{
				IsReValue = true;
			}
			//��Χ����
			if (!CheckRange(m_pCurTextCtrl->GetValue(),true))
			{
				IsReValue = true;
			}
		}
		else
		{
			//�������
			wxString str = m_pCurTextCtrl->GetValue();
			if ( str.Contains(wxT("%")) )
			{
				IsReValue = true;
			}
			else if (InputRegEx(str,m_vecCurPos->RegExstr))	//����������ŷ���True
			{
				IsReValue = true;
			}
			else if ( !CheckInputLong(str) )//�������ƣ���Ҫ�ж�
			{
				IsReValue = true;
			}
		}
		if (IsReValue)
		{
			m_pCurTextCtrl->SetValue(m_sOldValue);
		}
		
		m_pCurTextCtrl = NULL;
		m_CurID = -1;
		event.Skip();
	}
	//************************************
	// Method:    KeyFilter
	// FullName:  CNewInputFliter::KeyFilter
	// Access:    public 
	// Returns:   bool
	// Qualifier:���˼����¼�
	// Parameter: const int keycode
	//************************************
	bool KeyFilter(const int keycode)
	{
		if (m_vecCurPos->type == FILTERTYPE_NUM ||					//���������㣬ʮ����������������
			m_vecCurPos->type == FILTERTYPE_PNUM||
			m_vecCurPos->type == FILTERTYPE_NNUM||
			m_vecCurPos->type ==FILTERTYPE_FLOAT||
			m_vecCurPos->type ==FILTERTYPE_PFLOAT||
			m_vecCurPos->type ==FILTERTYPE_NFLOAT||
			m_vecCurPos->type == FILTERTYPE_CHAR16
			)
		{
			if (keycode>='0' && keycode<='9')
			{
				return true;
			}
			if (keycode>=WXK_NUMPAD0 && keycode<=WXK_NUMPAD9)
			{
				return true;
			}
		}
		if(m_vecCurPos->type ==FILTERTYPE_PFLOAT||					//��������.
			m_vecCurPos->type ==FILTERTYPE_NFLOAT||
			m_vecCurPos->type ==FILTERTYPE_FLOAT)
		{
			if (keycode == '.')
				return true;
		}
		if (m_vecCurPos->type == FILTERTYPE_CHAR)							//�ַ�
		{
			if (wxIsalpha(keycode))
			{
				return true;
			}
		}
		if (m_vecCurPos->type == FILTERTYPE_CHAR16)						//ʮ������X
		{
			if (keycode == 'x'|| keycode == 'X'||
				(keycode >= 'a' && keycode <= 'f')||
				(keycode >= 'A' && keycode <= 'F'))
			{
				return true;
			}
		}
		if (m_vecCurPos->type == FILTERTYPE_PNUM ||						//+�����ж�
			m_vecCurPos->type ==FILTERTYPE_PFLOAT||
			m_vecCurPos->type ==FILTERTYPE_FLOAT||
			m_vecCurPos->type ==FILTERTYPE_NUM
			)
		{
			if (keycode == '+')
			{
				return true;
			}
		}
		if (m_vecCurPos->type == FILTERTYPE_NNUM||						//-�����ж�
			m_vecCurPos->type ==FILTERTYPE_NFLOAT||
			m_vecCurPos->type ==FILTERTYPE_FLOAT||
			m_vecCurPos->type ==FILTERTYPE_NUM
			)
		{
			if (keycode == '-')
			{
				return true;
			}
		}
		if (keycode>=WXK_F1 && keycode<=WXK_F24)
		{
			return true;
		}
		return false;
	}
	//************************************
	// Method:    OnChar
	// FullName:  CNewInputFliter::OnChar
	// Access:    public 
	// Returns:   void
	// Qualifier: �����������
	// Parameter: wxKeyEvent & event
	//************************************
	void OnChar( wxKeyEvent& event )
	{
		if (m_pCurTextCtrl==NULL)
		{
			return;
		}
		if (m_CurID!=event.GetId())
		{
			return;
		}
		
		int KeyCode = event.GetKeyCode();
		switch (KeyCode)								//���ð���
		{
		case 1:						//Ctrl+A
		case 3:						//Ctrl+C
		case 22:					//Ctrl+V
		case 24:					//Ctrl+X
		case 26:					//Ctrl+Z
		case WXK_SHIFT:
		case WXK_ALT:
		case WXK_PAUSE:
		case WXK_PRINT:
		case WXK_HELP:
		case WXK_INSERT:
		case WXK_CLEAR:
		case WXK_BACK :
		case WXK_LEFT :
		case WXK_RIGHT :
		case WXK_TAB :
		case WXK_DELETE :
		case WXK_END :
		case WXK_HOME :
		case WXK_NUMLOCK:
		case WXK_NUMPAD_DECIMAL:
			event.Skip(true);
			return;
		default:break;
		}
		wxString curstr = m_pCurTextCtrl->GetValue();
		if (!m_pCurTextCtrl->GetStringSelection().IsEmpty())
		{
			long From=0,To=0;
			m_pCurTextCtrl->GetSelection(&From,&To);
			curstr.Remove(From,(To-From));
		}
		//��ֹ�ظ�����".""-""+"
		if (( ( KeyCode == '.') && (curstr.Contains(wxT(".")) )) || 
			( ( KeyCode == '-') && (curstr.Contains(wxT("-")) )) ||
			( ( KeyCode == '+') && (curstr.Contains(wxT("+")) ))  )
		{
			event.Skip(false);
			return;
		}
		curstr.insert(m_pCurTextCtrl->GetInsertionPoint(),1,event.GetUnicodeKey());
		//��ȡ���ݴ�С
		if (m_vecCurPos->type == FILTERTYPE_LONGLIMIT)
		{
			if(KeyCode == '%' || !CheckInputLong(curstr))
			{
				event.Skip(false);
			}
			else event.Skip(true);
			return;
		}
		if (KeyFilter(KeyCode))
		{
			if (CheckRange(curstr,false))
			{
				event.Skip(true);
				return;
			}
		}
		event.Skip(false);
	}
	//************************************
	// Method:    OnCharUpdate
	// FullName:  CNewInputFliter::OnCharUpdate
	// Access:    public 
	// Returns:   void
	// Qualifier: �ı������¼�����  //���ڲ���Ҫ��ɫ�����ѣ�ȡ�����¼�
	// Parameter: wxCommandEvent & event
	//************************************
	void OnCharUpdate(wxCommandEvent& event )
	{
		//if (m_pCurTextCtrl==NULL)
		//{
		//	return;
		//}
		//long id = event.GetId();								//���Ҷ�Ӧ�Ŀؼ�
		//if (id!=m_CurID)
		//{
		//	m_CurID = id;
		//	m_pCurTextCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(m_CurID);
		//	m_vecCurPos = m_verIDconnectTYPE.begin();
		//	while(m_vecCurPos->IDNum != m_CurID && m_vecCurPos<m_verIDconnectTYPE.end()) m_vecCurPos++;
		//}
		//if (m_pCurTextCtrl!=m_vecCurPos->pTextCtrl)
		//{
		//	return;
		//}
		//if (InputRegEx(event.GetString(),m_vecCurPos->RegExstr))
		//{
		//	//m_pCurTextCtrl->SetBackgroundColour(RGB(255,255,255));
		//	m_pCurTextCtrl->Refresh();
		//	m_bCheck =false;
		//}
		//else
		//{
		//	//m_pCurTextCtrl->SetBackgroundColour(RGB(255,190,190));
		//	m_pCurTextCtrl->Refresh();
		//	m_bCheck = true;
		//}
		event.Skip(false);
	}
	//************************************
	// Method:    InputRegEx
	// FullName:  CNewInputFliter::InputRegEx
	// Access:    public 
	// Returns:   bool
	// Qualifier: ������ʽ����
	// Parameter: const wxString inputstr
	// Parameter: const VALIDTYPE type
	//************************************
	bool InputRegEx(const wxString &inputstr,const char *Pattern)
	{
		if (inputstr.IsEmpty())
		{
			return true;
		}
		wxString wxstr(Pattern,wxConvLocal);
		if (!wxstr.IsEmpty() && m_CurEx.Compile(wxstr))				//��ֹ��ģ�� ƥ������
		{
			if ( m_CurEx.Matches(inputstr) )
			{
				return true;
			}
		}
		return false;
	}

	void SetRegExStr(char *cstr , const VALIDTYPE type)
	{
		switch(type)
		{
		case FILTERTYPE_NUM:							//��������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-+]?[0-9]+$");
				break;
			}
		case FILTERTYPE_PNUM:							//������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[+]?[0-9]+$");
				break;
			}
		case FILTERTYPE_NNUM:							//������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-]{1,1}[0-9]+$");
				break;
			}
		case FILTERTYPE_CHAR:							//�ַ�
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^^[A-Za-z]+$");
				break;
			}
		case FILTERTYPE_FLOAT:							//��������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-+]?[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_PFLOAT:							//������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[+]?[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_NFLOAT:							//������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-]{1,1}[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_CHAR16:							//ʮ������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^0[xX][0-9a-fA-F]+$");
				break;
			}
		case FILTERTYPE_LONGLIMIT:						//��������
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"[`~!@#$^&*()=|\':;,<>/?��@#������&*��������|������������������������]");
				break;
			}
			//case FILTERTYPE_CHAR8:							//�˽���
			//	{
			//		CurPattern = wxT("^\\n+$");
			//		break;
			//	}
		default:
			break;
		}
		return ;
	}
	bool CheckRange(const wxString &str,bool Tpye)
	{
		if (m_vecCurPos->type == FILTERTYPE_PFLOAT || m_vecCurPos->type == FILTERTYPE_PNUM ||
			m_vecCurPos->type == FILTERTYPE_NFLOAT || m_vecCurPos->type == FILTERTYPE_NNUM ||
			m_vecCurPos->type == FILTERTYPE_FLOAT  || m_vecCurPos->type == FILTERTYPE_NUM )		//��Χ����
		{
			const double fzero = 0.000001;
			bool bmax =!(m_vecCurPos->MaxValue<fzero && m_vecCurPos->MaxValue>-fzero);
			bool bmin =!(m_vecCurPos->MinValue<fzero && m_vecCurPos->MinValue>-fzero);
			if (bmax || bmin)
			{
				double MinValue = m_vecCurPos->MinValue;
				double MaxValue = m_vecCurPos->MaxValue;
				bool bReCheck = false;
				double ftmp = 0;
				bool bCov = str.ToDouble(&ftmp);
				if (!Tpye)						//	�������ļ��
				{
					if (ftmp<fzero && ftmp>-fzero)	//������Ϊ0��ʱ�򣬷�ֹ�����䷵��Ϊ����
					{
						return true;
					}
					if (ftmp>0)					//������
					{
						MinValue = 0;
					}
					else						//������
					{
						MaxValue = 0;
					}
				}
				else if (!bCov)			//����⣬���������
				{
					return false;
				}

				switch(m_vecCurPos->Interval)
				{
				case AllClosedInterval:
					{
						if (ftmp>=MinValue && ftmp<=MaxValue)
							bReCheck = true;
						break;
					}
				case AllOpenInterval:
					{
						if (ftmp>MinValue && ftmp<MaxValue)
							bReCheck = true;
						break;
					}
				case LeftOpenRightClosedInterval:
					{
						if (ftmp>MinValue && ftmp<=MaxValue)
							bReCheck = true;
						break;
					}
				case LeftClosedRightOpenInterval:
					{
						if (ftmp>=MinValue && ftmp<MaxValue)
							bReCheck = true;
						break;
					}
				default:
					break;
				}
				return bReCheck;
				
			}
		}
		return true;
	}
	//************************************
	// Method:    CheckInputLong
	// FullName:  CNewInputFliter::CheckInputLong
	// Access:    private 
	// Returns:   bool
	// Qualifier: ��ȡ���볤������
	//************************************
	bool CheckInputLong(const wxString &instr)
	{
		string str(instr.mb_str());
		if ( strlen(str.c_str())+1 > m_vecCurPos->InputMaxLong )//�������һ����β��
		{
			return false;
		}
		return true;
	}


protected:
	wxString m_sOldValue;					//��ʷ���ݱ���
	wxWindowID m_CurID;						//�ؼ�ID
	//bool m_bCheck;						//�Ƿ���Ҫ��ԭ,T��Ҫ
	wxTextCtrl *m_pCurTextCtrl;				//��ǰ�ؼ�
	wxRegEx m_CurEx;
	vector<tagIDconnectTYPE> m_verIDconnectTYPE;
	vector<tagIDconnectTYPE>::iterator m_vecCurPos;
};


#endif