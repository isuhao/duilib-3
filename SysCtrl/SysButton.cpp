#include "StdAfx.h"

namespace DuiLib{

class CSysButtonWnd : public CWindowWnd
{
public:
	CSysButtonWnd();
	void Init(CSysButtonUI* pOwner);

	virtual LPCTSTR GetSuperClassName() const override;
	virtual LPCTSTR GetWindowClassName() const override;
	virtual void OnFinalMessage(HWND hWnd) override;

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
	CSysButtonUI* m_pOwner;
};

CSysButtonWnd::CSysButtonWnd()
	: m_pOwner(nullptr)
{

}

void CSysButtonWnd::Init(CSysButtonUI* pOwner)
{
	m_pOwner = pOwner;
	Create(m_pOwner->GetManager()->GetPaintWindow(),nullptr,m_pOwner->GetStyle(),m_pOwner->GetExStyle());

	HFONT hFont=NULL;
	int iFontIndex=m_pOwner->GetFont();
	if (iFontIndex!=-1)
		hFont=m_pOwner->GetManager()->GetFont(iFontIndex);
	if (hFont==NULL)
		hFont=m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;
	SetWindowFont(*this, hFont, TRUE);
}

LPCTSTR CSysButtonWnd::GetSuperClassName() const
{
	return WC_BUTTON;
}

LPCTSTR CSysButtonWnd::GetWindowClassName() const
{
	return _T("___") WC_BUTTON;
}

void CSysButtonWnd::OnFinalMessage(HWND hWnd)
{
	m_pOwner->Invalidate();
	m_pOwner->m_pWnd = nullptr;
	delete this;
}

LRESULT CSysButtonWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CONTEXTMENU:
		{
			if(m_pOwner->IsContextMenuUsed())
				m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_MENU);
			return 0;
		}
	}

	if(uMsg >= OCM__BASE){
		uMsg -= OCM__BASE;
		TNotifyUI msg;
		msg.pSender = m_pOwner;
		msg.wParam = 0;
		msg.lParam = 0;
		if(uMsg == WM_COMMAND){
			auto code = HIWORD(wParam);
			switch(code)
			{
			case BN_CLICKED:
				msg.sType = DUI_MSGTYPE_CLICK;
				m_pOwner->GetManager()->SendNotify(msg);
				return 0;
			}
		}
		else if(uMsg == WM_CTLCOLORBTN){
			return LRESULT(GetStockBrush(WHITE_BRUSH));
		}
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 

CSysButtonUI::CSysButtonUI()
	: m_pWnd(nullptr)
{
}

CSysButtonUI::~CSysButtonUI()
{
	if(m_pWnd) ::PostMessage(*m_pWnd,WM_CLOSE,0,0);
}

LPCTSTR CSysButtonUI::GetClass() const
{
	return SYSCTRL_BUTTON_UI;
}
LPVOID CSysButtonUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, SYSCTRL_BUTTON) ==0 ) return this;
	return __super::GetInterface(pstrName);
}

UINT CSysButtonUI::GetControlFlags() const
{
	if( !IsEnabled() ) return CControlUI::GetControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}


void CSysButtonUI::SetFont(int index)
{
	m_iFont = index;
}

int CSysButtonUI::GetFont() const
{
	return m_iFont;
}

void CSysButtonUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
	CDuiRect r = GetPos();
	if(m_pWnd) ::SetWindowPos(*m_pWnd,0,r.left, r.top, r.GetWidth(), r.GetHeight(), SWP_NOZORDER | ::IsWindowVisible(*m_pWnd)?SWP_SHOWWINDOW:SWP_HIDEWINDOW);
}

SIZE CSysButtonUI::EstimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
	return CControlUI::EstimateSize(szAvailable);
}

void CSysButtonUI::DoEvent(TEventUI& event)
{
	if(event.Type == UIEVENT_SETFOCUS){
		::SetFocus(*m_pWnd);
	}
	return __super::DoEvent(event);
}

void CSysButtonUI::DoInit()
{
	m_pWnd = new CSysButtonWnd;
	m_pWnd->Init(this);
	SetEnabled(IsEnabled());
	SetVisible(IsVisible());
	SetText(GetText());
	::SetWindowPos(*m_pWnd,0,0,0,GetFixedWidth(),GetFixedHeight(),SWP_NOMOVE|SWP_NOZORDER);
}

void CSysButtonUI::SetEnabled(bool bEnable)
{
	if(m_pWnd) ::EnableWindow(*m_pWnd, bEnable);
	m_bEnabled = bEnable;
}

bool CSysButtonUI::IsEnabled() const
{
	return m_bEnabled;
}

void CSysButtonUI::SetVisible(bool bVisible /* = true */)
{
	__super::SetVisible(bVisible);
	if(m_pWnd) m_pWnd->ShowWindow(bVisible,false);
}

void CSysButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if(_tcscmp(pstrName, _T("font")) == 0)			SetFont(_ttoi(pstrValue));

	else __super::SetAttribute(pstrName, pstrValue);
}


//////////////////////////////////////////////////////////////////////////

void CSysButtonUI::SetText( LPCTSTR pstrText )
{
	__super::SetText(pstrText);
	if(m_pWnd) m_pWnd->SendMessage(WM_SETTEXT,0,LPARAM(pstrText));
}

DWORD CSysButtonUI::GetStyle()
{
	return WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON;
}

DWORD CSysButtonUI::GetExStyle()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////


LPCTSTR CSysOptionUI::GetClass() const
{
	return SYSCTRL_OPTION_UI;
}
LPVOID CSysOptionUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, SYSCTRL_OPTION) ==0 ) return this;
	return __super::GetInterface(pstrName);
}

UINT CSysOptionUI::GetControlFlags() const
{
	if( !IsEnabled() ) return CControlUI::GetControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

DWORD CSysOptionUI::GetStyle()
{
	return WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON;
}
DWORD CSysOptionUI::GetExStyle()
{
	return 0;
}

void CSysOptionUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	return __super::SetAttribute(pstrName, pstrValue);
}


//////////////////////////////////////////////////////////////////////////


LPCTSTR CSysCheckBoxUI::GetClass() const
{
	return SYSCTRL_CHECKBOX_UI;
}
LPVOID CSysCheckBoxUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, SYSCTRL_CHECKBOX) ==0 ) return this;
	return __super::GetInterface(pstrName);
}

UINT CSysCheckBoxUI::GetControlFlags() const
{
	if( !IsEnabled() ) return CControlUI::GetControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

DWORD CSysCheckBoxUI::GetStyle()
{
	return WS_CHILD|WS_VISIBLE|BS_CHECKBOX;
}
DWORD CSysCheckBoxUI::GetExStyle()
{
	return 0;
}

void CSysCheckBoxUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	return __super::SetAttribute(pstrName, pstrValue);
}

//////////////////////////////////////////////////////////////////////////

// 
// LPCTSTR CSysGroupBoxUI::GetClass() const
// {
// 	return SYSCTRL_GROUPBOX_UI;
// }
// LPVOID CSysGroupBoxUI::GetInterface(LPCTSTR pstrName)
// {
// 	if(_tcscmp(pstrName, SYSCTRL_GROUPBOX) ==0 ) return this;
// 	return __super::GetInterface(pstrName);
// }
// 
// UINT CSysGroupBoxUI::GetControlFlags() const
// {
// 	if( !IsEnabled() ) return CControlUI::GetControlFlags();
// 
// 	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
// }
// 
// DWORD CSysGroupBoxUI::GetStyle()
// {
// 	return WS_CHILD|WS_VISIBLE|BS_GROUPBOX;
// }
// DWORD CSysGroupBoxUI::GetExStyle()
// {
// 	return 0;
// }
// 
// void CSysGroupBoxUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
// {
// 	return __super::SetAttribute(pstrName, pstrValue);
// }


}
