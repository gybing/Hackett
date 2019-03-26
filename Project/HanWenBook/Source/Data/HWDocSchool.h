#pragma once
#include "HWDocBase.h"
#include "HWDefine.h"

class HWParaClassInfo : public HWParaUI
{
public:
    HWParaClassInfo(int nID)
        : m_nID(nID)
    {
    }

protected:
    friend class HWDocSchool;
    HWParaClassInfo(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("classid", m_nID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        m_nID = para->IntVal("classid");
        return true;
    }

private:
    int m_nID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaClassInfo)
};

class HWParaStudentList : public HWParaUI
{
public:
    HWParaStudentList(int classid)
    {
        m_nClassID = classid;
    }

protected:
    friend class HWDocSchool;
    HWParaStudentList(){}

    const HWParas* ToHWPara()
    {
        m_Para.Add("classid", m_nClassID);
        return &m_Para;
    }

    bool FromHWPara(const HWParas* para)
    {
        if(!para->Exist("classid"))
            return false;

        m_nClassID = para->IntVal("classid");
        return true;
    }

public:
    int m_nClassID;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaStudentList)
};

class HWParaTeacherList : public HWParaStudentList
{
public:
    HWParaTeacherList(int classid) : HWParaStudentList(classid)
    {
    }

protected:
    friend class HWDocSchool;
    HWParaTeacherList(){}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaTeacherList)
};

class HWDocSchool :
    public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocSchool, false)
    HWDocSchool(void);
    ~HWDocSchool(void);

public:

    // 学生/老师信息
    typedef HWTUserInfo HWSchoolUserInfo;
    typedef  std::vector<HWSchoolUserInfo*> HWSchoolUserList; // 学生/老师列表

public:
    const HWSchoolUserList* GetStudentList() const;
    const HWSchoolUserList* GetTeacherList() const;
    const HWTClassInfo* GetClassInfo() const;
protected:
    virtual void OnInit();
    virtual void OnFinal();

    void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict);
    void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);
    HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

protected:
    HWTClassInfo* m_pClassInfo;
    HWSchoolUserList* m_pStudentList;
    HWSchoolUserList* m_pTeacherList;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocSchool)
};

