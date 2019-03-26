#pragma once

#include "HWDocBase.h"
#include "json.h"

class IHWSubjectBookPostil
{
public:
    virtual void OnBookPostilAdd() = 0;
};

class HWParaBookPostilList : public HWParaUI
{
public:
    HWParaBookPostilList(const juce::String& strBook)
        : m_strBook(strBook)
    {
    }

protected:
    friend class HWDocBookPostil;
    HWParaBookPostilList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        return true;
    }

private:
    juce::String m_strBook;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookPostilList)
};

class HWParaBookPostilAdd : public HWParaUI
{
public:
    HWParaBookPostilAdd(const char* strBook, int nPageNo, int nZone, int nBex, int nBec, const wchar_t* strText)
        : m_strBook(strBook), m_nPageNo(nPageNo), m_nZone(nZone), m_nBeX(nBex), m_nBeC(nBec), m_strText(strText)
    {
    }

protected:
    friend class HWDocBookPostil;
    HWParaBookPostilAdd(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        m_Para.Add("zone", m_nZone);

        if (m_nBeC!=0 || m_nBeX!=0)
        {
            Json::Value jv;
            jv.append(Json::Value(m_nBeX));
            jv.append(Json::Value(m_nBeC));
            m_Para.Add("be", jv.toStyledString().c_str());
        }
        m_Para.Add("text", m_strText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        m_nZone = para->IntVal("zone");

        Json::Reader jr;
        Json::Value jv;
        if (jr.parse(para->StrVal("be"), jv))
        {
            m_nBeX = jv[0u].asInt();
            m_nBeC = jv[1].asInt();
        }
        m_strText = para->StrVal("text");
        return true;
    }

private:
    juce::String m_strBook;
    int m_nPageNo;
    int m_nZone;
    int m_nBeX;
    int m_nBeC;
    juce::String m_strText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookPostilAdd)
};

class HWParaBookPostilDel : public HWParaUI
{
public:
    HWParaBookPostilDel(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocBookPostil;
    HWParaBookPostilDel(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        return true;
    }

private:
    int m_nID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookPostilDel)
};

class HWParaPostilPageArea : public HWParaUI
{
public:
    HWParaPostilPageArea(const juce::String& strBook, int nPageNo)
        : m_strBook(strBook), m_nPageNo(nPageNo)
    {
    }

protected:
    friend class HWDocBookPostil;
    HWParaPostilPageArea(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        return true;
    }

private:
    juce::String m_strBook;
    int m_nPageNo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaPostilPageArea)
};

class HWParaAreaPostil : public HWParaUI
{
public:
    HWParaAreaPostil(const char* strBook, int nPageNo, int nZone)
        : m_strBook(strBook), m_nPageNo(nPageNo), m_nZone(nZone)
    {
    }

protected:
    friend class HWDocBookPostil;
    HWParaAreaPostil(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        m_Para.Add("zone", m_nZone);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        m_nZone = para->IntVal("zone");
        return true;
    }

private:
    juce::String m_strBook;
    int m_nPageNo;
    int m_nZone;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaAreaPostil)
};

class HWDocBookPostil 
    : public HWDocBase
    , public HWSubject<IHWSubjectBookPostil>
{
public:
    juce_DeclareSingleton(HWDocBookPostil, false)
    HWDocBookPostil();
    virtual ~HWDocBookPostil();

    void GetPagePostils(int nPage, std::vector<HWTBookPostil*>& vecPostils);
    std::map<int, std::pair<int,int>>* GetPostilPageArea();
    std::vector<HWTBookPostil*>* GetPostilList();
    HWTBookPostil* GetPostil(int nID);
    std::pair<std::vector<int>, std::vector<int>>* GetAreaPostilIDs();
protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    std::vector<HWTBookPostil*>* m_pVecBookPostils;
    std::map<int, std::pair<int,int>>* m_pMapPostilPageArea;
    std::pair<std::vector<int>, std::vector<int>>* m_pVecAreaPostils;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBookPostil)
};