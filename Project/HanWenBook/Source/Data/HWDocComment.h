#pragma once

#include "HWDocBase.h"
#include "json.h"

class HWParaCommentList : public HWParaUI
{
public:
    HWParaCommentList(const char* szBook)
        : m_strBook(szBook)
    {
    }

protected:
    friend class HWDocComment;
    HWParaCommentList(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaCommentList)
};

class HWParaCommentAdd : public HWParaUI
{
public:
    HWParaCommentAdd(const char* szBook, const wchar_t* strText)
        : m_strBook(szBook), m_strText(strText)
    {
    }

protected:
    friend class HWDocComment;
    HWParaCommentAdd(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("text", m_strText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_strText = para->StrVal("text");
        return true;
    }

private:
    juce::String m_strBook;
    juce::String m_strText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaCommentAdd)
};

class HWParaCommentDel : public HWParaUI
{
public:
    HWParaCommentDel(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocComment;
    HWParaCommentDel(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaCommentDel)
};

class HWDocComment : public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocComment, false)
    HWDocComment();
    virtual ~HWDocComment();

    std::vector<HWTBookComment*>* GetCommentList();
    HWTBookComment* GetComment(int nID);
protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    std::vector<HWTBookComment*>* m_pVecCommentList;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocComment)
};