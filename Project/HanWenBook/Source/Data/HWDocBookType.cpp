#include "JuceHeader.h"
#include "HWDocBookType.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocBookType)

HWDocBookType::HWDocBookType()
{
}

HWDocBookType::~HWDocBookType()
{
}

void HWDocBookType::OnInit()
{
    m_jvTypeTree = NULL;
}

void HWDocBookType::OnFinal()
{
    SAFE_DELETE(m_jvTypeTree);
}

void* HWDocBookType::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;
    if (id == HWReq_Type_Tree)
    {
        HWDict::const_iterator iter = dict->find("tree");
        if (iter != dict->end() )
        {
            HWString* pStr = static_cast<HWString*>(iter->second);
            Json::Reader jr;
            Json::Value* Jv = new Json::Value();

            const char* szVal = (const char*)pStr->Value();
            if(!(pStr && jr.parse(szVal, szVal+strlen(szVal), *Jv)) || !Jv->isArray())
            {
                CXLOG(lld_error, "PreDataProcess(%d) json parse error, return null data", id);
                delete Jv;
                return NULL;
            }

            result = Jv;
        }
    }

    return result;
}

void HWDocBookType::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    if(data)
    {
        if (id == HWReq_Type_Tree)
        {
            SAFE_DELETE(m_jvTypeTree);
            m_jvTypeTree = (Json::Value*)data;
        }
    }
}

Json::Value* HWDocBookType::FindTypeIterator(std::vector<juce::String>& vecType, int id, Json::Value* jv)
{
    if (!jv->empty() && !(*jv)[0u].isInt())
    {
        return FindTypeInArray(vecType, id, jv);
    }

    return FindTypeInType(vecType, id, jv);
}

Json::Value* HWDocBookType::FindTypeInArray(std::vector<juce::String>& vecType, int id, Json::Value* jv)
{
    Json::Value* jvFind;
    for (unsigned int i=0; i<jv->size(); ++i)
    {
        jvFind = FindTypeIterator(vecType, id, &(*jv)[i]);
        if (jvFind)
        {
            return jvFind;
        }
    }

    vecType.pop_back();
    return NULL;
}

Json::Value* HWDocBookType::FindTypeInType(std::vector<juce::String>& vecType, int id, Json::Value* jv)
{
    vecType.push_back(juce::CharPointer_UTF8((*jv)[1].asCString()));
    if ((*jv)[0u].asInt() == id)
    {
        return jv;
    }

    return FindTypeInArray(vecType, id, &(*jv)[3]);
}

bool HWDocBookType::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    if(reqID == HWReq_Type_Tree)
    {
        if(m_jvTypeTree != NULL)
        {            
            return true;
        }
    }

    return false;
}

HWParaUI* HWDocBookType::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Type_Books:
        {
            HWParaTypeBookList* paraUI = new HWParaTypeBookList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;
    case HWReq_Type_Tree:
        {
            HWParaTypeTree* paraUI = new HWParaTypeTree();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;
    default:
        return NULL;
    }
}

juce::String HWDocBookType::GetBookCategory(int nTypeID)
{
    if (m_jvTypeTree && m_jvTypeTree->isArray())
    {
        std::vector<juce::String> vecType;
        if (FindTypeIterator(vecType, nTypeID, m_jvTypeTree))
        {
            juce::String strType;
            for (unsigned int i=0; i<vecType.size(); ++i)
            {
                strType += vecType[i];
                if (vecType.size() - 1 != i)
                    strType += L" > ";
            }
            return strType;
        }
    }
    
    return L"";
}
