#pragma once
#include "HWDocBase.h"
#include "HWDefine.h"

class HWParaVsnCheck : public HWParaUI
{
public:
    HWParaVsnCheck(HWEnum_Device type, const juce::String& vsn, bool btest = false)
        : m_DeviceType(type), m_strVsn(vsn), m_bForTest(btest)
    {
    }

protected:
    friend class HWDocAuth;
    HWParaVsnCheck(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("devicetype", (int)m_DeviceType);
        m_Para.Add("version", m_strVsn.toUTF8());
        if (m_bForTest)
        {
            m_Para.Add("fortest", 1);
        }
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("devicetype") || !para->Exist("version"))
            return false;

        m_DeviceType = (HWEnum_Device)para->IntVal("devicetype");
        m_strVsn = juce::CharPointer_UTF8(para->StrVal("version"));
        m_bForTest = para->IntVal("fortest") == 1;
        return true;
    }

private:
    HWEnum_Device m_DeviceType;
    juce::String m_strVsn;
    bool m_bForTest;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaVsnCheck)
};

class HWParaLogin : public HWParaUI
{
public:
    HWParaLogin(const juce::String& szUsrId, const juce::String& szToken, HWEnum_StartupType tp)
        : m_strUsrID(szUsrId), m_strToken(szToken), m_nStartupType(tp)
    {
    }

protected:
    friend class HWDocAuth;
    HWParaLogin(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("usrid", m_strUsrID.toUTF8());
        m_Para.Add("reqtoken", m_strToken.toUTF8());
        m_Para.Add("startuptype", m_nStartupType);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strUsrID = juce::CharPointer_UTF8(para->StrVal("usrid"));
        m_strToken = juce::CharPointer_UTF8(para->StrVal("reqtoken"));
        m_nStartupType = (HWEnum_StartupType) para->IntVal("startuptype");
        return true;
    }

public:
    juce::String m_strUsrID;
    juce::String m_strToken;
    HWEnum_StartupType m_nStartupType;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaLogin)
};

class HWParaLogout : public HWParaUI
{
public:
    HWParaLogout(){}

protected:
    friend class HWDocAuth;

    const HWParas* ToHWPara(){ return NULL; }
    bool FromHWPara(const HWParas* para){ return true; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaLogout)
};

class HWParaModPsw : public HWParaUI
{
public:
    HWParaModPsw(const char* szShelfNo, const char* szOldPswMd5, const char* szNewPsw, const char* szPhone = "")
    {
        m_strUsrID = szShelfNo;
        m_strPswMd5 = szOldPswMd5;
        m_strNewPsw = szNewPsw;
        m_strUsrPhone = szPhone;
    }

protected:
    friend class HWDocAuth;
    HWParaModPsw(){}

    const HWParas* ToHWPara() 
    {
        if (!m_strUsrPhone.isEmpty())
            m_Para.Add("phoneno", m_strUsrPhone.toUTF8());
        else
            m_Para.Add("shelfno", m_strUsrID.toUTF8());
        m_Para.Add("oldpassmd5", m_strPswMd5.toUTF8());
        m_Para.Add("newpass", m_strNewPsw.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(para->Exist("phoneno"))
            m_strUsrPhone = juce::CharPointer_UTF8(para->StrVal("phoneno"));
        else m_strUsrID = juce::CharPointer_UTF8(para->StrVal("shelfno"));

        m_strPswMd5 = juce::CharPointer_UTF8(para->StrVal("oldpassmd5"));
        m_strNewPsw = juce::CharPointer_UTF8(para->StrVal("newpass"));
        return true;
    }

    juce::String m_strUsrID;
    juce::String m_strUsrPhone;
    juce::String m_strPswMd5;
    juce::String m_strNewPsw;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaModPsw)
};


class HWDocAuth :
    public HWDocBase
{
public:
    HWDocAuth(void);
    ~HWDocAuth(void);

    HWEnum_UserType GetUserType();
    const juce::String& GetShelfNo();
    const juce::String& GetUserName();
    HWTUserInfo* GetCurrUserInfo();

    HWTCheckVersion* GetCheckVersion();
    HWEnum_StartupType GetStartupType();
    void SetStartupType(HWEnum_StartupType tp);

    juce::String GetCurrentUserToken();
    juce::String GetCurrentUserID();
    juce::String GetCurrentUerChk();

    juce_DeclareSingleton(HWDocAuth, false)
protected:
    virtual void OnInit();
    virtual void OnFinal();

    void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict);
    void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);
    HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

private:
    HWTCheckVersion* m_pCheckVersion;
    HWTUserInfo* m_pUserInfo;
    HWEnum_StartupType m_enStartupType;

    juce::String m_strToken;
    juce::String m_strUserID;
    juce::String m_strUserChk;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocAuth)
};

