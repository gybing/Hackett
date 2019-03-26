#pragma once

#include "HWDocBase.h"
#include "json.h"

class IHWSubjectBookMark
{
public:
    virtual void OnBookMarkAdd() = 0;
    virtual void OnBookMarkDel() = 0;
};

class HWParaBookMarkList : public HWParaUI
{
public:
    HWParaBookMarkList(const juce::String& strBook)
        :  m_strBook(strBook)
    {
    }

protected:
    friend class HWDocBookMark;
    HWParaBookMarkList(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookMarkList)
};

class HWParaBookMarkAdd : public HWParaUI
{
public:
    HWParaBookMarkAdd(const juce::String& strBook, int nPageNo)
        : m_strBook(strBook)
        , m_nPageNo(nPageNo)
    {
    }

protected:
    friend class HWDocBookMark;
    HWParaBookMarkAdd(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookMarkAdd)
};

class HWParaBookMarkDel : public HWParaUI
{
public:
    HWParaBookMarkDel(const juce::String& strBook, int nPageNo)
        : m_strBook(strBook)
        , m_nPageNo(nPageNo)
    {
    }

protected:
    friend class HWDocBookMark;
    HWParaBookMarkDel(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookMarkDel)
};

class HWDocBookMark
    : public HWDocBase
    , public HWSubject<IHWSubjectBookMark>
{
public:
    juce_DeclareSingleton(HWDocBookMark, false)
    HWDocBookMark();
    virtual ~HWDocBookMark();

    bool HasBookMark(int nPageNo);
    int GetBookMarkCount();
    const HWTBookMark* GetBookMark(int nIdx);

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    std::vector<HWTBookMark*>* m_pVecBookMarks;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBookMark)
};