#include "JuceHeader.h"
#include "HWDocBase.h"
#include "HWDocManager.h"

std::set<HWRequestor*> HWRequestor::m_setObjs;

HWDocBase::HWDocBase()
{
    
}

HWDocBase::~HWDocBase()
{

}

bool HWDocBase::SubmitRequest( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx, bool bPrior/* = false*/, bool bResp/* = true */)
{
    if(!LocalSet(pRequestor, reqID, para, ctx))
    {
        return false;
    }

    if(LocalGet(pRequestor, reqID, para, ctx))
    {
        // 本地存在数据
        // 如果这样处理导致UI组件内部逻辑出现问题，则改为异步事件模式。
        HWRequestor::OnResponse(pRequestor, reqID, HWError_SUCCEED, HWBusi_UNKNOWN, ctx, para);    
        return true;
    }
    else
    {
        // 发送后台请求
        const HWParas* hwpara = NULL;
        if(para != NULL)
        {
            hwpara = para->ToHWPara();
        }

        return HWDocManager::getInstance()->SubmitRequest(this, reqID, hwpara, pRequestor, (unsigned int)ctx, bPrior, bResp);
    }
}

void HWDocBase::OnReqFinish(HWEnum_ReqID id, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWParas* paras, HWRequestor* pRequestor, HWContext ctx)
{
    // 通知UI
    HWParaUI* para = MakeHWParaUI(id, paras);
    HWDocHelper::getInstance()->CheckResponseError(id, err, busi);
    HWRequestor::OnResponse(pRequestor, id, err, busi, ctx, para);
    SAFE_DELETE(para);
}