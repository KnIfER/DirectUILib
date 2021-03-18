// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "APP.h"
#include "ControlEx.h"

#ifndef _DWMAPI_H_
typedef struct DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;

typedef struct tagDWL_MARGINS {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} DWM_MARGINS, *PDWM_MARGINS;

// Window attributes
enum DWMWINDOWATTRIBUTE
{
    DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
    DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
    DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
    DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
    DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
    DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
    DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
    DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
    DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
    DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
    DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
    DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
    DWMWA_CLOAK,                        // [set] Cloak or uncloak the window
    DWMWA_CLOAKED,                      // [get] Gets the cloaked state of the window
    DWMWA_FREEZE_REPRESENTATION,        // [set] Force this window to freeze the thumbnail without live update
    DWMWA_LAST
};

// Non-client rendering policy attribute values
enum DWMNCRENDERINGPOLICY
{
    DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
    DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
    DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
    DWMNCRP_LAST
};

// Values designating how Flip3D treats a given window.
enum DWMFLIP3DWINDOWPOLICY
{
    DWMFLIP3D_DEFAULT,      // Hide or include the window in Flip3D based on window style and visibility.
    DWMFLIP3D_EXCLUDEBELOW, // Display the window under Flip3D and disabled.
    DWMFLIP3D_EXCLUDEABOVE, // Display the window above Flip3D and enabled.
    DWMFLIP3D_LAST
};

#define DWM_BB_ENABLE                 0x00000001
#define DWM_BB_BLURREGION             0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004

#define DWM_EC_DISABLECOMPOSITION     0x00000000
#define DWM_EC_ENABLECOMPOSITION      0x00000001
#endif // _DWMAPI_H_

class CDwm
{
public:
    typedef HRESULT (WINAPI *FNDWMENABLECOMPOSITION)(UINT);
    typedef HRESULT (WINAPI *FNDWNISCOMPOSITIONENABLED)(LPBOOL);
    typedef HRESULT (WINAPI *FNENABLEBLURBEHINDWINDOW)(HWND, CONST DWM_BLURBEHIND*);
    typedef HRESULT (WINAPI *FNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND, CONST DWM_MARGINS*);
    typedef HRESULT (WINAPI *FNDWMSETWINDOWATTRIBUTE)(HWND, DWORD, LPCVOID pvAttribute, DWORD);

    FNDWMENABLECOMPOSITION fnDwmEnableComposition;
    FNDWNISCOMPOSITIONENABLED fnDwmIsCompositionEnabled;
    FNENABLEBLURBEHINDWINDOW fnDwmEnableBlurBehindWindow;
    FNDWMEXTENDFRAMEINTOCLIENTAREA fnDwmExtendFrameIntoClientArea;
    FNDWMSETWINDOWATTRIBUTE fnDwmSetWindowAttribute;

    CDwm()
    {
        static HINSTANCE hDwmInstance = ::LoadLibrary(_T("dwmapi.dll"));
        if( hDwmInstance != NULL ) {
            fnDwmEnableComposition = (FNDWMENABLECOMPOSITION) ::GetProcAddress(hDwmInstance, "DwmEnableComposition");
            fnDwmIsCompositionEnabled = (FNDWNISCOMPOSITIONENABLED) ::GetProcAddress(hDwmInstance, "DwmIsCompositionEnabled");
            fnDwmEnableBlurBehindWindow = (FNENABLEBLURBEHINDWINDOW) ::GetProcAddress(hDwmInstance, "DwmEnableBlurBehindWindow");
            fnDwmExtendFrameIntoClientArea = (FNDWMEXTENDFRAMEINTOCLIENTAREA) ::GetProcAddress(hDwmInstance, "DwmExtendFrameIntoClientArea");
            fnDwmSetWindowAttribute = (FNDWMSETWINDOWATTRIBUTE) ::GetProcAddress(hDwmInstance, "DwmSetWindowAttribute");
        }
        else {
            fnDwmEnableComposition = NULL;
            fnDwmIsCompositionEnabled = NULL;
            fnDwmEnableBlurBehindWindow = NULL;
            fnDwmExtendFrameIntoClientArea = NULL;
            fnDwmSetWindowAttribute = NULL;
        }
    }

    BOOL IsCompositionEnabled() const
    {
        HRESULT Hr = E_NOTIMPL;
        BOOL bRes = FALSE;
        if( fnDwmIsCompositionEnabled != NULL ) Hr = fnDwmIsCompositionEnabled(&bRes);
        return SUCCEEDED(Hr) && bRes;
    }

    BOOL EnableComposition(UINT fEnable)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmEnableComposition(fEnable));
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = DWM_BB_ENABLE;
            bb.fEnable = TRUE;
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd, CONST DWM_BLURBEHIND& bb)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL ExtendFrameIntoClientArea(HWND hWnd, CONST DWM_MARGINS& Margins)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmExtendFrameIntoClientArea(hWnd, &Margins));
        return bRes;
    }

    BOOL SetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
    {
        BOOL bRes = FALSE;
        if( fnDwmSetWindowAttribute != NULL ) bRes = SUCCEEDED(fnDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute));
        return bRes;
    }
};

#ifndef WM_DPICHANGED
#   define WM_DPICHANGED       0x02E0
#endif


class CFrameWindowWnd : public CWindowWnd, public INotifyUI, public CDwm
{
public:
    CFrameWindowWnd()  { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void Init() { }

    bool OnHChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), S, L);
        }
        return true;
    }

    bool OnSChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), L);
        }
        return true;
    }

    bool OnLChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, S, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    bool OnAlphaChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            m_pm.SetOpacity((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    bool OnValueChanged(void* param) {
        //TNotifyUI* pMsg = (TNotifyUI*)param;
        //if( pMsg->sType == _T("valuechanged") ) {
        //    m_pm.SetOpacity((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        //}
        return true;
    }

    void OnPrepare() 
    {

       CCheckBoxUI *pCheck = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chkOption")));
       pCheck->Activate();
       pCheck->SetMaxHeight(40);
       pCheck->SetMaxWidth(60);
       
       CSliderUI* pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("alpha_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnAlphaChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("h_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnHChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("s_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnSChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("l_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnLChanged);


       CCircleProgressUI* pSwitch = static_cast<CCircleProgressUI*>(m_pm.FindControl(_T("circle_progress")));

       if( pSwitch ) {
           pSwitch->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnValueChanged);
       }


        //pCheck->SetAttribute(_T("normalimage"), _T("file='image\\switchbutton.png' source='0,0,143,91'"));
        //pCheck->SetAttribute(_T("selectedimage"), _T("file='image\\switchbutton.png' source='0,182,143,273'"));
    
    }

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
        else if( msg.sType == _T("click") ) {
            if( msg.pSender->GetName() == _T("insertimagebtn") ) {
                CRichEditUI* pRich = static_cast<CRichEditUI*>(m_pm.FindControl(_T("testrichedit")));
                if( pRich ) {
                    pRich->RemoveAll();
                }
            }
            else if( msg.pSender->GetName() == _T("changeskinbtn") ) {
                if( CPaintManagerUI::GetResourcePath() == CPaintManagerUI::GetInstancePath() )
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\FlashRes"));
                else
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
                CPaintManagerUI::ReloadSkin();
            }
        }
    }


    class CDialogBuilderCallbackEx : public IDialogBuilderCallback
    {
    public:
        CControlUI* CreateControl(LPCTSTR pstrClass) 
        {
            //if( _tcscmp(pstrClass, DUI_CTR_BTN_PROGRESS) == 0 ) return new CBtnProgressUI;
            if( _tcscmp(pstrClass, TEXT("CircleProgress")) == 0 ) return new CCircleProgressUI;
            return NULL;
        }
    };


    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;

            CDialogBuilderCallbackEx cb;
            CControlUI* pRoot = builder.Create(TEXT("test1.xml"), (UINT)0, &cb, &m_pm);
           
            //CControlUI* pRoot = builder.Create(TEXT("frame.xml"), (UINT)0, &cb, &m_pm);

            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);

            //m_pm.SetDPI(100);  // Set the new DPI, retrieved from the wParam

            //m_pWndShadow = new CWndShadow;
            //m_pWndShadow->Create(m_hWnd);
            //RECT rcCorner = {3,3,4,4};
            //RECT rcHoleOffset = {0,0,0,0};
            //m_pWndShadow->SetImage(_T("LeftWithFill.png"), rcCorner, rcHoleOffset);

            DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
            SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));

            //DWM_BLURBEHIND bb = {0};
            //bb.dwFlags = DWM_BB_ENABLE;
            //bb.fEnable = true;
            //bb.hRgnBlur = NULL;
            //EnableBlurBehindWindow(m_hWnd, bb);

            //DWM_MARGINS margins = {-1}/*{0,0,0,25}*/;
            //ExtendFrameIntoClientArea(m_hWnd, margins);

            Init();
            return 0;
        }
        else if( uMsg == WM_DESTROY ) {
            ::PostQuitMessage(0L);
        }
        else if( uMsg == WM_NCACTIVATE ) {
            if( !::IsIconic(*this) ) return (wParam == 0) ? TRUE : FALSE;
        }
        LRESULT lRes = 0;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

public:
    CPaintManagerUI m_pm;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;


    CFrameWindowWnd* pFrame = new CFrameWindowWnd();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);



    //pFrame->ShowModal();

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
