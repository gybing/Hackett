#include "JuceHeader.h"
#include "HWDocAuth.h"
juce_ImplementSingleton(HWDocAuth)

void* HWDocAuth::OnRespPreProcess( HWEnum_ReqID id, const HWParas* paras, const HWDict* dict )
{
    void* result = NULL;

    if (id == HWReq_Auth_Login)
    {
        HWDict* pCookies = NULL;
        if (dict->find("cookies") != dict->end())
        {
            pCookies = new HWDict;
            *pCookies = *HWDictElemDict(dict, "cookies");
        }

        HWTUserInfo* pUserInfo = new HWTUserInfo;
        pUserInfo->FromHWDict(HWDictElemDict(dict, "userinfo"));
        result = new std::pair<void*,void*>(pUserInfo, pCookies);
    }
    else if (id == HWReq_System_CheckVersion)
    {
        if (dict && !dict->empty())
        {
            HWTCheckVersion* pCheckVersion = new HWTCheckVersion;
            pCheckVersion->FromHWDict(dict);
            result = pCheckVersion;
        }
    }
    
    return result;
}

void HWDocAuth::OnRespProcess( HWEnum_ReqID id, const HWParas* paras, void* data )
{
    if (id == HWReq_Auth_Login)
    {
        SAFE_DELETE(m_pUserInfo);

        std::pair<void*,void*>* pResult = (std::pair<void*,void*>*)data;
        m_pUserInfo = (HWTUserInfo*)pResult->first;
        if (pResult->second != NULL)
        {
            HWDict* pCookies = (HWDict*)pResult->second;
            m_strToken = HWDictElemString(pCookies, "usertoken");
            m_strUserID = m_pUserInfo->strShelfNo;
            m_strUserChk = HWDictElemString(pCookies, "userchk");

#ifdef JUCE_MAC
            juce::WebBrowserComponent::setCookie(L"usertoken", m_strToken);
            juce::WebBrowserComponent::setCookie(L"userchk", m_strUserChk);
            juce::WebBrowserComponent::setCookie(L"userid", m_strUserID);
#else if JUCE_WINDOWS
            CEFBrowserControl::SetCookie(L".hanwenbook.com", L"usertoken", m_strToken.toUTF16());
            CEFBrowserControl::SetCookie(L".hanwenbook.com", L"userchk", m_strUserChk.toUTF16());
            CEFBrowserControl::SetCookie(L".hanwenbook.com", L"userid", m_strUserID.toUTF16());
#endif
        }
    }
    else if (id == HWReq_System_CheckVersion)
    {
        SAFE_DELETE(m_pCheckVersion);
        m_pCheckVersion = (HWTCheckVersion*)data;
    }
}

HWParaUI* HWDocAuth::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch(reqID)
    {
    case HWReq_System_CheckVersion:
        {
            HWParaVsnCheck* paraUI = new HWParaVsnCheck();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Auth_Login:
        {
            HWParaLogin* paraUI = new HWParaLogin;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Auth_Logout:
        {
            HWParaLogout* paraUI = new HWParaLogout;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

HWDocAuth::HWDocAuth( void )
{
}

HWDocAuth::~HWDocAuth( void )
{
}

void HWDocAuth::OnInit()
{
    m_pUserInfo = NULL;
    m_pCheckVersion = NULL;
}

void HWDocAuth::OnFinal()
{
    SAFE_DELETE(m_pUserInfo);
    SAFE_DELETE(m_pCheckVersion);
}

const juce::String& HWDocAuth::GetShelfNo()
{
    return m_pUserInfo->strShelfNo;
}

const juce::String& HWDocAuth::GetUserName()
{
    return m_pUserInfo->strName;
}

HWTUserInfo* HWDocAuth::GetCurrUserInfo()
{
    return m_pUserInfo;
}

HWEnum_UserType HWDocAuth::GetUserType()
{
    if (m_pUserInfo)
    {
        return m_pUserInfo->enUserType;
    }
    return HWUserType_Student;
}

HWTCheckVersion* HWDocAuth::GetCheckVersion()
{
    return m_pCheckVersion;
}

HWEnum_StartupType HWDocAuth::GetStartupType()
{
    return m_enStartupType;
}

void HWDocAuth::SetStartupType(HWEnum_StartupType tp)
{
    m_enStartupType = tp;
}

juce::String HWDocAuth::GetCurrentUserToken()
{
    return m_strToken;
}

juce::String HWDocAuth::GetCurrentUserID()
{
    return m_strUserID;
}

juce::String HWDocAuth::GetCurrentUerChk()
{
    return m_strUserChk;
}
