#include "JuceHeader.h"
#include "HWDocDraw.h"
#include "KnlBlock.h"

juce_ImplementSingleton(HWDocDraw)

HWDocDraw::HWDocDraw()
{
}

HWDocDraw::~HWDocDraw()
{
}

void HWDocDraw::OnInit()
{
    m_pPageObject = nullptr;
}

void HWDocDraw::OnFinal()
{
    m_pPageObject = nullptr;
}

void* HWDocDraw::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;

    if (id == HWReq_Draw_PageDraw)
    {
        HWPageObject* pageObj = HWDictElemObject(dict, "pageobj");
        if (pageObj)
        {
            result = const_cast<void*>(pageObj->Value());
        }
    }
    return result;
}

void HWDocDraw::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    if (id == HWReq_Draw_PageDraw)
    {
        if (PageObject* obj = static_cast<PageObject*>(data))
        {
            m_pPageObject = new HWPageObjectUI(obj);
        }
    }
}


HWParaUI* HWDocDraw::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Draw_PageDraw:
        {
            HWParaPageDraw* paraUI = new HWParaPageDraw();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;

    default:
        return NULL;
    }
}

HWPageObjectUI::Ptr HWDocDraw::GetPageObject()
{
    return m_pPageObject;
}

//////////////////////////////////////////////////////////////////////////
// HWPageObjectUI
HWPageObjectUI::HWPageObjectUI(PageObject* po)
    : object(po)
{
}

HWPageObjectUI::~HWPageObjectUI()
{
    if (object)
    {
        const PagePixmap* pPix = object->GetPagePixmap();
        if (pPix) KnlDropPagePixmap(pPix);
        HWBaseInstance()->HWDelete((void*)object);
        object = nullptr;
    }
}

PageObject* HWPageObjectUI::getPageObject()
{
    return object;
}