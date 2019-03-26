#include "JuceHeader.h"
#include "HWDocManager.h"
#include "HWBase.h"
#include "time_util.h"

const char* url_config[4] = {
    "https://i8test.hanwenbook.com/ssl.ashx",
    "http://i8test.hanwenbook.com/i.ashx",
    "http://d8test.hanwenbook.com:8080",
    "push8test.hanwenbook.com:9000"
};

const char* dns_config = "8.8.8.8,8.8.4.4";

std::set<HWUIContext*> m_setContextUI;

// 请求上下文信息封装
struct HWUIContext
{
    HWDocBase* doc;
    HWRequestor* pRequestor;
    int docCtx;
    int tmStart;
};

class CXExecTarget : public juce::AsyncUpdater
{
    virtual void Run() = 0;
    virtual void handleAsyncUpdate() override 
    {
        Run();
        delete this;
    }
};

class CXExecNotifySys : public CXExecTarget
{
public:
    CXExecNotifySys(HWEnum_SysNotifyType sysNotify, HWContext ctx)
        : m_enSysNotify(sysNotify)
        , m_ctx(ctx)
    {}

    virtual void Run()
    {
        // 获取通知Doc
        HWDocNotify::getInstance()->OnSystemNotify(m_enSysNotify, m_ctx);
    }

protected:
    HWEnum_SysNotifyType m_enSysNotify;
    HWContext m_ctx;
};

class CXExecNotifyBusi : public CXExecTarget
{
public:
    CXExecNotifyBusi(HWEnum_BusiNotifyType busiNotify, const HWDict* dict)
        : m_enBusiNotify(busiNotify)
    {
        if (dict) m_dict = *dict;
    }

    virtual void Run()
    {
        // 获取通知Doc
        HWDocNotify::getInstance()->OnBusiNotify(m_enBusiNotify, &m_dict);
    }

protected:
    HWEnum_BusiNotifyType m_enBusiNotify;
    HWDict m_dict;
};

// 用于响应结果传递
class CXResponse : public CXExecTarget
{
public:
    CXResponse(HWEnum_ReqID id, HWUIContext* ctx, const HWParas* paras, HWEnum_ErrorID err, HWEnum_BusiID busi, void* predata) 
        : m_id(id) , m_ctx(ctx) , m_err(err) , m_busi(busi) , m_uidata(predata) , m_paras(*paras)
    {}

    virtual ~CXResponse()
    {
        std::set<HWUIContext*>::iterator it = m_setContextUI.find(m_ctx);
        if (it != m_setContextUI.end()) m_setContextUI.erase(it);
        SAFE_DELETE(m_ctx);
    }

    virtual void Run()
    {
        if (!m_ctx || !m_ctx->doc)
        {
            CXLOG(lld_error, "resp process failed with doc is null, reqid:%d", m_id);
            return;
        }

        if(m_err == HWError_SUCCEED)
        {
            // 成功时调用
            m_ctx->doc->OnRespProcess(m_id, &m_paras, m_uidata);
        }

        m_ctx->doc->OnReqFinish(m_id, m_err, m_busi, &m_paras, m_ctx->pRequestor, m_ctx->docCtx);

    }
public:
    HWEnum_ReqID m_id;
    HWUIContext* m_ctx;
    HWParas m_paras;
    HWEnum_ErrorID m_err;
    HWEnum_BusiID m_busi;
    void* m_uidata;
};

juce_ImplementSingleton(HWDocManager)

HWDocManager::HWDocManager()
    : m_bIsDataModuleInited(false)
{
}

HWDocManager::~HWDocManager()
{
}

bool HWDocManager::Initialize()
{
    InitDataModules();

    // 初始化后台
    // 获取DeviceID
    juce::String strDeviceID;
    CXOption::DeviceID(strDeviceID);

    // 获取文档路径
    juce::String strDocPath = CXOption::getInstance()->GetUserDataPath();
    return HWBaseInstance()->Initialize(HWDevice_PC, strDeviceID.toUTF8(), strDocPath.toUTF8(), url_config, 4, dns_config, this);
}

bool HWDocManager::Finalize()
{
    HWBaseInstance()->Finalize();
    HWBaseDestory(HWBaseInstance());
    FinalDataModules();

    for (std::set<HWUIContext*>::iterator it = m_setContextUI.begin(), 
        itEnd = m_setContextUI.end(); it!=itEnd; ++it) 
        delete *it;
    m_setContextUI.clear();

    return true;
}

bool HWDocManager::SubmitRequest(HWDocBase* respReceiver, HWEnum_ReqID reqID, const HWParas* paras, 
                                 HWRequestor* pRequestor, HWContext ctx /*= 0*/, bool bPrior /*= false*/, bool bResp /*= true*/)
{
    HWUIContext* pCtx = new HWUIContext();
    m_setContextUI.insert(pCtx);

    pCtx->pRequestor = pRequestor;
    pCtx->docCtx = ctx;
    pCtx->doc = respReceiver;
    pCtx->tmStart = TickNow();
    
     bool bSucceed = HWBaseInstance()->SubmitRequest(reqID, (HWContext)pCtx, paras, bPrior, bResp);
    CXLOG(lld_info, "request %d submit %s", (int)reqID, bSucceed ? "ok" : "failed");
    return bSucceed;
}

void HWDocManager::OnRequestFinish( HWEnum_ReqID id, HWContext ctx, const HWParas* paras, 
                                   HWEnum_ErrorID err, HWEnum_BusiID busi, const HWDict* dict )
{
    HWUIContext* pCtx = (HWUIContext*)ctx;
    if (pCtx == NULL)
    {
        CXLOG(lld_error, "HWDocManager.OnRequestFinish ctx is null");
        return;
    }

    if(!m_bIsDataModuleInited || pCtx->doc == NULL)
    {
        CXLOG(lld_error, "HWDocManager.OnRequestFinish hwdocbase is null");
        SAFE_DELETE(pCtx);
        return;
    }

    // 记录执行时间
    CXLOG(lld_info, "request %d finish err:%d, busi:%d with time used:%d", (int)id, err, busi, TickDiff(pCtx->tmStart, TickNow()));

    void* predata = NULL;
    if(err == HWError_SUCCEED)
    {
        // 预处理
        predata = pCtx->doc->OnRespPreProcess(id, paras, dict);
    }

    // 发送消息给自己，异步处理响应结果
    (new CXResponse(id, pCtx, paras, err, busi, predata))->triggerAsyncUpdate();
}

void HWDocManager::OnSystemNotify( HWEnum_SysNotifyType sysNotify, HWContext ctx )
{
    (new CXExecNotifySys(sysNotify, ctx))->triggerAsyncUpdate();
}

void HWDocManager::OnBusiNotify( HWEnum_BusiNotifyType busiNotify, const HWDict* dict )
{
    (new CXExecNotifyBusi(busiNotify, dict))->triggerAsyncUpdate();
}

void HWDocManager::InitDataModules()
{
    FOR_EACH_VECTOR(i, m_vecDocModules) m_vecDocModules[i]->Initialize();
    m_bIsDataModuleInited = true;
}

void HWDocManager::FinalDataModules()
{
    m_bIsDataModuleInited = false;
    FOR_EACH_VECTOR(i, m_vecDocModules) m_vecDocModules[i]->Finalize();
}

void HWDocManager::CreateDataModules()
{
    HWDocHelper::getInstance();
    HWDocNotify::getInstance();
    m_vecDocModules.push_back(HWDocAuth::getInstance());
    m_vecDocModules.push_back(HWDocUserData::getInstance());
    m_vecDocModules.push_back(HWDocBookData::getInstance());
    m_vecDocModules.push_back(HWDocBookType::getInstance());
    m_vecDocModules.push_back(HWDocRead::getInstance());
    m_vecDocModules.push_back(HWDocDraw::getInstance());
    m_vecDocModules.push_back(HWDocSchool::getInstance());
    m_vecDocModules.push_back(HWDocRecommend::getInstance());
    m_vecDocModules.push_back(HWDocTag::getInstance());
    m_vecDocModules.push_back(HWDocBookMark::getInstance());
    m_vecDocModules.push_back(HWDocBookPostil::getInstance());
    m_vecDocModules.push_back(HWDocBookSign::getInstance());
    m_vecDocModules.push_back(HWDocComment::getInstance());
    m_vecDocModules.push_back(HWDocSpace::getInstance());
    m_vecDocModules.push_back(HWDocDynamic::getInstance());
}

void HWDocManager::DestoryDataModules()
{
    HWDocAuth::deleteInstance();
    HWDocUserData::deleteInstance();
    HWDocBookData::deleteInstance();
    HWDocBookType::deleteInstance();
    HWDocRead::deleteInstance();
    HWDocDraw::deleteInstance();
    HWDocSchool::deleteInstance();
    HWDocRecommend::deleteInstance();
    HWDocTag::deleteInstance();
    HWDocBookMark::deleteInstance();
    HWDocBookPostil::deleteInstance();
    HWDocBookSign::deleteInstance();
    HWDocComment::deleteInstance();
    HWDocSpace::deleteInstance();
    HWDocDynamic::deleteInstance();
    HWDocHelper::deleteInstance();
    HWDocNotify::deleteInstance();
    m_vecDocModules.clear();
}
