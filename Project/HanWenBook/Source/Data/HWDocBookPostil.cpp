#include "JuceHeader.h"
#include "HWDocBookPostil.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocBookPostil)

HWDocBookPostil::HWDocBookPostil()
{

}

HWDocBookPostil::~HWDocBookPostil()
{
}

void HWDocBookPostil::OnInit()
{
    m_pVecBookPostils = NULL;
    m_pMapPostilPageArea = NULL;
    m_pVecAreaPostils = NULL;
}

void HWDocBookPostil::OnFinal()
{
    SAFE_DELETE(m_pVecAreaPostils);
    SAFE_DELETE(m_pMapPostilPageArea);
    SAFE_DELETE_VECTOR_PTR(m_pVecBookPostils);
}

void* HWDocBookPostil::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void * result = NULL;

    if (id == HWReq_Postil_List)
    {
        std::vector<HWTBookPostil*>* pVecPostils = new std::vector<HWTBookPostil*>;
        HWArray* pArrPostils = HWDictElemArray(dict, "postils");
        for (unsigned int i=0; i<HWArraySize(pArrPostils); ++i)
        {
            HWTBookPostil* pBookPostil = new HWTBookPostil;
            pBookPostil->FromHWDict(HWArrayElemDict(pArrPostils, i));
            pVecPostils->push_back(pBookPostil);
        }
        result = pVecPostils;
    }
    else if (id == HWReq_Postil_Add)
    {
        HWTBookPostil* pBookPostil = new HWTBookPostil;
        pBookPostil->FromHWDict(HWDictElemDict(dict, "postil"));
        result = pBookPostil;
    }
    else if (id == HWReq_Dynamic_PageArea) // 取批注位置统计信息
    {
        std::map<int, std::pair<int,int>>* pMapPostilPageArea = new std::map<int, std::pair<int,int>>;
        HWDict& dictPoss = *HWDictElemDict(dict, "zones");
        HWDict::iterator iter = dictPoss.begin();
        while (iter != dictPoss.end())
        {
            HWDict* dictItem = (HWDict*)(iter->second->Value());
            (*pMapPostilPageArea)[atoi(iter->first.c_str())] = std::pair<int,int>(HWDictElemInt(dictItem, "my"), HWDictElemInt(dictItem, "other"));
            iter++;
        }
        result = pMapPostilPageArea;
    }
    else if (id == HWReq_Dynamic_AreaPostil) // 取区域批注
    {
        std::pair<std::vector<int>, std::vector<int>>* pVecAreaPostils = new std::pair<std::vector<int>, std::vector<int>>;

        HWArray* pArray = HWDictElemArray(dict, "my");
        for (unsigned int i=0; i<HWArraySize(pArray); ++i)
            pVecAreaPostils->first.push_back(HWArrayElemInt(pArray, i));

        pArray = HWDictElemArray(dict, "other");
        for (unsigned int i=0; i<HWArraySize(pArray); ++i)
            pVecAreaPostils->second.push_back(HWArrayElemInt(pArray, i));
        result = pVecAreaPostils;
    }

    return result;
}

void HWDocBookPostil::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    if (id == HWReq_Postil_List)
    {
        SAFE_DELETE_VECTOR_PTR(m_pVecBookPostils);
        if (data) m_pVecBookPostils = (std::vector<HWTBookPostil*>*)data;
    }
    else if (id == HWReq_Postil_Add)
    {
        if (data)
        {
            if (!m_pVecBookPostils)
                m_pVecBookPostils = new std::vector<HWTBookPostil*>;
            m_pVecBookPostils->insert(m_pVecBookPostils->begin(), (HWTBookPostil*)data);
            HWNOTIFY(OnBookPostilAdd())
        }
    }
    else if (id == HWReq_Dynamic_PageArea) // 取批注位置统计信息
    {
        if (m_pMapPostilPageArea) { delete m_pMapPostilPageArea; m_pMapPostilPageArea = NULL; }
        if (data) m_pMapPostilPageArea = (std::map<int, std::pair<int,int>>*)data;
    }
    else if (id == HWReq_Dynamic_AreaPostil) // 取区域批注
    {
        if (m_pVecAreaPostils) { delete m_pVecAreaPostils; m_pVecAreaPostils = NULL; }
        if (data) m_pVecAreaPostils = (std::pair<std::vector<int>, std::vector<int>>*)data;
    }
}

bool HWDocBookPostil::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocBookPostil::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Postil_List:
        {
            HWParaBookPostilList* paraUI = new HWParaBookPostilList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Postil_Add:
        {
            HWParaBookPostilAdd* paraUI = new HWParaBookPostilAdd();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Postil_Del:
        {
            HWParaBookPostilDel* paraUI = new HWParaBookPostilDel();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Dynamic_PageArea: // 取批注位置统计信息
        {
            HWParaPostilPageArea* paraUI = new HWParaPostilPageArea();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Dynamic_AreaPostil: // 取区域批注
        {
            HWParaAreaPostil* paraUI = new HWParaAreaPostil();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

void HWDocBookPostil::GetPagePostils( int nPage, std::vector<HWTBookPostil*>& vecPostils )
{
    if (!m_pVecBookPostils)
        return;

    FOR_EACH_VECTOR(i, (*m_pVecBookPostils))
    {
        if ((*m_pVecBookPostils)[i]->nPageNo == nPage)
            vecPostils.push_back((*m_pVecBookPostils)[i]);
    }
}

std::map<int, std::pair<int,int>>* HWDocBookPostil::GetPostilPageArea()
{
    return m_pMapPostilPageArea;
}

std::vector<HWTBookPostil*>* HWDocBookPostil::GetPostilList()
{
    return m_pVecBookPostils;
}

HWTBookPostil* HWDocBookPostil::GetPostil( int nID )
{
    if (m_pVecBookPostils)
    {
        FOR_EACH_VECTOR(i, (*m_pVecBookPostils))
        {
            if ((*m_pVecBookPostils)[i]->nID == nID)
            {
                return (*m_pVecBookPostils)[i];
            }
        }
    }
    return NULL;
}

std::pair<std::vector<int>, std::vector<int>>* HWDocBookPostil::GetAreaPostilIDs()
{
    return m_pVecAreaPostils;
}


