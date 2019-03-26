#pragma once

#include "HWDocBase.h"
#include "json.h"

class HWParaBookScoreGet : public HWParaUI
{
public:
    HWParaBookScoreGet(const juce::String& szBook)
        : m_strBook(szBook)
    {
    }

protected:
    friend class HWDocBookData;
    HWParaBookScoreGet(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookScoreGet)
};

class HWParaBookScoreSet : public HWParaUI
{
public:
    HWParaBookScoreSet(const char* szBook, int nScore)
        : m_strBook(szBook), m_nScore(nScore)
    {
    }

protected:
    friend class HWDocBookData;
    HWParaBookScoreSet(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("score", m_nScore);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nScore = para->IntVal("score");
        return true;
    }

private:
    juce::String m_strBook;
    int m_nScore;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookScoreSet)
};

class HWParaTextSearch : public HWParaUI
{
public:
    HWParaTextSearch(const juce::String& text, const juce::String& book, int pn = -1)
        : m_strText(text), m_strBook(book), m_nPn(pn)
    {
    }

protected:
    friend class HWDocBookData;
    HWParaTextSearch(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("ss", m_strText.toUTF8());
        m_Para.Add("guid", m_strBook.toUTF8());
        if (m_nPn != -1)
            m_Para.Add("pn", m_nPn);

        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strText = juce::CharPointer_UTF8(para->StrVal("ss"));
        m_strBook = juce::CharPointer_UTF8(para->StrVal("guid"));
        if (para->Exist("pn"))
        {
            m_nPn = para->IntVal("pn");
        }
        return true;
    }

private:
    juce::String m_strText;
    juce::String m_strBook;
    int m_nPn;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTextSearch)
};

class HWParaBookTextGet : public HWParaUI
{
public:
    HWParaBookTextGet(const juce::String& szBook, int nPageNo, int nBex, int nBec)
        : m_strBook(szBook), m_nPageNo(nPageNo), m_nBex(nBex), m_nBec(nBec)
    {
    }

protected:
    friend class HWDocBookData;
    HWParaBookTextGet(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        
        juce::String strFmt;
        strFmt << L"[" << m_nBex << L"," << m_nBec << L"]";
        m_Para.Add("be", strFmt.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
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
    int m_nBex;
    int m_nBec;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookTextGet)
};

class HWParaBookInfo : public HWParaUI
{
public:
    HWParaBookInfo(const juce::String& strBook)
    {
        m_strBook = strBook;
    }

protected:
    friend class HWDocBookData;
    HWParaBookInfo(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("guid"))
            return false;


        m_strBook = para->StrVal("guid");
        return true;
    }

public:
    juce::String m_strBook;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookInfo)
};

class HWParaBookRes : public HWParaUI
{
public:
    HWParaBookRes(const juce::String& szGuid)
    {
        m_strBook = szGuid;
    }

protected:
    friend class HWDocBookData;
    HWParaBookRes(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("guid"))
            return false;

        m_strBook = para->StrVal("guid");
        return true;
    }

public:
    juce::String m_strBook;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookRes)
};

class HWParaBookPic : public HWParaUI
{
public:
    HWParaBookPic(const juce::String& szGuid, HWEnum_BookPic pic, int nTime = 100)
    {
        m_strBook = szGuid;
        m_enBookPic = pic;
        m_strUrl << L"http://d8test.hanwenbook.com:8080/?reqid=3106&reqver=1&guid=" << m_strBook << L"&picno=" << m_enBookPic;
        m_nCacheTime = nTime;
    }

protected:
    friend class HWDocBookData;
    HWParaBookPic(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("url", m_strUrl.toUTF8());
        m_Para.Add("cachetime", m_nCacheTime);
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("picno", m_enBookPic);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("url") || !para->Exist("cachetime") ||
            !para->Exist("guid") || !para->Exist("picno"))
            return false;

        m_strUrl = para->StrVal("url");
        m_nCacheTime = para->IntVal("cachetime");
        m_strBook = para->StrVal("guid");
        m_enBookPic = (HWEnum_BookPic)para->IntVal("picno");
        return true;
    }

public:
    juce::String m_strUrl;
    juce::String m_strBook;
    HWEnum_BookPic m_enBookPic;
    int m_nCacheTime;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookPic)
};

class HWDocBookData : 
    public HWDocBase 
{
public:
    juce_DeclareSingleton(HWDocBookData, false)
    HWDocBookData();
    virtual ~HWDocBookData();

    const HWTBookInfo* GetBookInfo();
    juce::Image* GetBookImage();
    juce::String* GetBookBrief();
    juce::String* GetTextSrc();
    HWTTextSearchData* GetTextSearchData();
    bool GetTextSearchData(int nPageNo, std::vector<int>& vecOffsets);
    HWTBookScore* GetBookScore();
    void SetSearchPageNo(int nPageNo);
protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

    void OnReqFinish(HWEnum_ReqID id, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWParas* paras, HWRequestor* pRequestor, HWContext ctx);

private:
    HWTBookInfo* m_pBookInfo;
    juce::Image* m_pBitmap;

    // BookRes
    HWTCatalog* m_pCatalog;
    HWTReadBase* m_pReadBase;
    juce::String* m_pstrBrief;

    // text get
    juce::String* m_pstrTextGet;
    HWTTextSearchData* m_pTextSearchData;
    HWTBookScore* m_pBookScore;
    
    // search param
    int m_nSearchPage;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBookData)
};