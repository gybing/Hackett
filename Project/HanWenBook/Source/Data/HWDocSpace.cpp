#include "JuceHeader.h"
#include "HWDocSpace.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocSpace)
HWDocSpace::HWDocSpace()
{
}

HWDocSpace::~HWDocSpace()
{
}

void HWDocSpace::OnInit()
{
    m_pUserStat = NULL;
    m_pMapBookStat = NULL;
    m_pMapClassStat = NULL;
}

void HWDocSpace::OnFinal()
{
    SAFE_DELETE(m_pUserStat);
    SAFE_DELETE_MAP_PTR(juce::String, HWTBookStat*, m_pMapBookStat);
    SAFE_DELETE_MAP_PTR(juce::String, HWTClassStat*, m_pMapClassStat);
}

void* HWDocSpace::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;
    switch (id)
    {
    case HWReq_Space_UserStatistics:
        {
            HWTUserStat* pUserStat = new HWTUserStat;
            pUserStat->FromHWDict(HWDictElemDict(dict, "userstat"));
            result = pUserStat;
            break;
        }
    case HWReq_Space_BookStatistics:
        {
            std::map<juce::String, HWTBookStat*>* pMapBookStat = new std::map<juce::String, HWTBookStat*>;
            HWDict* pDictBookStat = HWDictElemDict(dict, "books");
            FOR_EACH_MAP(std::string, HWTypeBase*, (*pDictBookStat))
            {
                HWTBookStat* pBookStat = new HWTBookStat;
                pBookStat->FromHWDict((HWDict*)it->second);
                (*pMapBookStat)[it->first.c_str()] = pBookStat;
            }
            result = pMapBookStat;
            break;
        }
    case HWReq_Space_ClassStatistics:
        {
            std::map<juce::String, HWTClassStat*>* pMapClassStat = new std::map<juce::String, HWTClassStat*>;
            HWDict* pDictClassStat = HWDictElemDict(dict, "users");
            FOR_EACH_MAP(std::string, HWTypeBase*, (*pDictClassStat))
            {
                HWTClassStat* pClassStat = new HWTClassStat;
                pClassStat->FromHWDict((HWDict*)it->second);
                (*pMapClassStat)[it->first.c_str()] = pClassStat;
            }
            result = pMapClassStat;
        }
    default:     
        break;
    }
    return result;
}

void HWDocSpace::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Space_UserStatistics:
        {
            SAFE_DELETE(m_pUserStat);
            if (data) m_pUserStat = (HWTUserStat*)data;
            break;
        }
    case HWReq_Space_BookStatistics:
        {
            SAFE_DELETE_MAP_PTR(juce::String, HWTBookStat*, m_pMapBookStat);
            if (data) m_pMapBookStat = (std::map<juce::String, HWTBookStat*>*)data;
            break;
        }
    case HWReq_Space_ClassStatistics:
        {
            SAFE_DELETE_MAP_PTR(juce::String, HWTClassStat*, m_pMapClassStat);
            if (data) m_pMapClassStat = (std::map<juce::String, HWTClassStat*>*)data;
        }
    default:
        break;
    }
}

bool HWDocSpace::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocSpace::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Space_UserStatistics:
        {
            HWParaUserStat* paraUI = new HWParaUserStat();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Space_BookStatistics:
        {
            HWParaBookStat* paraUI = new HWParaBookStat();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Space_ClassStatistics:
        {
            HWParaClassStat* paraUI = new HWParaClassStat();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

HWTUserStat* HWDocSpace::GetUserStat()
{
    return m_pUserStat;
}

HWTClassStat* HWDocSpace::GetClassStat(const juce::String& strUserID )
{
    if (m_pMapClassStat)
    {
        FIND_MAP(juce::String, HWTClassStat*, (*m_pMapClassStat), strUserID)
        {
            return it->second;
        }
    }
    return NULL;
}

std::map<juce::String, HWTClassStat*>* HWDocSpace::GetClassStat()
{
    return m_pMapClassStat;
}

HWTBookStat* HWDocSpace::GetBookStat( const juce::String& strBook )
{
    if (m_pMapBookStat)
    {
        std::map<juce::String, HWTBookStat*>::iterator it = m_pMapBookStat->find(strBook);
        if (it != m_pMapBookStat->end())
        {
            return it->second;
        }
    }
    return NULL;
}

