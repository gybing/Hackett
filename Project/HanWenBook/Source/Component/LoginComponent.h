#pragma once

class LoginWindow   : public juce::CXDocumentWindow
{
public:
    LoginWindow();
    ~LoginWindow();

protected:
    void closeButtonPressed() override;

    virtual juce::BorderSize<int> getContentComponentBorder();
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginWindow)
};

class LoginWebBrowserComponent;
class LoginAnimateWaitComponent;

class LoginComponent : public juce::Component, public HWRequestor
{
public:
    LoginComponent();
    ~LoginComponent();

protected:
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;

public:
    void AsyncLogin(const juce::String& strUid, const juce::String& strToken, HWEnum_StartupType type);
    void Login(const juce::String& strUid, const juce::String& strToken, HWEnum_StartupType type);
    void AutoLogin();
    void ChangeLoginUI(bool bWait); // µÇÂ¼ µÈ´ý ÇÐ»»

    class AsyncLoginUpdater;
    friend class AsyncLoginUpdater;
protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

private:
    juce::ScopedPointer<juce::AttributedString> version;
    juce::ScopedPointer<LoginWebBrowserComponent> webbrowser;
    juce::ScopedPointer<juce::GIFComponent> animateWait;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginComponent)
};

#if JUCE_WINDOWS
class LoginWebBrowserComponent : public CEFBrowserControl
#else if JUCE_MAC
class LoginWebBrowserComponent : public juce::WebBrowserComponent
#endif
{
public:
    LoginWebBrowserComponent();
    ~LoginWebBrowserComponent();

protected:
    virtual const wchar_t* getDefaultUrl();
    virtual void pageFinishedLoading (const juce::String& url);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginWebBrowserComponent)
};

class MainAppWindow;

class LoginWindowHelper : public HWRequestor
{
public:
    juce_DeclareSingleton(LoginWindowHelper, false)
    LoginWindowHelper();
    ~LoginWindowHelper();

    bool checkVersionAndLogin();
    void asyncRelogin();

protected:
    bool showLogin();
    void showMain();
    void relogin();

    void updateVersion(bool bUpdate);
    void cleanUpdateFile(const juce::String& path);
    void killProcess(DWORD dwProcessID);
    bool openProcess(const juce::String& name);
    DWORD findProcess(const juce::String& name);
    DWORD getModuleFileNameExW( HANDLE hProcess, void* hModule, const wchar_t* lpFilename, DWORD nSize);
    bool enumProcessModules(HANDLE hProcess, void* *lphModule, DWORD cb, DWORD* lpcbNeeded);
    bool enumProcesses (DWORD * lpidProcess,DWORD cb,DWORD* lpcbNeeded);
    void* getPsapiLibrary();
protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;

private:
    class AsyncRelogin;
    friend class AsyncRelogin;

    bool checkedVersion;
    juce::ScopedPointer<MainAppWindow> mainWindow;
    juce::ScopedPointer<LoginWindow> loginWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginWindowHelper)
};
