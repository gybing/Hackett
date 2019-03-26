#include "JuceHeader.h"
#include "HWDocRecommend.h"

juce_ImplementSingleton(HWDocRecommend)

HWDocRecommend::HWDocRecommend( void )
{
}

HWDocRecommend::~HWDocRecommend( void )
{
}

void HWDocRecommend::OnInit()
{
    m_pCommendData = NULL;
}

void HWDocRecommend::OnFinal()
{
    SAFE_DELETE(m_pCommendData);
}

void* HWDocRecommend::OnRespPreProcess( HWEnum_ReqID id, const HWParas* paras, const HWDict* dict )
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_Commend_Class:
        {
            HWTCommendData* pCommendData = new HWTCommendData;
            pCommendData->FromHWDict(HWDictElemDict(dict, "commend"));
            result = pCommendData;
            break;
        }
    case HWReq_Commend_Book:
        {
            break;
        }
    default:
        break;
    }

    return result;
}

void HWDocRecommend::OnRespProcess( HWEnum_ReqID id, const HWParas* paras, void* data )
{
    switch (id)
    {
    case HWReq_Commend_Class:
        {
            SAFE_DELETE(m_pCommendData);
            if (data) m_pCommendData = (HWTCommendData*)data;
        }
    case HWReq_Commend_Book:
        {
            break;
        }
    default:
        break;
    }
}

HWParaUI* HWDocRecommend::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Commend_Class:
        {
            HWParaClassCommend* paraUI = new HWParaClassCommend();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Commend_Book:
        {
            HWParaBookCommend* paraUI = new HWParaBookCommend();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

HWTCommendData* HWDocRecommend::GetCommendData()
{
    return m_pCommendData;
}

