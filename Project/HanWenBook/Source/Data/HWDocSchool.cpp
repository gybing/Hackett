#include "JuceHeader.h"
#include "HWDocSchool.h"

juce_ImplementSingleton(HWDocSchool)


HWDocSchool::HWDocSchool( void )
{
}

HWDocSchool::~HWDocSchool( void )
{
}

void HWDocSchool::OnInit()
{
    m_pClassInfo = NULL;
    m_pStudentList = NULL;
    m_pTeacherList = NULL;
}

void HWDocSchool::OnFinal()
{
    SAFE_DELETE(m_pClassInfo);
    SAFE_DELETE_VECTOR_PTR(m_pStudentList);
    SAFE_DELETE_VECTOR_PTR(m_pTeacherList);
}

void* HWDocSchool::OnRespPreProcess( HWEnum_ReqID id, const HWParas* paras, const HWDict* dict )
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_School_ClassInfo:
        {
            HWTClassInfo* pInfo = new HWTClassInfo;
            pInfo->FromHWDict(HWDictElemDict(dict, "classinfo"));
            result = pInfo;
            break;
        }
    case HWReq_School_StudentList:
        {
            HWSchoolUserList* pStudentList = new HWSchoolUserList;
            HWSchoolUserInfo* pStudent;

            HWArray* pArr = HWDictElemArray(dict, "users");
            for (unsigned int i=0; i<HWArraySize(pArr); ++i)
            {
                pStudent = new HWSchoolUserInfo;
                pStudent->FromHWDict(HWArrayElemDict(pArr, i));
                pStudentList->push_back(pStudent);
            }

            result = pStudentList;
        }
        break;

    case HWReq_School_TeacherList:
        {
            HWSchoolUserList* pTeacherList = new HWSchoolUserList;
            HWSchoolUserInfo* pTeacher;

            HWArray* pArr = HWDictElemArray(dict, "users");
            for (unsigned int i=0; i<HWArraySize(pArr); ++i)
            {
                pTeacher = new HWSchoolUserInfo;
                pTeacher->FromHWDict(HWArrayElemDict(pArr, i));
                pTeacherList->push_back(pTeacher);
            }

            result = pTeacherList;
        }
        break;

    default:
        break;
    }

    return result;
}

void HWDocSchool::OnRespProcess( HWEnum_ReqID id, const HWParas* paras, void* data )
{
    switch (id)
    {
    case HWReq_School_ClassInfo:
        {
            SAFE_DELETE(m_pClassInfo);
            if (data) m_pClassInfo = (HWTClassInfo*)data;
            break;
        }
    case HWReq_School_StudentList:
        {
            SAFE_DELETE_VECTOR_PTR(m_pStudentList);
            if (data) m_pStudentList = (HWSchoolUserList*)data;
        }
        break;
    case HWReq_School_TeacherList:
        {
            SAFE_DELETE_VECTOR_PTR(m_pTeacherList);
            if (data) m_pTeacherList = (HWSchoolUserList*)data;
        }
        break;
    default:
        break;
    }
}

HWParaUI* HWDocSchool::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_School_StudentList:
        {
            HWParaStudentList* paraUI = new HWParaStudentList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_School_TeacherList:
        {
            HWParaTeacherList* paraUI = new HWParaTeacherList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_School_ClassInfo:
        {
            HWParaClassInfo* paraUI = new HWParaClassInfo();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

const HWDocSchool::HWSchoolUserList* HWDocSchool::GetStudentList() const
{
    return m_pStudentList;
}

const HWTClassInfo* HWDocSchool::GetClassInfo() const
{
    return m_pClassInfo;
}

const HWDocSchool::HWSchoolUserList* HWDocSchool::GetTeacherList() const
{
    return m_pTeacherList;
}
