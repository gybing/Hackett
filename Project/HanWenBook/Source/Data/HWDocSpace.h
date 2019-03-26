#pragma once

#include "HWDocBase.h"
#include "json.h"

class HWParaClassStat : public HWParaUI
{
public:
    HWParaClassStat(int nClassID, const char* szBook)
        : m_nClassID(nClassID), m_strBook(szBook)
    {
    }

protected:
    friend class HWDocSpace;
    HWParaClassStat(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("classid", m_nClassID);
        m_Para.Add("guid", m_strBook.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nClassID = para->IntVal("classid");
        return true;
    }

private:
    int m_nClassID;
    juce::String m_strBook;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaClassStat)
};

class HWParaUserStat : public HWParaUI
{
public:
    HWParaUserStat(const char* szUserID)
        : m_strUserID(szUserID)
    {
    }

protected:
    friend class HWDocSpace;
    HWParaUserStat(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("shelfno", m_strUserID.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strUserID = para->StrVal("shelfno");
        return true;
    }

private:
    juce::String m_strUserID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUserStat)
};

class HWParaBookStat : public HWParaUI
{
public:
    HWParaBookStat(const juce::String& szBook)
    {
        m_vecBooks.push_back(szBook);
    }

    HWParaBookStat(std::vector<juce::String>& vecBooks)
        : m_vecBooks(vecBooks)
    {
    }

protected:
    friend class HWDocSpace;
    HWParaBookStat(){}

    const HWParas* ToHWPara()
    {
        Json::Value jvArr;
        for (unsigned int i=0; i<m_vecBooks.size(); ++i)
            jvArr.append(Json::Value((const char*)m_vecBooks[i].toUTF8()));
        m_Para.Add("guids", jvArr.toStyledString().c_str());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        Json::Reader jr;
        Json::Value jv;
        if (jr.parse(para->StrVal("guids"), jv))
        {
            for (unsigned int i=0; i<jv.size(); ++i)
                m_vecBooks.push_back(juce::CharPointer_UTF8(jv[i].asCString()));
        }
        return true;
    }

private:
    std::vector<juce::String> m_vecBooks;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookStat)
};

class HWDocSpace : public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocSpace, false)
    HWDocSpace();
    virtual ~HWDocSpace();

    HWTBookStat* GetBookStat(const juce::String& strBook);
    HWTUserStat* GetUserStat();
    HWTClassStat* GetClassStat(const juce::String& strUserID);
    std::map<juce::String, HWTClassStat*>* GetClassStat();
protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    HWTUserStat* m_pUserStat;
    std::map<juce::String, HWTBookStat*>* m_pMapBookStat;
    std::map<juce::String, HWTClassStat*>* m_pMapClassStat;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocSpace)
};