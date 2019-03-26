#ifndef __DATA_READ__
#define __DATA_READ__
#include "HWDocBase.h"
#include "KnlRender.h"
#include "json.h"

// 打开图书
class HWParaOpenBook : public HWParaUI
{
public:
    HWParaOpenBook(const juce::String& szGuid)
        : m_strGuid(szGuid)
    {
    }

protected:
    friend class HWDocRead;
    HWParaOpenBook(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strGuid.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("guid"))
            return false;

        m_strGuid = para->StrVal("guid");
        return true;
    }

private:
    juce::String m_strGuid;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaOpenBook)
};

// 阅读一页
class HWParaReadOnePage : public HWParaUI
{
public:
    HWParaReadOnePage(const juce::String& szGuid, int nPageNo)
        : m_strGuid(szGuid)
        , m_nPageNo(nPageNo)
    {
    }

protected:
    friend class HWDocRead;
    HWParaReadOnePage(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strGuid.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("guid") || !para->Exist("pageno"))
            return false;

        m_strGuid = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        return true;
    }

private:
    juce::String m_strGuid;
    int m_nPageNo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaReadOnePage)
};

// 获取用户阅读历史列表    
class HWParaReadHistoryList : public HWParaUI
{
public:
    HWParaReadHistoryList(){}

protected:
    friend class HWDocRead;

    const HWParas* ToHWPara()
    {
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        return true;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaReadHistoryList)
};


// 删除用户历史数据    
class HWParaReadDelHistory : public HWParaUI
{
public:
    HWParaReadDelHistory(const std::vector<juce::String>& vecBooks)
        : m_vecBooks(vecBooks)
    {
    }

    HWParaReadDelHistory()
    {
    }

    void AddBook(const char* szBook)
    {
        m_vecBooks.push_back(szBook);
    }

    void AddBook(const juce::String& szBook)
    {
        m_vecBooks.push_back(szBook);
    }
protected:
    friend class HWDocRead;

    const HWParas* ToHWPara()
    {
        Json::Value* jv = new Json::Value(Json::arrayValue);
        for (unsigned int i=0; i<m_vecBooks.size(); ++i)
        {
            jv->append((const char*)m_vecBooks[i].toUTF8());
        }
        m_Para.Add("guids",jv->toStyledString().c_str());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("guids"))
            return false;
        
        const char* guids = para->StrVal("guids");

        Json::Reader jr;
        Json::Value jv;
        if(!jr.parse(guids, guids+strlen(guids), jv) || !jv.isArray() || jv.empty())
            return false;

        for (unsigned int i=0; i<jv.size(); ++i)
        {
            m_vecBooks.push_back(juce::CharPointer_UTF8(jv[i].asCString()));
        }
        return true;
    }

private:
    std::vector<juce::String> m_vecBooks;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaReadDelHistory)
};

// 取用户图书统计(暂未实现，有UI需求吗？)
class HWParaReadBookStat : public HWParaUI
{
public:
    HWParaReadBookStat(const char* szGuid)
        : m_strGuid(szGuid)
    {
    }

protected:
    friend class HWDocRead;
    HWParaReadBookStat(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strGuid.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if (!para->Exist("guid"))
            return false;

        m_strGuid = para->StrVal("guid");
        return true;
    }

private:
    juce::String m_strGuid;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaReadBookStat)
};

class IHWSubjectRead
{
public:
    virtual void OnReadHistoryDel() {}
    virtual void OnReadOnePage() {}
    virtual void OnBookOpened() {}
};

class HWDocRead : 
    public HWDocBase,
    public HWSubject<IHWSubjectRead>
{
public:
    juce_DeclareSingleton(HWDocRead, false)
    HWDocRead();
    virtual ~HWDocRead();

    const juce::String& GetBook();
    bool IsBookOpened();

    int GetHistoryPageNo();
    int GetHistoryCount();
    std::vector<HWTReadHistoryData*>* GetHistoryList();
    HWTReadHistoryData* GetHistoryData(unsigned int nIdx);

    int GetPDFPageCount();
    int GetPDFWidth();
    int GetPDFHeight();
    float GetPDFAspectRatio();

    juce::String* GetBookBrief();

    bool HasChildCatalog();
    int GetCatalogCount(bool bAll = false);
    const HWTCatalog* GetCatalog(int nIdx, bool bAll, int& nDeep);

    int GetPDFPageNo(const wchar_t* strShowPage);
    bool GetShowPageNo(int nPDFNo, juce::String& strShow);
    bool IsValidBookData();

protected:
    int GetCatalogCount(HWTCatalog* pCatalog);
    const HWTCatalog* GetCatalog(int& nSrc, int& nIdx, int& nDeep, const HWTCatalog* pCatalog);

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);


protected:
    juce::String m_strBook;
    int m_nHistoryPageNo;
    float m_fAspectRatio;
    std::vector<HWTReadHistoryData*>* m_pReadHistoryList;

    // BookRes
    HWTCatalog* m_pCatalog;
    HWTReadBase* m_pReadBase;
    juce::String* m_pstrBrief;
    bool m_bIsBookOpened;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocRead)
};

#endif // __DATA_READ__