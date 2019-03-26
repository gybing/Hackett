#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

typedef void (_cdecl *PFuncOnItemClick)(int,const std::vector<juce::String>&);

class CEFClientApp 
    : public CefApp
    , public CefBrowserProcessHandler
    , public CefRenderProcessHandler
{
public:
    CEFClientApp();

    static CefRefPtr<CEFClientApp> getInstance();
    bool Initlize(PFuncOnItemClick func);
    void Finalize();

protected:
    virtual CefRefPtr<CefRenderProcessHandler>	GetRenderProcessHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefBrowserProcessHandler>	GetBrowserProcessHandler() OVERRIDE { return this; }
    virtual void OnContextInitialized() OVERRIDE;
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

private:
    PFuncOnItemClick funcOnItemClick;
    IMPLEMENT_REFCOUNTING(CEFClientApp);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CEFClientApp)
};

class JWebTopJSHandler : public CefV8Handler
{
public:
    JWebTopJSHandler(PFuncOnItemClick func) : funcOnItemClick(func) {}

    virtual bool JWebTopJSHandler::Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) OVERRIDE;

private:
    PFuncOnItemClick funcOnItemClick;
    IMPLEMENT_REFCOUNTING(JWebTopJSHandler);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JWebTopJSHandler)
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
