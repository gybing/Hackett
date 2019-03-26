#include "JuceHeader.h"
#include "LoginComponent.h"
#include "MainComponent.h"

LoginWindow::LoginWindow()
    : CXDocumentWindow (juce::JUCEApplication::getInstance()->getApplicationName(),
    juce::Colour(0xFFf0f0f0),
    CXDocumentWindow::closeButton,
    true, juce::BorderSize<int>(0,0,0,0))
{
    setContentOwned(new LoginComponent, false);
    setUsingNativeTitleBar (false);
    setResizable (false, false);
    setSize(600,400);
    centreAroundComponent (nullptr, getWidth(), getHeight());
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::closeButtonPressed()
{
    exitModalState(0);
    setVisible (false);
    juce::JUCEApplication::quit();
}

juce::BorderSize<int> LoginWindow::getContentComponentBorder()
{
    return juce::BorderSize<int>(50,118,50,118);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class LoginComponent::AsyncLoginUpdater : public juce::AsyncUpdater
{
public:
    AsyncLoginUpdater(LoginComponent& comp, const juce::String& uid, const juce::String& token, HWEnum_StartupType type)
        : loginComponent(comp)
        , strUserID(uid)
        , strUserToken(token)
        , typeStartup(type)
    {
    }

private:
    virtual void handleAsyncUpdate() override
    {
        if (strUserID.isNotEmpty() && strUserToken.isNotEmpty())
            loginComponent.Login(strUserID, strUserToken, typeStartup);
    }

    LoginComponent& loginComponent;
    juce::String strUserID;
    juce::String strUserToken;
    HWEnum_StartupType typeStartup;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LoginComponent::LoginComponent()
{
    juce::String strVersion;
    strVersion << L"版本：" << ProjectInfo::versionString;
    version = new juce::AttributedString;
    version->append(strVersion, juce::Font(16), ProjectInfo::colorLightGrey);
    addAndMakeVisible(animateWait = new juce::GIFComponent(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\res\\images\\loading.gif"));
    addAndMakeVisible(webbrowser = new LoginWebBrowserComponent());
    
    AutoLogin();
}

LoginComponent::~LoginComponent()
{
}

void LoginComponent::paint(juce::Graphics& g)
{
    version->draw(g, juce::Rectangle<float>(265.0f,275.0f,100.0f,25.0f));
    juce::Image image = CXIMAGE_CACHE_LOAD(L"login_title_logo.png");
    g.setOpacity (1.0f);
    g.drawImageWithin (image, 0, 0, image.getWidth(), image.getHeight(), juce::RectanglePlacement::centred, false);
}

void LoginComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    animateWait->setBounds(bounds.getCentreX()-32, bounds.getCentreY()-32, 64, 64);
    webbrowser->setBounds(0,90,364,158);
}

void LoginComponent::Login(const juce::String& strUid, const juce::String& strToken, HWEnum_StartupType type)
{
    // 登录。。。
    ChangeLoginUI(true);

    // 发送登录请求。。。
    HWParaLogin paraUI(strUid, strToken, type);
    HWDocAuth::getInstance()->SubmitRequest(this, HWReq_Auth_Login, &paraUI, 0);

    // 记住用户名
    {
        CXUserLoginConfig userInfo;
        userInfo.strUserID = strUid;
        userInfo.strUserToken = strToken;
        userInfo.bAotuLogin = true;

        CXOption::getInstance()->SetCurrentUserInfo(userInfo);
        CXOption::getInstance()->SaveUserLoginConfig();
    }   
}

void LoginComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Auth_Login)
    {
        if (err == HWError_SUCCEED)
        {
            LoginWindow* window = static_cast<LoginWindow*>(getParentComponent());
            if (window)
            {
                window->exitModalState(1);
                window->setVisible(false);
            }

            // 请求类别数
            HWParaTypeTree treeUI;
            HWDocBookType::getInstance()->SubmitRequest(nullptr, HWReq_Type_Tree, &treeUI, 0);
            return;
        }

        ChangeLoginUI(false);
    }
}

void LoginComponent::AutoLogin()
{
    // 如果是第一次启动 读取命令行参数
    static bool bInitedAutoLogin = false;
    juce::String strInitCmdLine;
    if (!bInitedAutoLogin)
    {
        bInitedAutoLogin = true;
        strInitCmdLine = juce::JUCEApplication::getInstance()->getCommandLineParameters();
    }

    juce::String strUserID, strToken;
    if (CXOption::ParseArgv(strUserID, strToken, strInitCmdLine.toUTF8()))
    {
        Login(strUserID, strToken, HWEnum_StartupType::HWStartupType_Lexin);
    }
    else if (CXOption::getInstance()->IsAutoLogin())
    {
        CXUserLoginConfig userInfo;
        userInfo = *CXOption::getInstance()->GetCurrentUserInfo();
        if (!strUserID.isEmpty() && strUserID != userInfo.strUserID)
            return;

        Login(userInfo.strUserID, userInfo.strUserToken, HWStartupType_Self);
    }
}

void LoginComponent::ChangeLoginUI(bool bWait)
{
    if (webbrowser) webbrowser->setVisible(!bWait);
    if (animateWait) animateWait->setVisible(bWait);
}

void LoginComponent::AsyncLogin(const juce::String& strUid, const juce::String& strToken, HWEnum_StartupType type)
{
    (new AsyncLoginUpdater(*this, strUid, strToken, type))->triggerAsyncUpdate();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LoginWebBrowserComponent::LoginWebBrowserComponent()
#if JUCE_MAC
    : juce::WebBrowserComponent(false)
{
    goToURL(ProjectInfo::LoginWebUrl);
}
#else if JUCE_WINDOWS
{
}
#endif

LoginWebBrowserComponent::~LoginWebBrowserComponent()
{
}

void LoginWebBrowserComponent::pageFinishedLoading(const juce::String& url)
{
    juce::String strUid, strToken;
    if (CXOption::ParseUrl(url, strUid, strToken))
    {
        if (strUid.isNotEmpty() && strToken.isNotEmpty())
        {
            LoginComponent* comp = static_cast<LoginComponent*>(getParentComponent());
            if (comp)
            {
                comp->AsyncLogin(strUid, strToken, HWStartupType_Self);
            }
        }

        return /*1*/;
    }
    return /*0*/;
}

const wchar_t* LoginWebBrowserComponent::getDefaultUrl()
{
     return ProjectInfo::LoginWebUrl;
}

//////////////////////////////////////////////////////////////////////////
// LoginWindowHelper

class LoginWindowHelper::AsyncRelogin : public juce::AsyncUpdater
{
public:
    AsyncRelogin(){}

private:
    virtual void handleAsyncUpdate() override
    {
        LoginWindowHelper::getInstance()->relogin();
        delete this;
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AsyncRelogin)
};


juce_ImplementSingleton(LoginWindowHelper)

LoginWindowHelper::LoginWindowHelper()
    : checkedVersion(false)
{
}

LoginWindowHelper::~LoginWindowHelper()
{
}

void LoginWindowHelper::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_System_CheckVersion)
    {
        updateVersion(TRUE);

        if (showLogin())
        {
            showMain();
        }
    }
}

void LoginWindowHelper::updateVersion(bool bUpdate)
{
#ifdef JUCE_WIN32
    HWTCheckVersion* pCheckVersion = HWDocAuth::getInstance()->GetCheckVersion();
    if (pCheckVersion && bUpdate)
    {
        juce::String strParam;
        strParam << ProjectInfo::updaterName << L" " << pCheckVersion->strVersion << L" " << (pCheckVersion->bForce==1?L"ENFORCE":L"SILENCE") << pCheckVersion->strVersion;
        if (pCheckVersion->bForce == 1)
        {
            // 强制更新，直接退出客户端
            openProcess(strParam);
            DWORD id = ::GetCurrentProcessId(); 
            HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, id); 
            TerminateProcess(hProc, 0);
        }
        else
        {
            // 非强制更新
            openProcess(strParam);
            return;
        }
    }

    wchar_t strModulePath[MAX_PATH];
    GetModuleFileNameW(NULL, strModulePath, MAX_PATH);
    juce::String str(strModulePath);
    cleanUpdateFile(str.substring(0, str.lastIndexOf(juce::String(L"\\"))));
#endif
}

#define HANWEN_REG_PATH L"Software\\LXRead\\Update\\"	// 更新组件相关注册表项存储路径
#define HANWEN_NAME_UPDATE_SUECCEED L"UpdateSucceed"		// 标识上次更新是否成功
#define HANWEN_NAME_ORIGINVERSION L"OriginVersion"				// 标识如果上次更新失败情况的更新前本地客户端版本号

bool LoginWindowHelper::checkVersionAndLogin()
{
#ifdef JUCE_WIN32
    // check vsn
    bool bFortest = false;
    juce::StringArray strArray = juce::JUCEApplication::getCommandLineParameterArray();
    for (int i=0; i<strArray.size(); ++i)
    {
        if (strArray[i].toUpperCase() == L"FORTEST")
        {
            bFortest = true;
            break;
        }
    }

    // 2.检测上次是否更新成功了
    DWORD dwProcessID = findProcess(ProjectInfo::updaterName);
    if (dwProcessID != 0)
        killProcess(dwProcessID);

    if (juce::WindowsRegistry::valueExists(HANWEN_REG_PATH))
    {
        juce::String strValue(juce::String::empty);
        strValue = juce::WindowsRegistry::getValue(HANWEN_REG_PATH HANWEN_NAME_UPDATE_SUECCEED, strValue);
        if(strValue.getIntValue() == 0)
        {
            strValue = juce::WindowsRegistry::getValue(HANWEN_REG_PATH HANWEN_NAME_ORIGINVERSION, strValue);
            if (strValue.isNotEmpty())
            {
                juce::String strParam;
                strParam << ProjectInfo::updaterName << L" " << strValue << L" ENFORCE" <<  (bFortest ? L" fortest" : L"");
                openProcess(strParam);
                return false;
            }
        }
    }

    HWParaVsnCheck paraUI(HWDevice_PC, ProjectInfo::versionString, bFortest);
    HWDocAuth::getInstance()->SubmitRequest(this, HWReq_System_CheckVersion, &paraUI, 0);
    return true;
#elif JUCE_MAC
    if (showLogin())
    {
        showMain();
    }
    return true;
#endif
}

bool EnableDebugPrivilege()
{
#ifdef JUCE_WIN32
    HANDLE hToken; TOKEN_PRIVILEGES tkp; // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false; // Get the LUID for the shutdown privilege. 
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
    tkp.PrivilegeCount = 1; // one privilege to set 
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // Get the shutdown privilege for this process. 
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
    if (GetLastError() != ERROR_SUCCESS) 
        return false;
    return true;
#endif // JUCE_WIN32
}

void LoginWindowHelper::cleanUpdateFile(const juce::String& path)
{
#ifdef JUCE_WIN32
    juce::String strDir = path + "\\*.*";

    //开始对目录进行搜索  
    WIN32_FIND_DATA fd;  
    HANDLE fHandle=FindFirstFile(strDir.toWideCharPointer(), &fd);  
    if(fHandle!=INVALID_HANDLE_VALUE)
    {  
        while(FindNextFile(fHandle,&fd))  
        {
            //继续搜索目录  
            if(fd.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)  
            {
                if (juce::String(L".") == fd.cFileName || juce::String(L"..") == fd.cFileName)
                    continue;

                juce::String strChild = path;
                strChild << L"\\";
                strChild << fd.cFileName;
            }

            juce::String strFile = fd.cFileName;
            strFile = strFile.substring(strFile.length()-4,strFile.length());
            if (strFile.toUpperCase() == L".BAK")
            {
                strFile = path;
                strFile << L"\\";
                strFile << fd.cFileName;
                if (!DeleteFile(strFile.toWideCharPointer()))
                {
                    if (EnableDebugPrivilege())
                    {
                        DeleteFile(strFile.toWideCharPointer());
                    }
                }
            }
        }  
    }
#endif
}

void LoginWindowHelper::killProcess(DWORD dwProcessID)
{
#ifdef JUCE_WIN32
    // When the all operation fail this function terminate the "winlogon" Process for force exit the system.
    HANDLE hYourTargetProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION |   // Required by Alpha
        PROCESS_CREATE_THREAD     |   // For CreateRemoteThread
        PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
        PROCESS_VM_WRITE,             // For WriteProcessMemory
        FALSE, dwProcessID);

    if(hYourTargetProcess == NULL)
    {
        return;
    }
    TerminateProcess(hYourTargetProcess, 0);
    return;
#endif
}

bool LoginWindowHelper::openProcess(const juce::String& name)
{
#ifdef JUCE_WIN32
    std::wstring strAppName, strParams;

    strAppName = name.toWideCharPointer();
    int nPos = (int)strAppName.find_first_of(L' ');
    strParams = strAppName.substr(nPos);
    strAppName = strAppName.substr(0, nPos);

    HINSTANCE nResult = ShellExecuteW(
        NULL,
        NULL,     // do not request elevation unless needed
        strAppName.c_str(),
        strParams.c_str(), // params 
        NULL,     // directory
        SW_SHOW);

    if((int)(DWORD_PTR)nResult == SE_ERR_ACCESSDENIED) {
        nResult = ShellExecuteW(
            NULL,
            L"runas", // Trick for requesting elevation, this is a verb not listed in the documentation above.
            strAppName.c_str(),
            strParams.c_str(), // params 
            NULL,        // directory
            SW_SHOW);
    }
#endif
    return TRUE;
}

DWORD LoginWindowHelper::findProcess(const juce::String& name)
{
#ifdef JUCE_WIN32
    DWORD aProcesses[1024], cbNeeded, cbMNeeded;
    void* hMods[1024];
    HANDLE hProcess;
    wchar_t szProcessName[MAX_PATH]={0};

    if (!enumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))  
        return 0;

    for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); ++i)
    {
        hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
        enumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
        getModuleFileNameExW( hProcess, hMods[0], szProcessName,sizeof(szProcessName));

        if(name == szProcessName)
            return(aProcesses[i]);
    }
#endif
    return 0;
}

bool LoginWindowHelper::enumProcesses(DWORD * lpidProcess,DWORD cb,DWORD* lpcbNeeded)
{
#ifdef JUCE_WIN32

    typedef	BOOL (__stdcall *EnumProcessesFunc)(DWORD * lpidProcess,DWORD cb,LPDWORD lpcbNeeded);
    static EnumProcessesFunc pfFuc = (EnumProcessesFunc)GetProcAddress((HMODULE)getPsapiLibrary(), "EnumProcesses");
    if (pfFuc == NULL)
        return false;

    return (*pfFuc)(lpidProcess,cb,lpcbNeeded);	
#else
    return false;
#endif
}

bool LoginWindowHelper::enumProcessModules(HANDLE hProcess, void** lphModule, DWORD cb, DWORD* lpcbNeeded)
{
#ifdef JUCE_WIN32
    typedef	BOOL (__stdcall *OBEnumProcessModulesFunc)(HANDLE hProcess,HMODULE *lphModule,DWORD cb,LPDWORD lpcbNeeded);
    static OBEnumProcessModulesFunc pfFuc = (OBEnumProcessModulesFunc)GetProcAddress((HMODULE)getPsapiLibrary(), "EnumProcessModules");
    if (pfFuc == NULL)
        return FALSE;

    return (*pfFuc)(hProcess,(HMODULE*)lphModule, cb, lpcbNeeded);	
#else
    return false;
#endif

}

DWORD LoginWindowHelper::getModuleFileNameExW(HANDLE hProcess, void* hModule, const wchar_t* lpFilename, DWORD nSize)
{
#ifdef JUCE_WIN32
    typedef	BOOL (__stdcall *OBGetModuleFileNameExWFunc)(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename,DWORD nSize);
    static OBGetModuleFileNameExWFunc pfFuc = (OBGetModuleFileNameExWFunc)GetProcAddress((HMODULE)getPsapiLibrary(), "GetModuleFileNameExW");
    if (pfFuc == NULL)
    {
        return FALSE;
    }

    return (*pfFuc)(hProcess, (HMODULE)hModule, (LPWSTR)lpFilename, nSize);
#else
    return false;
#endif
}

void* LoginWindowHelper::getPsapiLibrary()
{
#ifdef JUCE_WIN32
    static void* hPsapiDll = LoadLibraryW(L"Psapi.dll");
    return hPsapiDll;
#else
    return nullptr;
#endif
}

bool LoginWindowHelper::showLogin()
{
    if (!loginWindow)
    {
        loginWindow = new LoginWindow;
        loginWindow->enterModalState(true, nullptr, true);
    }
    return 0 != loginWindow->runModalLoop();
}

void LoginWindowHelper::showMain()
{
    mainWindow = new MainAppWindow();
    mainWindow->setVisible(true);
}

void LoginWindowHelper::relogin()
{
    CXOption::getInstance()->SetAutoLogin(false);

    ReadWindowHelper::getInstance()->closeWindow();

    if (mainWindow)
    {
        mainWindow->setVisible(false);
        mainWindow = nullptr;
    }

    if (showLogin())
    {
        showMain();
    }
}

void LoginWindowHelper::asyncRelogin()
{
    (new AsyncRelogin())->triggerAsyncUpdate();
}
