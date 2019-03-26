#include "JuceHeader.h"
#include "HWDocUserData.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocUserData)
HWDocUserData::HWDocUserData()
{
}

HWDocUserData::~HWDocUserData()
{

}

void HWDocUserData::OnInit()
{
    m_pBitmap = NULL;
    m_pUserInfo = NULL;
}

void HWDocUserData::OnFinal()
{
    SAFE_DELETE(m_pBitmap);
    SAFE_DELETE(m_pUserInfo);
}

void* HWDocUserData::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void * result = NULL;
    switch (id)
    {
    case HWReq_Pic_Download:
        {
            HWImg* pImage = HWDictElemImage(dict, "img");
			if (pImage && pImage->Length() > 0)
			{
                result = new juce::Image(juce::ImageFileFormat::loadFrom (pImage->Value(), pImage->Length()));
			}
            break;
        }
    case HWReq_School_UsrInfo:
        {
            HWTUserInfo* pUserInfo = new HWTUserInfo;
            pUserInfo->FromHWDict(HWDictElemDict(dict,"userinfo"));
            result = pUserInfo;
            break;
        }
    case HWReq_User_ModHead:
        {
            break;
        }
    }
    return result;
}

void HWDocUserData::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Pic_Download:
        {
			SAFE_DELETE(m_pBitmap);
			if (data)
			{
				m_pBitmap = (juce::Image*)data;
			}
            break;
        }
    case HWReq_School_UsrInfo:
        {
            SAFE_DELETE(m_pUserInfo);
            if (data) m_pUserInfo = (HWTUserInfo*)data;
            break;
        }
    case HWReq_User_ModHead:
        {
            break;
        }
    default:break;
    }
}

bool HWDocUserData::LocalGet(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx)
{
    return false;
}

HWParaUI* HWDocUserData::MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para)
{
    switch (reqID)
    {
    case HWReq_Pic_Download:
    {
        HWParaUserHead* paraUI = new HWParaUserHead();
        paraUI->FromHWPara(para);
        return paraUI;
    }
    case HWReq_School_UsrInfo:
        {
            HWParaUserInfo* paraUI = new HWParaUserInfo();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

juce::Image* HWDocUserData::GetUserHead()
{
    return m_pBitmap;
}

HWTUserInfo* HWDocUserData::GetUserInfo()
{
    return m_pUserInfo;
}
