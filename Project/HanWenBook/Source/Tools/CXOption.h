#pragma once

struct CXUserLoginConfig
{
	juce::String strUserID;
	juce::String strUserToken;
	bool bAotuLogin;
};

struct CXSystemConfig
{
    int dwSysExitType; // CXEnumSysExitType
    int bBookPageBackColorProtect;
    int bReadRightMenuAutoPopup;
    int bReadFontSmooth;

    int bReadBrowserHor;
    int nReadBookRate;
    int bReadCompact;

    bool bReadExpandShow;
};

class CXOption
{
public:
    juce_DeclareSingleton(CXOption, false);
	CXOption();
	virtual ~CXOption();

	bool Initialize();
	void Finalize();

    void LoadDefaultConfig();

	void InitApplicationConfig();
	juce::Point<int> GetMainWndSize();

	void InitUserLoginConfig();
	void SetCurrentUserInfo(const CXUserLoginConfig& lui);
	const CXUserLoginConfig* GetCurrentUserInfo();
	bool IsAutoLogin();
	void SetAutoLogin(bool bAuto);
	void SaveUserLoginConfig();

    void InitSystemConfig();
    void SaveSystemConfig();
    int GetSystemExitType();
    void SetSystemExitType(int en);

    bool GetBookPageBackColor();
    void SetBookPageBackColor(bool bProtect);
    bool GetReadDeskRightMenuAutoPop();
    void SetReadDeskRightMenuAutoPop(bool bAutoPop);
    bool GetReadFontSmooth();
    void SetReadFontSmooth(bool bSmooth);
    bool GetReadBrowserTypeHor();
    void SetReadBrowserTypeHor(bool bHor);
    bool GetReadExpandShow();
    void SetReadExpandShow(bool b);
    int GetReadBookRate();
    void SetReadBookRate(int nRate);
    bool GetReadCompact();
    void SetReadCompact(bool bCompact);

    juce::ApplicationProperties* GetAppProperties();

    bool getApplicationDebugMode();
    void setApplicationDebugMode(bool b);

public:
	static void DeviceID(juce::String& strID);
	static juce::String PasswordMd5(const char* strPassword);
	static juce::String DeviceString(HWEnum_Device enDev);
    static bool ParseUrl(const juce::String& url, juce::String& uid, juce::String& token);
    static bool ParseArgv(juce::String &uid, juce::String& token, const char* szCmdLine);
    static juce::Image GetImage(const wchar_t* szFile, int index, int count, bool vertical);
    static juce::Image loadImageFromMemory(const void* data, int width, int height, bool hasAlphaChan = false);

	juce::String GetUserDataPath();

    void CaculateBookPosition(const float* px, const float* py, const float width, const float height);
    const juce::Rectangle<int>& GetBookBounds(int n) const;

private:
	CXUserLoginConfig* m_cfgUserLogin;
    CXSystemConfig* m_cfgSystemConfig;

private:
	juce::ApplicationProperties* appProperties;
	juce::String m_strAppRootPath;
    bool m_bInited;

private:
    juce::Rectangle<int> m_rcBookBounds[16];
    bool runApplicationDebugMode;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXOption)
};
