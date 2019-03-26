#include "JuceHeader.h"
#include "HWDocComment.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocComment)
HWDocComment::HWDocComment()
{

}

HWDocComment::~HWDocComment()
{

}

void HWDocComment::OnInit()
{
    m_pVecCommentList = NULL;
}

void HWDocComment::OnFinal()
{
    SAFE_DELETE_VECTOR_PTR(m_pVecCommentList);
}

void* HWDocComment::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_Comment_List:
        {
            std::vector<HWTBookComment*>* pVecCommentList = new std::vector<HWTBookComment*>;

            HWArray* pArr = HWDictElemArray(dict, "comments");
            for (int i = 0; i < HWArraySize(pArr); i++)
            {
                HWTBookComment* pComment = new HWTBookComment;
                pComment->FromHWDict(HWArrayElemDict(pArr, i));
                pVecCommentList->push_back(pComment);
            }

            result = pVecCommentList;
        }
        break;
    case HWReq_Comment_Add:
        {
            HWTBookComment* pComment = new HWTBookComment;
            pComment->FromHWDict(HWDictElemDict(dict, "comment"));
            result = pComment;
        }
        break;
    case HWReq_Comment_Del:
        {
        }
        break;
    default:
        break;
    }
    return result;
}

void HWDocComment::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Comment_List:
        {
            SAFE_DELETE_VECTOR_PTR(m_pVecCommentList);
            if (data) m_pVecCommentList = (std::vector<HWTBookComment*>*)data;
        }
        break;
    case HWReq_Comment_Add:
        {
            if (m_pVecCommentList && data)
            {
                m_pVecCommentList->insert(m_pVecCommentList->begin(), (HWTBookComment*)data);
            }
        }
        break;
    case HWReq_Comment_Del:
        {
        }
        break;
    default:
        break;
    }
}

bool HWDocComment::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocComment::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Comment_List:
        {
            HWParaCommentList* paraUI = new HWParaCommentList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;
    case HWReq_Comment_Add:
        {
            HWParaCommentAdd* paraUI = new HWParaCommentAdd();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;
    case HWReq_Comment_Del:
        {
            HWParaCommentDel* paraUI = new HWParaCommentDel();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;
    default:
        return NULL;
    }
}

std::vector<HWTBookComment*>* HWDocComment::GetCommentList()
{
    return m_pVecCommentList;
}

HWTBookComment* HWDocComment::GetComment( int nID )
{
    if (m_pVecCommentList)
    {
        for (unsigned int i=0; i<m_pVecCommentList->size(); ++i)
        {
            if ((*m_pVecCommentList)[i]->nID == nID)
            {
                return (*m_pVecCommentList)[i];
            }
        }
    }
    return NULL;
}

