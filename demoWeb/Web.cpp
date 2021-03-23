// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Web.h"
#include "ControlEx.h"

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
    CFrameWindowWnd()  { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void OnPrepare() 
    {

       CCheckBoxUI *pCheck = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chkOption")));

    }

    void Init() {

        //CWebBrowserUI* pBrowser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("bw")));


        CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("ie")));
        if( pActiveXUI ) {
            IWebBrowser2* pWebBrowser = NULL;
            pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
            if( pWebBrowser != NULL ) {
                //pWebBrowser->Navigate(L"https://github.com/duilib/duilib",NULL,NULL,NULL,NULL);  
                //pWebBrowser->Navigate(L"about:blank",NULL,NULL,NULL,NULL); 

                pWebBrowser->Navigate(TEXT("https://www.baidu.com"),NULL,NULL,NULL,NULL);

                pWebBrowser->Release();
            }
        }

        


    }


    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
    }


    class CDialogBuilderCallbackEx : public IDialogBuilderCallback
    {
    public:
        CControlUI* CreateControl(LPCTSTR pstrClass) 
        {
            //if( _tcscmp(pstrClass, TEXT("CircleProgress")) == 0 ) return new CCircleProgressUI;

            return NULL;
        }
    };


    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;

            CDialogBuilderCallbackEx cb;
            CControlUI* pRoot = builder.Create(TEXT("testWeb.xml"), (UINT)0, &cb, &m_pm);
           
            //CControlUI* pRoot = builder.Create(TEXT("frame.xml"), (UINT)0, &cb, &m_pm);

            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);


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
