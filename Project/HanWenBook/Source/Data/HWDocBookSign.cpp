#include "JuceHeader.h"
#include "HWDocBookSign.h"
#include "HWLog.h"


juce_ImplementSingleton(HWDocBookSign)

    HWDocBookSign::HWDocBookSign()
{

}

HWDocBookSign::~HWDocBookSign()
{
}

void HWDocBookSign::OnInit()
{
    m_pSignList = NULL;
}

void HWDocBookSign::OnFinal()
{
    SAFE_DELETE_VECTOR_PTR(m_pSignList)
}

void* HWDocBookSign::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;
    if (id == HWReq_Sign_List)
    {
        std::vector<HWTBookSign*>* pvecSignList = new std::vector<HWTBookSign*>;
        HWArray* pArrSigns = HWDictElemArray(dict, "signs");
        for (int i=HWArraySize(pArrSigns) - 1; i>=0; --i)
        {
            HWTBookSign* pBookSign = new HWTBookSign;
            pBookSign->FromHWDict(HWArrayElemDict(pArrSigns, i));
            pvecSignList->push_back(pBookSign);
        }
        result = pvecSignList;
    }
    else if (id == HWReq_Sign_Add)
    {
        HWTBookSign* pBookSign = new HWTBookSign;
        pBookSign->FromHWDict(HWDictElemDict(dict, "booksign"));
        return pBookSign;
    }
    else if (id == HWReq_Sign_Del)
    {

    }

    return result;
}

void HWDocBookSign::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    if (id == HWReq_Sign_List)
    {
        SAFE_DELETE_VECTOR_PTR(m_pSignList);
        if (data)
        {
            m_pSignList = (std::vector<HWTBookSign*>*)data;
            HWNOTIFY(OnBookSignListChanged());
        }
    }
    else if (id == HWReq_Sign_Add)
    {
        if (data)
        {
            HWTBookSign* pBookSign = (HWTBookSign*)data;
            if (!m_pSignList)
                m_pSignList = new std::vector<HWTBookSign*>;
            m_pSignList->insert(m_pSignList->begin(), pBookSign);
            HWNOTIFY(OnBookSignAdd());
        }
    }
    else if (id == HWReq_Sign_Del)
    {
        if (m_pSignList)
        {
            int nID = paras->IntVal("id");
            for (unsigned int i=0; i<m_pSignList->size(); ++i)
            {
                if ((*m_pSignList)[i]->nId == nID)
                {
                    delete (*m_pSignList)[i];
                    m_pSignList->erase(m_pSignList->begin()+i);
                    break;
                }
            }
            HWNOTIFY(OnBookSignDel());
        }
    }
}

bool HWDocBookSign::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocBookSign::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Sign_List:
        {
            HWParaSignList* paraUI = new HWParaSignList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Sign_Add:
        {
            HWParaSignAdd* paraUI = new HWParaSignAdd();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Sign_Del:
        {
            HWParaSignDel* paraUI = new HWParaSignDel();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

void HWDocBookSign::GetPageSignList(int nPage, std::vector<HWTBookSign*>& vecSigns)
{
    if (m_pSignList)
    {
        FOR_EACH_VECTOR(i, (*m_pSignList))
        {
            if ((*m_pSignList)[i]->nPageNo == nPage)
            {
                vecSigns.push_back((*m_pSignList)[i]);
            }
        }
    }
}

int HWDocBookSign::GetBookSignCount()
{
    return m_pSignList ? m_pSignList->size() : 0;
}

const HWTBookSign* HWDocBookSign::GetBookSign( int nIdx )
{
    return (m_pSignList && nIdx >=0 && nIdx < m_pSignList->size()) ? (*m_pSignList)[nIdx] : NULL;
}

bool HWDocBookSign::LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    switch (reqID)
    {
    case HWReq_Sign_Del:
        {
            HWParaSignDel* pSignDelPara = (HWParaSignDel*)para;
            if (m_pSignList)
            {
                for (unsigned int i=0; i<m_pSignList->size(); ++i)
                {
                    if ((*m_pSignList)[i]->nId == pSignDelPara->m_nID)
                    {
                        delete (*m_pSignList)[i];
                        m_pSignList->erase(m_pSignList->begin()+i);
                        return true;
                    }
                }
            }
            break;
        }
    default:
        break;
    }
    return true;
}


