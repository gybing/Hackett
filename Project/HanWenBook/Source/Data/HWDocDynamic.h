#pragma once

#include "HWDocBase.h"
#include "json.h"

class HWParaBookNoteList : public HWParaUI
{
public:
    HWParaBookNoteList(const juce::String& szBook, HWEnum_DynamicType tp, int nBegin, int nCount, bool bHasFlower)
        : m_strBook(szBook), m_tp(tp), m_nBegin(nBegin), m_nCount(nCount), m_bHasFlower(bHasFlower)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaBookNoteList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("hasflower", m_bHasFlower ? 1:0);
        m_Para.Add("type", (int)m_tp);
        m_Para.Add("begin", m_nBegin);
        m_Para.Add("count", m_nCount);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_bHasFlower = para->IntVal("hasflower") == 1;
        m_tp = (HWEnum_DynamicType)para->IntVal("type");
        m_nBegin = para->IntVal("begin");
        m_nCount = para->IntVal("count");
        return true;
    }

private:
    HWEnum_DynamicType m_tp;
    int m_nBegin;
    int m_nCount;
    juce::String m_strBook;
    bool m_bHasFlower;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaBookNoteList)
};

class HWParaUserNoteList : public HWParaUI
{
public:
    HWParaUserNoteList(const char* szUser, const char* szBook, HWEnum_DynamicType tp, int nBegin, int nCount, bool bHasFlower)
        : m_strUser(szUser), m_strBook(szBook), m_tp(tp), m_nBegin(nBegin), m_nCount(nCount), m_bHasFlower(bHasFlower)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaUserNoteList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("shelfno", m_strUser.toUTF8());
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("hasflower", m_bHasFlower);
        m_Para.Add("type", m_tp);
        m_Para.Add("begin", m_nBegin);
        m_Para.Add("count", m_nCount);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_strUser = para->StrVal("shelfno");
        m_strBook = para->StrVal("guid");
        m_bHasFlower = 1==para->IntVal("hasflower");
        m_tp = (HWEnum_DynamicType)para->IntVal("type");
        m_nBegin = para->IntVal("begin");
        m_nCount = para->IntVal("count");
        return true;
    }

private:
    HWEnum_DynamicType m_tp;
    int m_nBegin;
    int m_nCount;
    juce::String m_strBook;
    juce::String m_strUser;
    bool m_bHasFlower;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUserNoteList)
};

class HWParaDynamicInfo : public HWParaUI
{
public:
    HWParaDynamicInfo(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicInfo(){}

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

public:
    int m_nID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicInfo)
};

class HWParaDynamicReplyInfo : public HWParaUI
{
public:
    HWParaDynamicReplyInfo(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicReplyInfo(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicReplyInfo)
};

class HWParaDynamicReplyList : public HWParaUI
{
public:
    HWParaDynamicReplyList(int nID, int nBegin, int nCount)
        : m_nID(nID), m_nBegin(nBegin), m_nCount(nCount)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicReplyList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("begin", m_nBegin);
        m_Para.Add("count", m_nCount);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        m_nBegin = para->IntVal("begin");
        m_nCount = para->IntVal("count");
        return true;
    }

private:
    int m_nID;
    int m_nBegin;
    int m_nCount;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicReplyList)
};

class HWParaDynamicReply : public HWParaUI
{
public:
    HWParaDynamicReply(int nID, const wchar_t* strText)
        : m_nID(nID), m_strText(strText)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicReply(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("text", m_strText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        m_strText = para->StrVal("text");
        return true;
    }

private:
    int m_nID;
    juce::String m_strText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicReply)
};

class HWParaDynamicReplyReply : public HWParaUI
{
public:
    HWParaDynamicReplyReply(int nID, const wchar_t* strText)
        : m_nID(nID), m_strText(strText)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicReplyReply(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("text", m_strText.toUTF8());
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        m_strText = para->StrVal("text");
        return true;
    }

private:
    int m_nID;
    juce::String m_strText;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicReplyReply)
};

class HWParaDynamicZan : public HWParaUI
{
public:
    HWParaDynamicZan(int nID, bool bZan)
        : m_nID(nID), m_bZan(bZan)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicZan(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("op", m_bZan);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        m_bZan = 1==para->IntVal("op");
        return true;
    }

private:
    int m_nID;
    bool m_bZan;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicZan)
};

class HWParaDynamicFlower : public HWParaUI
{
public:
    HWParaDynamicFlower(int nID, int nOP)
        : m_nID(nID), m_nOP(nOP)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicFlower(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("id", m_nID);
        m_Para.Add("op", m_nOP);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("id");
        m_nOP = para->IntVal("op");
        return true;
    }

private:
    int m_nID;
    int m_nOP;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicFlower)
};

class HWParaDynamicDel : public HWParaUI
{
public:
    HWParaDynamicDel(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocDynamic;
    HWParaDynamicDel(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaDynamicDel)
};

class HWDocDynamic : public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocDynamic, false)
    HWDocDynamic();
    virtual ~HWDocDynamic();

    struct HWDynamicReply
    {
        int nID;
        int nReplyCount;
        int nReviewCount;
        HWTReplyInfo replyInfo;
    };

    struct HWDinamicReplyReply
    {
        int nID;
        int nReplyCount;
        HWTReplyInfo replyInfo;
    };

    int GetBookNoteCount() const;
    void GetBookNoteList(std::vector<std::pair<int,int>>& vec);
    int GetUserNoteCount() const;
    void GetUserNoteList(std::vector<std::pair<int,int>>& vec);

    HWTDynamicInfo* GetDynamicInfo();

    int GetDynamicReplyCount();
    std::vector<int>* GetDynamicReplyList();
    HWTReplyInfo* GetDynamicReplyInfo();

    std::pair<int,int>* GetFavourResult();
    std::pair<int,int>* GetFlowerResult();
protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

private:
    int m_nBookNoteListCount;
    int m_nUserNoteListCount;
    std::vector<std::pair<int,int>>* m_pVecBookNoteList;
    std::vector<std::pair<int,int>>* m_pVecUserNoteList;

    HWTDynamicInfo* m_pDynamicInfo;
    HWTReplyInfo* m_pReplayInfo;
    int m_nDynamicReplyCount;
    std::vector<int>* m_pVecDynamicReplyList;
    HWDynamicReply* m_pDynamicReply;
    HWDinamicReplyReply* m_pDynamicReplyReply;

    std::pair<int,int>* m_pFavourResult;
    std::pair<int,int>* m_pFlowerResult;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocDynamic)
};