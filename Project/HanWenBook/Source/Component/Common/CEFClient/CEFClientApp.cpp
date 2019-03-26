#include "JuceHeader.h"
#include "CEFClient.h"
#include <string>
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

static CefRefPtr<CEFClientApp> theCefClientApp;

CEFClientApp::CEFClientApp()
    : funcOnItemClick(nullptr)
{
}

void CEFClientApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();
}

CefRefPtr<CEFClientApp> CEFClientApp::getInstance()
{
    if (!theCefClientApp)
    {
        theCefClientApp = new CEFClientApp;
    }
    
    return theCefClientApp;
}

void CEFClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    CefRefPtr<CefV8Value> object = context->GetGlobal();// 获取根对象(window对象）
    CefRefPtr<JWebTopJSHandler> handler = new JWebTopJSHandler(funcOnItemClick);// 创建对应的handler
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("onItemClick", handler);// 创建一个V8函数
    object ->SetValue("onItemClick", func, V8_PROPERTY_ATTRIBUTE_NONE);// 添加函数到根对象上
}

bool CEFClientApp::Initlize(PFuncOnItemClick func)
{
    funcOnItemClick = func;

    CefEnableHighDPISupport();
    CefMainArgs mainargs(nullptr);
    if (CefExecuteProcess(mainargs, this, NULL) == 0)
        return false;

    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    settings.log_severity = LOGSEVERITY_DISABLE;
    settings.multi_threaded_message_loop = true; //使用主程序消息循环
    settings.single_process = true;              //使用多进程模式
    settings.ignore_certificate_errors = true;   //忽略掉ssl证书验证错误
    settings.command_line_args_disabled = true;
    cef_string_set(L"zh-CN", 6, &settings.accept_language_list, 1);
    settings.no_sandbox = true;
    return CefInitialize(mainargs, settings, static_cast<CefRefPtr<CefApp> >(this), NULL);
}

void CEFClientApp::Finalize()
{
    CefShutdown();
    theCefClientApp = nullptr;
}

bool JWebTopJSHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
    if (name == L"onItemClick" && funcOnItemClick)
    {
        if (arguments.size() == 2 && arguments[0]->IsInt() && arguments[1]->IsArray())
        {
            std::vector<juce::String> vecParas;
            const unsigned int length = arguments[1]->GetArrayLength();
            for (unsigned int i=0; i<length; ++i)
            {
                vecParas.push_back(juce::CharPointer_UTF8(arguments[1]->GetValue(i)->GetStringValue().ToString().c_str()));
            }
            (*funcOnItemClick)(arguments[0]->GetIntValue(), vecParas);
        }
    }
    return true;
} 