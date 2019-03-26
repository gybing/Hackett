#pragma once

#include "HWDocBase.h"
#include "json.h"

class IHWSubjectBookSign
{
public:
    virtual void OnBookSignAdd() = 0;
    virtual void OnBookSignDel() = 0;
    virtual void OnBookSignListChanged() {}
};

class HWParaSignList : public HWParaUI
{
public:
    HWParaSignList(const juce::String& strBook)
        : m_strBook(strBook)
    {
    }

protected:
    friend class HWDocBookSign;
    HWParaSignList(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaSignList)
};

class HWParaSignAdd : public HWParaUI
{
public:
    HWParaSignAdd(const juce::String& strBook, int nPageNo, int nClr, int nBex, int nBec)
        : m_strBook(strBook)
        , m_nPageNo(nPageNo)
        , m_nClr(nClr)
        , m_nBex(nBex)
        , m_nBec(nBec)
    {
    }

protected:
    friend class HWDocBookSign;
    HWParaSignAdd(){}

    const HWParas* ToHWPara()
    {
        juce::String strFmt;
        strFmt << L"[" << m_nBex << L"," << m_nBec << L"]";

        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        m_Para.Add("clr", m_nClr);
        m_Para.Add("be", strFmt.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        m_nClr = para->IntVal("clr");
        Json::Reader jr;
        Json::Value jv;
        if (jr.parse(para->StrVal("be"), jv) &&
            jv.isArray() && jv.size() == 2)
        {
            m_nBex = jv[0u].asInt();
            m_nBec = jv[1].asInt();
        }
        return true;
    }

private:
    juce::String m_strBook;
    int m_nPageNo;
    int m_nClr;
    int m_nBex;
    int m_nBec;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaSignAdd)
};

class HWParaSignDel : public HWParaUI
{
public:
    HWParaSignDel(const juce::String& strBook, int nID)
        : m_strBook(strBook)
        , m_nID(nID)
    {
    }

protected:
    friend class HWDocBookSign;
    HWParaSignDel(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("id", m_nID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nID = para->IntVal("id");
        return true;
    }

private:
    juce::String m_strBook;
    int m_nID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaSignDel)
};

class HWDocBookSign
    : public HWDocBase
    , public HWSubject<IHWSubjectBookSign>
{
public:
    juce_DeclareSingleton(HWDocBookSign, false)
    HWDocBookSign();
    virtual ~HWDocBookSign();

	void GetPageSignList(int nPage, std::vector<HWTBookSign*>& vecSigns);
    int GetBookSignCount();
    const HWTBookSign* GetBookSign(int nIdx);

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    std::vector<HWTBookSign*>* m_pSignList;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBookSign)
};