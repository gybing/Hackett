#include "JuceHeader.h"
#include "HWDocBookMark.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocBookMark)

HWDocBookMark::HWDocBookMark()
{
    
}

HWDocBookMark::~HWDocBookMark()
{
}

void HWDocBookMark::OnInit()
{
    m_pVecBookMarks = NULL;
}

void HWDocBookMark::OnFinal()
{
    SAFE_DELETE_VECTOR_PTR(m_pVecBookMarks);
}

void* HWDocBookMark::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_Mark_List:
        {
            std::vector<HWTBookMark*>* pBookMarks = new std::vector<HWTBookMark*>;
            HWArray* pArrMarks = HWDictElemArray(dict, "bookmarks");
            for (int i=pArrMarks->size()-1; i>=0; --i)
            {
                HWTBookMark* pMark = new HWTBookMark;
                pMark->FromHWDict(HWArrayElemDict(pArrMarks, i));
                pBookMarks->push_back(pMark);
            }
            result = pBookMarks;
            break;
        }
    case HWReq_Mark_Add:
        {
            HWTBookMark* pMark = new HWTBookMark;
            pMark->FromHWDict(HWDictElemDict(dict, "bookmark"));
            result = pMark;
            break;
        }
    case HWReq_Mark_Del:
        {
            break;
        }
    default:break;
    }
    return result;
}

void HWDocBookMark::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Mark_List:
        {
            SAFE_DELETE_VECTOR_PTR(m_pVecBookMarks);
            if (data) m_pVecBookMarks = (std::vector<HWTBookMark*>*)data;
        }
        break;
    case HWReq_Mark_Add:
        {
            if (data)
            {
                if (!m_pVecBookMarks)
                    m_pVecBookMarks = new std::vector<HWTBookMark*>;
                m_pVecBookMarks->insert(m_pVecBookMarks->begin(), (HWTBookMark*)data);
                HWNOTIFY(OnBookMarkAdd());
            }
        }
        break;
    case HWReq_Mark_Del:
        {
            HWNOTIFY(OnBookMarkDel());
        }
        break;
    default:
        break;
    }
}

bool HWDocBookMark::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    switch (reqID)
    {
    case HWReq_Mark_List:
    case HWReq_Mark_Add:
    case HWReq_Mark_Del:
    default:
        break;
    }
    return false;
}

HWParaUI* HWDocBookMark::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Mark_List:
        {
            HWParaBookMarkList* paraUI = new HWParaBookMarkList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Mark_Add:
        {
            HWParaBookMarkAdd* paraUI = new HWParaBookMarkAdd();
            paraUI->FromHWPara(para);
            return paraUI;

        }
    case HWReq_Mark_Del:
        {
            HWParaBookMarkDel* paraUI = new HWParaBookMarkDel();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

bool HWDocBookMark::HasBookMark( int nPageNo )
{
    if (m_pVecBookMarks)
    {
        FOR_EACH_VECTOR(i, (*m_pVecBookMarks))
        {
            if ((*m_pVecBookMarks)[i]->nPageNo == nPageNo)
                return true;
        }
    }
    return false;
}

int HWDocBookMark::GetBookMarkCount()
{
    return m_pVecBookMarks ? m_pVecBookMarks->size() : 0;
}

const HWTBookMark* HWDocBookMark::GetBookMark( int nIdx )
{
    return (m_pVecBookMarks && nIdx >= 0 && nIdx < m_pVecBookMarks->size()) ? (*m_pVecBookMarks)[nIdx] : NULL;
}

bool HWDocBookMark::LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    if (reqID == HWReq_Mark_Del)
    {
        if (m_pVecBookMarks)
        {
            HWParaBookMarkDel* pDel = (HWParaBookMarkDel*)para;
            FOR_EACH_VECTOR(i, (*m_pVecBookMarks))
            {
                if ((*m_pVecBookMarks)[i]->nPageNo == pDel->m_nPageNo)
                {
                    delete (*m_pVecBookMarks)[i];
                    m_pVecBookMarks->erase(m_pVecBookMarks->begin()+i);
                    return true;
                }
            }
        }
    }
    return true;
}

