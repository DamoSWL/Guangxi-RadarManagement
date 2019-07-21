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
	purpose:	wxTextCtrl 文本框输入字符过滤。
	可定义仅输入
	正负浮点、正浮点、负浮点；正负整数、正整数、负整数；
	大小写字符、16进制字符；输入长度。
	使用方法：
	1.创建一个CNewInputFliter的对象"m_InputFliter";(不能在局部声明）
	2.链接需要过滤的控件：
	m_InputFliter.InputFilterConnect(m_textCtrl,FILTERTYPE_PFLOAT,3,0,2000,LeftOpenRightClosedInterval);
*********************************************************************/
using namespace std;
#define REGEXSTRLENGTH 100
enum VALIDTYPE{
	FILTERTYPE_FLOAT=0,				//正负浮点
	FILTERTYPE_PFLOAT,				//正浮点
	FILTERTYPE_NFLOAT,				//负浮点
	FILTERTYPE_NUM,					//正负整数
	FILTERTYPE_PNUM,				//正整数
	FILTERTYPE_NNUM,				//负整数
	FILTERTYPE_CHAR,				//大小写字符
	FILTERTYPE_CHAR16,				//16进制字符
	FILTERTYPE_LONGLIMIT			//长度限制
	//FILTERTYPE_CHAR8=6
};
enum INTERVALTYPE{
	AllClosedInterval = 0,			//全闭
	AllOpenInterval,				//全开
	LeftOpenRightClosedInterval,	//左开右闭区间
	LeftClosedRightOpenInterval	//左闭右开区间
};
typedef struct _IDconnectTYPE_
{
	wxWindowID IDNum;				//控件ID
	wxTextCtrl *pTextCtrl;			//控件指针
	VALIDTYPE type;					//过滤类型
	char RegExstr[REGEXSTRLENGTH];	//正则表达式
	size_t InputMaxLong;			//输入长度限制
	int FloatLong;					//浮点个数
	double MinValue;				//最小值
	double MaxValue;				//最大值
	INTERVALTYPE Interval;			//开闭区间
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
	// Qualifier: 过滤文本的输入类型和范围
	// Parameter: wxTextCtrl * pTextCtrl		//传入需要注册的控件
	// Parameter: const VALIDTYPE type			//过滤类型
	// Parameter: const unsigned int FloatLong	//保留小数点后位数（浮点数有效）
	// Parameter: const double MinValue			//最小值
	// Parameter: const double MaxValue			//最大值
	// Parameter: const INTERVALTYPE Interval	//区间类型
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
		assert(MinValue <= MaxValue);			//输入反，进行提示
		if (MinValue < MaxValue)//防止输入错误
		{
			IDconTYPE.MinValue = MinValue;
			IDconTYPE.MaxValue = MaxValue;
		}
		else
		{
			IDconTYPE.MinValue = MaxValue;
			IDconTYPE.MaxValue = MinValue;
		}
		if (FloatLong<=10)				//小数点后，最多10位
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
	// Qualifier: 只过滤文本的长度
	// Parameter: wxTextCtrl * pTextCtrl		//传入需要注册的控件
	// Parameter: const size_t InputMaxLong		//最大长度
	// Parameter: const VALIDTYPE type			//默认长度限制
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
		//由于不需要颜色的提醒，取消本事件
		//pTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CNewInputFliter::OnCharUpdate ), NULL, this );
	}
	//************************************
	// Method:    ChangeInputRangeLimit
	// FullName:  CNewInputFliter::ChangeInputRangeLimit
	// Access:    public 
	// Returns:   bool
	// Qualifier: 注册后，修改文本过滤值的范围
	// Parameter: const wxTextCtrl * pTextCtrl	//传入需要注册的控件
	// Parameter: const unsigned int FloatLong	//保留小数点后位数
	// Parameter: const float MinValue			//最小值
	// Parameter: const float MaxValue			//最大值
	// Parameter: const INTERVALTYPE Interval	//区间类型
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
		assert(MinValue <= MaxValue);			//输入反，进行提示
		if (MinValue < MaxValue)//防止输入错误
		{
			vecCurPos->MinValue = MinValue;
			vecCurPos->MaxValue = MaxValue;
		}
		else
		{
			vecCurPos->MinValue = MaxValue;
			vecCurPos->MaxValue = MinValue;
		}
		if (FloatLong<=10)				//小数点后，最多10位
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
	// Qualifier: 删除过滤事件
	// Parameter: wxTextCtrl * TextCtrl传入需要反注册的控件
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
		//由于不需要颜色的提醒，取消本事件
		//pTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CNewInputFliter::OnCharUpdate ), NULL, this );

	}
protected:
	//************************************
	// Method:    OnSetFocus
	// FullName:  CNewInputFliter::OnSetFocus
	// Access:    public 
	// Returns:   void
	// Qualifier: 保存最新正确值
	// Parameter: wxFocusEvent & event
	//************************************
	void OnSetFocus( wxFocusEvent& event )
	{
		//m_bCheck = false;
		m_CurID = event.GetId();
		m_pCurTextCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(m_CurID);		//查找对应的控件

		m_vecCurPos = m_verIDconnectTYPE.begin();
		assert(m_vecCurPos != m_verIDconnectTYPE.end());
		while(m_vecCurPos!=m_verIDconnectTYPE.end() && m_vecCurPos->IDNum != m_CurID ) m_vecCurPos++;
		if (m_vecCurPos == m_verIDconnectTYPE.end() || m_pCurTextCtrl!=m_vecCurPos->pTextCtrl)
		{
			if (m_vecCurPos->pTextCtrl->GetId() == m_CurID)		//ID相同
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
	// Qualifier:	焦点撤销过滤判断输入正确与否
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
		/*if (m_bCheck)			//颜色不需要还原
		{
			m_pCurTextCtrl->SetValue(m_sOldValue);
			m_pCurTextCtrl->SetBackgroundColour(RGB(255,255,255));
			m_pCurTextCtrl->Refresh();
		}*/
		//去除前面0
		if (m_vecCurPos->type == FILTERTYPE_PNUM || m_vecCurPos->type == FILTERTYPE_NNUM||
			m_vecCurPos->type == FILTERTYPE_NUM)			//防止出现0001
		{
			long ivle=0 ;
			if (m_pCurTextCtrl->GetValue().ToLong(&ivle))
			{
				wxString tp = wxString::Format(wxT("%d"),ivle);
				m_pCurTextCtrl->SetValue(tp);
			}			
		}
		if (m_vecCurPos->type == FILTERTYPE_PFLOAT || m_vecCurPos->type == FILTERTYPE_FLOAT ||
			m_vecCurPos->type == FILTERTYPE_NFLOAT)			//防止出现0001
		{
			double dv=0;
			if (m_pCurTextCtrl->GetValue().ToDouble(&dv))
			{
				wxString Ft=wxT("%") + wxString::Format(wxT(".%df"),m_vecCurPos->FloatLong);
				wxString tp = wxString::Format(Ft,dv);
				m_pCurTextCtrl->SetValue(tp);
			}
		}
		//防止出现+
		if (m_vecCurPos->type == FILTERTYPE_PFLOAT || m_vecCurPos->type == FILTERTYPE_FLOAT||
			m_vecCurPos->type == FILTERTYPE_PNUM || m_vecCurPos->type == FILTERTYPE_NUM)		//防止出现+123.1
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
			//正则过滤
			if (!InputRegEx(m_pCurTextCtrl->GetValue(),m_vecCurPos->RegExstr))
			{
				IsReValue = true;
			}
			//范围过滤
			if (!CheckRange(m_pCurTextCtrl->GetValue(),true))
			{
				IsReValue = true;
			}
		}
		else
		{
			//正则过滤
			wxString str = m_pCurTextCtrl->GetValue();
			if ( str.Contains(wxT("%")) )
			{
				IsReValue = true;
			}
			else if (InputRegEx(str,m_vecCurPos->RegExstr))	//包含特殊符号返回True
			{
				IsReValue = true;
			}
			else if ( !CheckInputLong(str) )//长度限制，需要判断
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
	// Qualifier:过滤键盘事件
	// Parameter: const int keycode
	//************************************
	bool KeyFilter(const int keycode)
	{
		if (m_vecCurPos->type == FILTERTYPE_NUM ||					//整数，浮点，十六进制能输入数字
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
		if(m_vecCurPos->type ==FILTERTYPE_PFLOAT||					//浮点输入.
			m_vecCurPos->type ==FILTERTYPE_NFLOAT||
			m_vecCurPos->type ==FILTERTYPE_FLOAT)
		{
			if (keycode == '.')
				return true;
		}
		if (m_vecCurPos->type == FILTERTYPE_CHAR)							//字符
		{
			if (wxIsalpha(keycode))
			{
				return true;
			}
		}
		if (m_vecCurPos->type == FILTERTYPE_CHAR16)						//十六进制X
		{
			if (keycode == 'x'|| keycode == 'X'||
				(keycode >= 'a' && keycode <= 'f')||
				(keycode >= 'A' && keycode <= 'F'))
			{
				return true;
			}
		}
		if (m_vecCurPos->type == FILTERTYPE_PNUM ||						//+符号判断
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
		if (m_vecCurPos->type == FILTERTYPE_NNUM||						//-符号判断
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
	// Qualifier: 键盘输入过滤
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
		switch (KeyCode)								//常用按键
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
		//防止重复输入".""-""+"
		if (( ( KeyCode == '.') && (curstr.Contains(wxT(".")) )) || 
			( ( KeyCode == '-') && (curstr.Contains(wxT("-")) )) ||
			( ( KeyCode == '+') && (curstr.Contains(wxT("+")) ))  )
		{
			event.Skip(false);
			return;
		}
		curstr.insert(m_pCurTextCtrl->GetInsertionPoint(),1,event.GetUnicodeKey());
		//获取数据大小
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
	// Qualifier: 文本更新事件过滤  //由于不需要颜色的提醒，取消本事件
	// Parameter: wxCommandEvent & event
	//************************************
	void OnCharUpdate(wxCommandEvent& event )
	{
		//if (m_pCurTextCtrl==NULL)
		//{
		//	return;
		//}
		//long id = event.GetId();								//查找对应的控件
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
	// Qualifier: 正则表达式批评
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
		if (!wxstr.IsEmpty() && m_CurEx.Compile(wxstr))				//防止空模版 匹配输入
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
		case FILTERTYPE_NUM:							//正负整数
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-+]?[0-9]+$");
				break;
			}
		case FILTERTYPE_PNUM:							//正整数
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[+]?[0-9]+$");
				break;
			}
		case FILTERTYPE_NNUM:							//负整数
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-]{1,1}[0-9]+$");
				break;
			}
		case FILTERTYPE_CHAR:							//字符
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^^[A-Za-z]+$");
				break;
			}
		case FILTERTYPE_FLOAT:							//正负负点
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-+]?[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_PFLOAT:							//正负点
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[+]?[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_NFLOAT:							//负负点
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^[-]{1,1}[0-9]+(\\.[0-9]+)?$");
				break;
			}
		case FILTERTYPE_CHAR16:							//十六进制
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"^0[xX][0-9a-fA-F]+$");
				break;
			}
		case FILTERTYPE_LONGLIMIT:						//长度限制
			{
				_sprintf_p(cstr,REGEXSTRLENGTH,"[`~!@#$^&*()=|\':;,<>/?！@#￥……&*（）——|【】‘；：”“。’，、？]");
				break;
			}
			//case FILTERTYPE_CHAR8:							//八进制
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
			m_vecCurPos->type == FILTERTYPE_FLOAT  || m_vecCurPos->type == FILTERTYPE_NUM )		//范围过滤
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
				if (!Tpye)						//	不是最后的检测
				{
					if (ftmp<fzero && ftmp>-fzero)	//当输入为0的时候，防止开区间返回为错误
					{
						return true;
					}
					if (ftmp>0)					//正输入
					{
						MinValue = 0;
					}
					else						//负输入
					{
						MaxValue = 0;
					}
				}
				else if (!bCov)			//最后检测，且输入出错
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
	// Qualifier: 获取输入长度限制
	//************************************
	bool CheckInputLong(const wxString &instr)
	{
		string str(instr.mb_str());
		if ( strlen(str.c_str())+1 > m_vecCurPos->InputMaxLong )//最后增加一个结尾符
		{
			return false;
		}
		return true;
	}


protected:
	wxString m_sOldValue;					//历史数据保存
	wxWindowID m_CurID;						//控件ID
	//bool m_bCheck;						//是否需要还原,T需要
	wxTextCtrl *m_pCurTextCtrl;				//当前控件
	wxRegEx m_CurEx;
	vector<tagIDconnectTYPE> m_verIDconnectTYPE;
	vector<tagIDconnectTYPE>::iterator m_vecCurPos;
};


#endif