#pragma once

#include "HWDocBase.h"
#include "json.h"
#include "xzcrypt.h"

class HWParaUserHead : public HWParaUI
{
public:
    HWParaUserHead(const juce::String& szUserID)
        : m_strUserID(szUserID)
    {
        HWDocHelper::getInstance()->GetHeadPicRequestUrl(m_strUrl, szUserID.toUTF8());
    }

protected:
    friend class HWDocUserData;
    HWParaUserHead(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("url", m_strUrl.toUTF8());
        m_Para.Add("userid", m_strUserID.toUTF8());
        m_Para.Add("cachetime", 0);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("url") || !para->Exist("userid"))
            return false;

        m_strUrl = para->StrVal("url");
        m_strUserID = para->StrVal("userid");
        return true;
    }

private:
    juce::String m_strUrl;
    juce::String m_strUserID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUserHead)
};

class HWParaUserInfo : public HWParaUI
{
public:
    HWParaUserInfo(const juce::String& szUserID)
        : m_strUserID(szUserID)
    {
    }

protected:
    friend class HWDocUserData;
    HWParaUserInfo(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("usrid", m_strUserID.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strUserID = para->StrVal("usrid");
        return true;
    }

private:
    juce::String m_strUserID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUserInfo)
};

class HWDocUserData : public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocUserData, false)
    HWDocUserData();
    virtual ~HWDocUserData();

    juce::Image* GetUserHead();
    HWTUserInfo* GetUserInfo();

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx);
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    juce::Image* m_pBitmap;
    HWTUserInfo* m_pUserInfo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocUserData)
};