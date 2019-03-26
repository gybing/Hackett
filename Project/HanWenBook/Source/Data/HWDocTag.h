#pragma once
#include "HWDocBase.h"
#include "HWDefine.h"

class HWParaTagList : public HWParaUI
{
public:
    HWParaTagList(const juce::String& szUserID)
        : m_strShelfNo(szUserID)
    {
    }

protected:
    friend class HWDocTag;
    HWParaTagList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("shelfno", m_strShelfNo.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strShelfNo = para->StrVal("shelfno");
        return true;
    }

public:
    juce::String m_strShelfNo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTagList)
};

class HWParaBookList : public HWParaUI
{
public:
    HWParaBookList(const juce::String& szUserID)
    {
        m_strShelfNo = szUserID;
    }

protected:
    friend class HWDocTag;
    HWParaBookList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("shelfno", m_strShelfNo.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("shelfno"))
            return false;

        m_strShelfNo = para->StrVal("shelfno");
        return true;
    }

public:
    juce::String m_strShelfNo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookList)
};

class HWParaAddTag : public HWParaUI
{
public:
    HWParaAddTag(const juce::String& szName, bool bFirst)
    {
        m_strName = szName;
        m_bFirst = bFirst;
    }

protected:
    friend class HWDocTag;
    HWParaAddTag(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("name", m_strName.toUTF8());
        m_Para.Add("bfirst", m_bFirst ? 1 : 0);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("name") || !para->Exist("bfirst"))
            return false;

        m_strName = para->StrVal("name");
        m_bFirst = para->IntVal("bfirst") == 1 ? true:false;
        return true;
    }

public:
    juce::String m_strName;
    bool m_bFirst;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaAddTag)
};

class HWParaDelTag : public HWParaUI
{
public:
    HWParaDelTag(int nID)
    {
        m_nID = nID;
    }

protected:
    friend class HWDocTag;
    HWParaDelTag(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("id"))
            return false;

        m_nID = para->IntVal("id");
        return true;
    }

public:
    int m_nID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDelTag)
};

class HWParaTagName : public HWParaUI
{
public:
    HWParaTagName(int nID, const juce::String& szName)
    {
        m_nID = nID;
        m_strName = szName;
    }

protected:
    friend class HWDocTag;
    HWParaTagName(){}
    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("name", m_strName.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("id") || !para->Exist("name"))
            return false;

        m_nID = para->IntVal("id");
        m_strName = para->StrVal("name");
        return true;
    }

public:
    int m_nID;
    juce::String m_strName;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTagName)
};

class HWParaTagOrder : public HWParaUI
{
public:
    HWParaTagOrder(const std::vector<int>& vecIDs)
    {
        for (unsigned int u = 0; u < vecIDs.size(); ++u)
        {
            m_arrIDs.push_back(vecIDs[u]);
        }
    }

protected:
    friend class HWDocTag;
    HWParaTagOrder(){}
    const HWParas* ToHWPara()
    {
        Json::FastWriter jw;
        Json::Value jv(Json::arrayValue), jvItem(Json::intValue);
        for (unsigned int i = 0; i < m_arrIDs.size(); i++)
        {
            jv.append(Json::Value(m_arrIDs[i]));
        }
        m_Para.Add("ids", jw.write(jv).c_str());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("ids"))
            return false;

        Json::Reader jr;
        Json::Value jv;

        std::string strJson = para->StrVal("ids");
        if(!jr.parse(strJson, jv) || !jv.isArray())
            return false;

        for (unsigned int i = 0; i < jv.size(); i++)
        {
            m_arrIDs.push_back(jv[i].asInt());
        }
        return true;
    }

public:
    std::vector<int> m_arrIDs;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTagOrder)
};

class HWParaBookOrder : public HWParaUI
{
public:
    HWParaBookOrder(const std::vector<int>& vecIDs)
    {
        m_vecIDs = vecIDs;
    }

    HWParaBookOrder(int nSwapIdx1, int nSwapIdx2);

protected:
    friend class HWDocTag;
    HWParaBookOrder(){}
    const HWParas* ToHWPara()
    {
        Json::FastWriter jw;
        Json::Value jv(Json::arrayValue), jvItem(Json::intValue);
        for (unsigned int i = 0; i < m_vecIDs.size(); i++)
        {
            jv.append(Json::Value(m_vecIDs[i]));
        }
        m_Para.Add("ids", jw.write(jv).c_str());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("ids"))
            return false;

        Json::Reader jr;
        Json::Value jv;

        std::string strJson = para->StrVal("ids");
        if(!jr.parse(strJson, jv) || !jv.isArray())
            return false;

        for (unsigned int i = 0; i < jv.size(); i++)
        {
            m_vecIDs.push_back(jv[i].asInt());
        }
        return true;
    }

public:
    std::vector<int> m_vecIDs;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookOrder)
};

class HWParaBookDel : public HWParaUI
{
public:
    HWParaBookDel(const std::vector<int>& vecIDs)
    {
        m_vecIDs = vecIDs;
    }

    HWParaBookDel(int nBookID)
    {
        m_vecIDs.push_back(nBookID);
    }

protected:
    friend class HWDocTag;
    HWParaBookDel(){}
    const HWParas* ToHWPara()
    {
        Json::FastWriter jw;
        Json::Value jv(Json::arrayValue), jvItem(Json::intValue);
        for (unsigned int i = 0; i < m_vecIDs.size(); i++)
        {
            jv.append(Json::Value(m_vecIDs[i]));
        }
        m_Para.Add("ids", jw.write(jv).c_str());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("ids"))
            return false;

        Json::Reader jr;
        Json::Value jv;

        std::string strJson = para->StrVal("ids");
        if(!jr.parse(strJson, jv) || !jv.isArray())
            return false;

        for (unsigned int i = 0; i < jv.size(); i++)
        {
            m_vecIDs.push_back(jv[i].asInt());
        }
        return true;
    }

public:
    std::vector<int> m_vecIDs;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookDel)
};

class HWParaBookCollect : public HWParaUI
{
public:
    HWParaBookCollect(const char* szGuid)
        : m_strBook(szGuid)
    {
    }

protected:
    friend class HWDocTag;
    HWParaBookCollect(){}
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookCollect)
};

class HWParaBookMakeTag : public HWParaUI
{
public:
    HWParaBookMakeTag(int nID, int nTagID)
    {
        m_nID = nID;
        m_nTagID = nTagID;
    }

protected:
    friend class HWDocTag;
    HWParaBookMakeTag(){}
    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("tagid", m_nTagID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("id") || !para->Exist("tagid"))
            return false;

        m_nID = para->IntVal("id");
        m_nTagID = para->IntVal("tagid");
        return true;
    }

public:
    int m_nID;
    int m_nTagID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookMakeTag)
};

class IHWSubjectTag
{
public:
    virtual void OnBookPosChange() {}
    virtual void OnBookDelete(const std::vector<int> vecIds) {}

    virtual void OnBookListChange() {}
    virtual void OnBookTagListChange() {}

private:
    JUCE_LEAK_DETECTOR (IHWSubjectTag)
};

class HWDocTag :
    public HWDocBase,
    public HWSubject<IHWSubjectTag>
{
public:
    juce_DeclareSingleton(HWDocTag, false)
    HWDocTag(void);
    ~HWDocTag(void);


    typedef std::vector<HWTUserTag*> HWTagList; // 用户标签列表
    typedef std::vector<HWTUserBook*> HWBookList; // 用户图书签列表

    HWTUserTag* GetUserTagByID(int nTagID);
    HWTagList* GetUserTagList();
    HWTUserTag* GetUserTag(int nIdx);
    int GetUserTagCount();
    void GetUserBookListFromTag(int nTag, HWDocTag::HWBookList& arrBookList);
    void GetUserBookListFromTag(int nBegin, int nCount, int nTag, HWDocTag::HWBookList& arrBookList);
    HWDocTag::HWBookList* GetBookList();
    int GetTagBookCount(int nTag);
    HWTUserBook* GetUserBook(int nID);
    HWTUserBook* GetUserBook(juce::String hBook);
    int GetUserBookCount();

    bool IsTagNameExist(juce::String& strName);

    // nDestPosInfo 0：dest<movebooks 1:dest>movebooks 2:movebooks<dest<movebooks
    void ChangeBookPos(std::vector<int>& vecBookIds, int nDestBookID, int nDestPosInfo);
    void SubmitChangeBookPos(HWRequestor* requestor);

protected:
    virtual void OnInit();
    virtual void OnFinal();

    void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict);
    void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);
    HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);
    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );

private:
    HWTagList* m_pTagList;
    HWBookList* m_pBookList;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocTag)
};

