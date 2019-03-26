#pragma once
#include "HWDocBase.h"
#include "HWDefine.h"

class HWParaBookCommend : public HWParaUI
{
public:
    HWParaBookCommend(const char* strBook, std::vector<juce::String>& vecUsers, const wchar_t* strText)
        : m_strBook(strBook), m_vecUsers(vecUsers), m_strText(strText)
    {
    }

protected:
    friend class HWDocRecommend;
    HWParaBookCommend(){}

    const HWParas* ToHWPara()
    {
        Json::Value jv;
        for (unsigned int i=0; i<m_vecUsers.size(); ++i)
            jv.append(Json::Value(m_vecUsers[i].toUTF8()));
        m_Para.Add("users", jv.toStyledString().c_str());
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("text", m_strText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        Json::Value jv;
        Json::Reader jr;
        if (jr.parse(para->StrVal("users"), jv))
            for (unsigned int i=0; i<jv.size(); ++i)
                m_vecUsers.push_back(juce::CharPointer_UTF8(jv[i].asCString()));
        m_strBook = para->StrVal("guid");
        m_strText = para->StrVal("text");
        return true;
    }

private:
    juce::String m_strBook;
    std::vector<juce::String> m_vecUsers;
    juce::String m_strText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookCommend)
};

class HWParaClassCommend : public HWParaUI
{
public:
    HWParaClassCommend(std::vector<int>& vecClassIds, std::vector<juce::String>& vecBooks, const wchar_t* szText)
        : m_vecClassIds(vecClassIds), m_vecBooks(vecBooks), m_szText(szText)
    {
    }

protected:
    friend class HWDocRecommend;
    HWParaClassCommend(){}

    const HWParas* ToHWPara()
    {
        Json::Value jvClasses, jvBooks;
        for (unsigned int i=0; i<m_vecClassIds.size(); ++i)
            jvClasses.append(Json::Value(m_vecClassIds[i]));
        for (unsigned int i=0; i<m_vecBooks.size(); ++i)
            jvBooks.append(Json::Value(m_vecBooks[i].toUTF8()));

        m_Para.Add("classes", jvClasses.toStyledString().c_str());
        m_Para.Add("guids", jvBooks.toStyledString().c_str());
        m_Para.Add("text", m_szText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        Json::Value jv;
        Json::Reader jr;
        if (jr.parse(para->StrVal("classes"), jv))
        {
            for (unsigned int i=0; i<jv.size(); ++i)
                m_vecClassIds.push_back(jv[i].asInt());
        }

        if (jr.parse(para->StrVal("guids"), jv))
        {
            for (unsigned int i=0; i<jv.size(); ++i)
                m_vecBooks.push_back(juce::CharPointer_UTF8(jv[i].asCString()));
        }
        m_szText = para->StrVal("text");
        return true;
    }

private:
    std::vector<int> m_vecClassIds;
    std::vector<juce::String> m_vecBooks;
    juce::String m_szText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaClassCommend)
};

class HWDocRecommend :
    public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocRecommend, false)
    HWDocRecommend(void);
    ~HWDocRecommend(void);

    HWTCommendData* GetCommendData();
protected:
    virtual void OnInit();
    virtual void OnFinal();

    void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict);
    void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);
    HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

private:
    HWTCommendData* m_pCommendData;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocRecommend)
};

