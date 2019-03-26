#ifndef __DATA_TYPEDEFINE__
#define __DATA_TYPEDEFINE__
#include "HWDocBase.h"
#include "json.h"

class HWParaTypeTree : public HWParaUI
{
public:
    HWParaTypeTree(){}
protected:
    friend class HWDocBookType;

    const HWParas* ToHWPara()
    {
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        return true;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTypeTree)
};

class HWParaTypeBookList : public HWParaUI
{
public:
    HWParaTypeBookList(int nTypeID, int nBegin, int nCnt, HWEnum_BookSort sort)
    {
        m_nTypeID = nTypeID;
        m_nBegin = nBegin;
        m_nCnt = nCnt;
        m_sort = sort;
    }

protected:
    friend class HWDocBookType;
    HWParaTypeBookList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("typeid", m_nTypeID);
        m_Para.Add("begin", m_nBegin);
        m_Para.Add("count", m_nCnt);
        m_Para.Add("sort", (int)m_sort);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("typeid") || !para->Exist("begin") || !para->Exist("count") || !para->Exist("sort") || !para->Exist("filter"))
            return false;

        m_nTypeID = para->IntVal("typeid");
        m_nBegin = para->IntVal("begin");
        m_nCnt = para->IntVal("count");
        m_sort = (HWEnum_BookSort)para->IntVal("sort");
        return true;
    }

private:
    int m_nTypeID;
    int m_nBegin; 
    int m_nCnt;
    HWEnum_BookSort m_sort;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTypeBookList)
};

class HWParaSubjectList : public HWParaUI
{
public:
    HWParaSubjectList(int nGradeNo)
    {
        m_nGradeNo = nGradeNo;
    }

protected:
    friend class HWDocBookType;
    HWParaSubjectList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("gradeno", m_nGradeNo);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("gradeno"))
            return false;

        m_nGradeNo = para->IntVal("gradeno");
        return true;
    }

public:
    int m_nGradeNo;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaSubjectList)
};

class HWParaSubjectBookList : public HWParaUI
{
public:
    HWParaSubjectBookList(int nID)
    {
        m_nID = nID;
    }

protected:
    friend class HWDocBookType;
    HWParaSubjectBookList(){}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaSubjectBookList)
};


class HWDocBookType : 
    public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocBookType, false)
    HWDocBookType();
    virtual ~HWDocBookType();

    // 数据方法定义
    juce::String GetBookCategory(int nTypeID);

private:
    Json::Value* FindTypeIterator(std::vector<juce::String>& vecType, int id, Json::Value* jv);
    Json::Value* FindTypeInArray(std::vector<juce::String>& vecType, int id, Json::Value* jv);
    Json::Value* FindTypeInType(std::vector<juce::String>& vecType, int id, Json::Value* jv);

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx );
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

protected:
    Json::Value* m_jvTypeTree; // 类别树

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBookType)
};

#endif // __DATA_TYPEDEFINE__