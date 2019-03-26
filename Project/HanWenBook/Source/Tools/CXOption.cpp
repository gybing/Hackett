#include "JuceHeader.h"
#include "CXOption.h"
#include "knltoken.h"
#include "md5new.h"

#ifdef JUCE_WINDOWS
#include "MachineID.h"
#endif


const juce::String CXPropertiesLoginUserName = L"LoginUserName";
const juce::String CXPropertiesLoginUserToken = L"LoginUserToken";
const juce::String CXPropertiesLoginUserAutoLogin = L"LoginUserAutoLogin";

const juce::String CXPropertiesSystemExitMode = L"SystemExitMode";
const juce::String CXPropertiesSystemReadRightMenuPopup = L"SystemReadRightMenuAutoPopup";
const juce::String CXPropertiesSystemBookPageBackColorProtect = L"SystemBookPageBackColorProtect";
const juce::String CXPropertiesSystemReadFontSmooth = L"SystemReadFontSmooth";
const juce::String CXPropertiesSystemReadBrowserHor = L"SystemReadBrowserHor";
const juce::String CXPropertiesSystemReadBookRate = L"SystemReadBookRate";
const juce::String CXPropertiesSystemReadCompact = L"SystemReadCompact";

juce_ImplementSingleton(CXOption)

CXOption::CXOption()
: appProperties(NULL)
, m_cfgUserLogin(new CXUserLoginConfig)
, m_cfgSystemConfig(new CXSystemConfig)
{
    GetAppProperties();
    LoadDefaultConfig();
}

CXOption::~CXOption()
{
    SAFE_DELETE(m_cfgUserLogin);
    SAFE_DELETE(m_cfgSystemConfig);
    if (appProperties)
    {
        appProperties->closeFiles();
        juce::deleteAndZero (appProperties);
    }
}

void CXOption::LoadDefaultConfig()
{
    runApplicationDebugMode = false;

    m_cfgUserLogin->strUserID = "";
    m_cfgUserLogin->strUserToken = "";
    m_cfgUserLogin->bAotuLogin = false; // 是否自动登录
        
    m_cfgSystemConfig->dwSysExitType = CXSET_HIDE | CXSET_ASK; // CXEnumSysExitType
    m_cfgSystemConfig->bBookPageBackColorProtect = 0;
    m_cfgSystemConfig->bReadRightMenuAutoPopup = 1;
    m_cfgSystemConfig->bReadFontSmooth = 1;
    
    m_cfgSystemConfig->bReadBrowserHor = 0;
    m_cfgSystemConfig->nReadBookRate = 2;
    m_cfgSystemConfig->bReadCompact = 0;
    m_cfgSystemConfig->bReadExpandShow = false;

    float marginX = 0.12f * ProjectInfo::DefaultBookPanelWidth;
    const float stepX = (ProjectInfo::DefaultBookPanelWidth - marginX) / 8.0f;
    marginX /= 2.0f;
    const float posX[4] = {marginX+stepX, marginX+(stepX*3.0f), marginX+(stepX*5.0f), marginX+(stepX*7.0f)};
    const float posY[4] = {0.267f*ProjectInfo::DefaultBookPanelHeight, 0.497f*ProjectInfo::DefaultBookPanelHeight, 0.728f*ProjectInfo::DefaultBookPanelHeight, 0.959f*ProjectInfo::DefaultBookPanelHeight};
    CaculateBookPosition(posX, posY, ProjectInfo::DefaultBookWidth, ProjectInfo::DefaultBookHeight);
}

bool CXOption::Initialize()
{
    InitApplicationConfig();
    InitUserLoginConfig();
    InitSystemConfig();
    return true;
}

void CXOption::Finalize()
{
    // SaveUserLoginConfig();
}

void CXOption::InitApplicationConfig()
{
}

juce::Point<int> CXOption::GetMainWndSize()
{
    juce::Point<int> szDeskTopClient, szMainWnd;
    const juce::Desktop::Displays& display = juce::Desktop::getInstance().getDisplays();

    szDeskTopClient.x = display.getMainDisplay().totalArea.getWidth();
    szDeskTopClient.y = display.getMainDisplay().totalArea.getHeight();
    if (szDeskTopClient.x <= ProjectInfo::DefaultFrameMaxWidth || szDeskTopClient.y <= ProjectInfo::DefaultFrameMaxHeight)
    {
        if (szDeskTopClient.x <= ProjectInfo::DefaultFrameMinWidth || szDeskTopClient.y <= ProjectInfo::DefaultFrameMinHeight)
        {
            juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                L"屏幕分辨率太小", L"信息提示");
        }
        szMainWnd.x = ProjectInfo::DefaultFrameMinWidth;
        szMainWnd.y = ProjectInfo::DefaultFrameMinHeight;
    }
    else
    {
        szMainWnd.x = ProjectInfo::DefaultFrameMaxWidth;
        szMainWnd.y = ProjectInfo::DefaultFrameMaxHeight;
    }

    return szMainWnd;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CXOption::InitUserLoginConfig()
{
    m_cfgUserLogin->strUserID = appProperties->getUserSettings()->getValue(CXPropertiesLoginUserName);
    m_cfgUserLogin->strUserToken = appProperties->getUserSettings()->getValue(CXPropertiesLoginUserToken);
    m_cfgUserLogin->bAotuLogin = (bool)appProperties->getUserSettings()->getIntValue(CXPropertiesLoginUserName);
}

void CXOption::SetCurrentUserInfo(const CXUserLoginConfig& lui )
{
    m_cfgUserLogin->bAotuLogin = lui.bAotuLogin;
    m_cfgUserLogin->strUserID = lui.strUserID;
    m_cfgUserLogin->strUserToken = lui.strUserToken;
}

const CXUserLoginConfig* CXOption::GetCurrentUserInfo()
{
    return m_cfgUserLogin;
}

void CXOption::SaveUserLoginConfig()
{
    if (!m_cfgUserLogin->strUserID.isEmpty())
    {
        appProperties->getUserSettings()->setValue(CXPropertiesLoginUserName, m_cfgUserLogin->strUserID);
        
        if (!m_cfgUserLogin->strUserToken.isEmpty() && m_cfgUserLogin->bAotuLogin)
        {
            appProperties->getUserSettings()->setValue(CXPropertiesLoginUserToken, m_cfgUserLogin->strUserToken);
            appProperties->getUserSettings()->setValue(CXPropertiesLoginUserAutoLogin, 1);
        }
        else
        {
            appProperties->getUserSettings()->setValue(CXPropertiesLoginUserToken, juce::var());
            appProperties->getUserSettings()->setValue(CXPropertiesLoginUserAutoLogin, 0);
        }
    }
}

bool CXOption::IsAutoLogin()
{
    return m_cfgUserLogin->bAotuLogin && !m_cfgUserLogin->strUserToken.isEmpty();
}

void CXOption::SetAutoLogin( bool bAuto )
{
    m_cfgUserLogin->bAotuLogin = bAuto;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

juce::ApplicationProperties* CXOption::GetAppProperties()
{
    if (!appProperties)
    {
        appProperties = new juce::ApplicationProperties;
        juce::PropertiesFile::Options options;
        options.applicationName = L"application";
        options.filenameSuffix = L"settings";
#ifdef JUCE_WINDOWS
        options.folderName = GetUserDataPath();
#else if JUCE_MAC
        options.osxLibrarySubFolder = L"Application Support/HanWenBook";
#endif // JUCE_WINDOWS
        appProperties->setStorageParameters(options);
    }
    
    return appProperties;
}

bool CXOption::getApplicationDebugMode()
{
    return runApplicationDebugMode;
}

void CXOption::setApplicationDebugMode(bool b)
{
    runApplicationDebugMode = b;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CXOption::DeviceID(juce::String& strID)
{
    char mid[25];
    char deviceid[25];
    //GetMachineID(mid);
    GetDeviceID(deviceid, mid, DEVICE_WIN);
    strID = deviceid;
}

juce::String CXOption::PasswordMd5( const char* strPassword )
{
    char passMD5[USER_PASSMD5_LEN + 1] = {0};
    GetPassMD5(passMD5, strPassword);
    return passMD5;
}

juce::String CXOption::DeviceString( HWEnum_Device enDev )
{
    switch (enDev)
    {
        case HWDevice_PC:       return L"PC客户端";
        case HWDevice_WinPad:   return L"WinPad客户端";
        case HWDevice_WinPhone: return L"WinPhone客户端";
        case HWDevice_MacOS:    return L"MacOS客户端";
        case HWDevice_IPad:     return L"IPad客户端";
        case HWDevice_IPhone:   return L"IPhone客户端";
        case HWDevice_Linux:    return L"Linux客户端";
        case HWDevice_ArdPad:   return L"AndroidPad客户端";
        case HWDevice_ArdPhone: return L"AndroidPhone客户端";
        default:                return L"客户端";
    }
}


bool CXOption::ParseUrl( const juce::String& url, juce::String& uid, juce::String& token )
{
    juce::URL strURL = juce::URL::createWithoutParsing(url);
    if (strURL.getDomain() == ProjectInfo::LoginWebCallback)
    {
        juce::String str = url.fromFirstOccurrenceOf("uid=", false, false);
        if (!str.isEmpty())
        {
            token = str.fromFirstOccurrenceOf("&token=", false, false);
            uid = str.upToFirstOccurrenceOf("&token=", false, false);
            return !token.isEmpty() && !uid.isEmpty();
        }

    }
    return false;
}

bool CXOption::ParseArgv( juce::String &uid, juce::String& token, const char* szCmdLine )
{
    return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

juce::String CXOption::GetUserDataPath()
{
#ifdef JUCE_WINDOWS
    juce::File file = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    file.getChildFile (juce::String(L"HanWenBook")).createDirectory();
    return file.getFullPathName() + L"\\HanWenBook";
#else if JUCE_MAC
    return juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getChildFile(juce::String(L"Contents/Support")).getFullPathName();
#endif // JUCE_WINDOWS
    
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CXOption::InitSystemConfig()
{
    // 取退出配置
    const int defaultValue = -1;
    
    int value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemExitMode, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->dwSysExitType = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemReadRightMenuPopup, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->bReadRightMenuAutoPopup = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemBookPageBackColorProtect, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->bBookPageBackColorProtect = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemReadFontSmooth, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->bReadFontSmooth = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemReadBrowserHor, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->bReadBrowserHor = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemReadBookRate, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->nReadBookRate = value;
    value = appProperties->getUserSettings()->getIntValue(CXPropertiesSystemReadCompact, defaultValue);
    if (value != defaultValue)
        m_cfgSystemConfig->bReadCompact = value;
}

void CXOption::SaveSystemConfig()
{
    appProperties->getUserSettings()->setValue(CXPropertiesSystemExitMode, m_cfgSystemConfig->dwSysExitType);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemReadRightMenuPopup, m_cfgSystemConfig->bReadRightMenuAutoPopup);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemBookPageBackColorProtect, m_cfgSystemConfig->bBookPageBackColorProtect);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemReadFontSmooth, m_cfgSystemConfig->bReadFontSmooth);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemReadBrowserHor, m_cfgSystemConfig->bReadBrowserHor);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemReadBookRate, m_cfgSystemConfig->nReadBookRate);
    appProperties->getUserSettings()->setValue(CXPropertiesSystemReadCompact, m_cfgSystemConfig->bReadCompact);
}

int CXOption::GetSystemExitType()
{
    return m_cfgSystemConfig->dwSysExitType;
}

void CXOption::SetSystemExitType(int en)
{
    m_cfgSystemConfig->dwSysExitType = en;
}


bool CXOption::GetBookPageBackColor()
{
    return m_cfgSystemConfig->bBookPageBackColorProtect;
}

void CXOption::SetBookPageBackColor( bool bProtect )
{
    m_cfgSystemConfig->bBookPageBackColorProtect = bProtect;
}


bool CXOption::GetReadDeskRightMenuAutoPop()
{
    return m_cfgSystemConfig->bReadRightMenuAutoPopup;
}

void CXOption::SetReadDeskRightMenuAutoPop(bool bAutoPop)
{
    m_cfgSystemConfig->bReadRightMenuAutoPopup = bAutoPop;
}

bool CXOption::GetReadFontSmooth()
{
    return m_cfgSystemConfig->bReadFontSmooth;
}

void CXOption::SetReadFontSmooth(bool bSmooth)
{
    m_cfgSystemConfig->bReadFontSmooth = bSmooth;
}

bool CXOption::GetReadBrowserTypeHor()
{
    return m_cfgSystemConfig->bReadBrowserHor;
}

void CXOption::SetReadBrowserTypeHor(bool bHor)
{
    m_cfgSystemConfig->bReadBrowserHor = bHor;
    SaveSystemConfig();
}

bool CXOption::GetReadExpandShow()
{
    return m_cfgSystemConfig->bReadExpandShow;
}

void CXOption::SetReadExpandShow(bool b)
{
    m_cfgSystemConfig->bReadExpandShow = b;
}

int CXOption::GetReadBookRate()
{
    return m_cfgSystemConfig->nReadBookRate;
}

void CXOption::SetReadBookRate(int nRate)
{
    m_cfgSystemConfig->nReadBookRate = nRate;
    SaveSystemConfig();
}

bool CXOption::GetReadCompact()
{
    return m_cfgSystemConfig->bReadCompact;
}

void CXOption::SetReadCompact(bool bCompact)
{
    m_cfgSystemConfig->bReadCompact = bCompact;
}

//////////////////////////////////////////////////////////////////////////

void CXOption::CaculateBookPosition(const float* px, const float* py, const float width, const float height)
{
    for (unsigned int y=0; y<4; ++y)
    {
        for (unsigned int x = 0; x < 4; ++x)
        {
            int index = (y*4)+x;
            m_rcBookBounds[index].setBounds(px[x] - (width / 2.0f), py[y] - height, width, height);
        }
    }
}

const juce::Rectangle<int>& CXOption::GetBookBounds(int n) const
{
    jassert(n>=0 && n<16);
    return m_rcBookBounds[n];
}

juce::Image CXOption::GetImage(const wchar_t* szFile, int index, int count, bool vertical)
{
    static struct
    {
        juce::String strImageName;
        juce::Image image;
        int nImageWidth;
        int nImageHeigt;
        juce::CriticalSection lock;
        
    }LoadImagePara;
    
    juce::ScopedLock lk(LoadImagePara.lock);
    
    if (LoadImagePara.strImageName != szFile)
    {
        LoadImagePara.strImageName = szFile;
        LoadImagePara.image = CXIMAGE_CACHE_LOAD(szFile);
        LoadImagePara.nImageWidth = LoadImagePara.image.getWidth();
        LoadImagePara.nImageHeigt = LoadImagePara.image.getHeight();
    }
    
    return vertical ? LoadImagePara.image.getClippedImage(juce::Rectangle<int>(
                                                                               0,
                                                                               index*(LoadImagePara.nImageHeigt/count),
                                                                               LoadImagePara.nImageWidth,
                                                                               LoadImagePara.nImageHeigt/count)) :
    LoadImagePara.image.getClippedImage(juce::Rectangle<int>(
                                                             index*(LoadImagePara.nImageWidth/count),
                                                             0,
                                                             LoadImagePara.nImageWidth/count,
                                                             LoadImagePara.nImageHeigt));
}

juce::Image CXOption::loadImageFromMemory(const void* data, int width, int height, bool hasAlphaChan/* = false*/)
{
    // now convert the data to a juce image format..
    juce::Image image (hasAlphaChan ? juce::Image::ARGB : juce::Image::RGB, width, height, hasAlphaChan);

    image.getProperties()->set ("originalImageHadAlpha", image.hasAlphaChannel());
    hasAlphaChan = image.hasAlphaChannel(); // (the native image creator may not give back what we expect)

    const juce::Image::BitmapData destData (image, juce::Image::BitmapData::writeOnly);

    for (int y = 0; y < (int) height; ++y)
    {
        const juce::uint8* src = ((const juce::uint8*)data) + (y * width * 4);
        juce::uint8* dest = destData.getLinePointer (y);

        if (hasAlphaChan)
        {
            for (int i = (int) width; --i >= 0;)
            {
                ((juce::PixelARGB*) dest)->setARGB (src[3], src[2], src[1], src[0]);
                ((juce::PixelARGB*) dest)->premultiply();
                dest += destData.pixelStride;
                src += 4;
            }
        }
        else
        {
            for (int i = (int) width; --i >= 0;)
            {
                ((juce::PixelRGB*) dest)->setARGB (0, src[0], src[1], src[2]);
                dest += destData.pixelStride;
                src += 4;
            }
        }
    }

    return image;
}
