#ifndef __DATAMANAGER__HH__
#define __DATAMANAGER__HH__

#include "HWBase.h"

/*
    HWDataManager与后台交互，
    将UI与后台彻底隔离
*/
struct HWUIContext;
class HWRequestor;
class HWDocBase;
class IHWDoc;
class HWDocManager 
    : public IHWDelegate
{
public:
    juce_DeclareSingleton(HWDocManager, false)
    HWDocManager();
    ~HWDocManager();

    // 创建数据模块
    void CreateDataModules();
    void DestoryDataModules();

    // 数据模块初始化
    bool Initialize();
    bool Finalize();

    // 发送请求
    // paras由调用者释放，定义为栈对象即可，如果无请求参数，可设置为NULL.
    virtual bool SubmitRequest(HWDocBase* respReceiver, HWEnum_ReqID reqID, const HWParas* paras, 
        HWRequestor* pRequestor, HWContext ctx = 0, bool bPrior = false, bool bResp = true);

protected:
    // 后台代理方法实现,处理响应、消息、通知。
    virtual void OnRequestFinish(HWEnum_ReqID id, HWContext ctx, const HWParas* paras, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWDict* dict);
    virtual void OnSystemNotify(HWEnum_SysNotifyType sysNotify, HWContext ctx); 
    virtual void OnBusiNotify(HWEnum_BusiNotifyType busiNotify, const HWDict* dict);

protected:

    void InitDataModules();
    void FinalDataModules();

protected:
    std::vector<HWDocBase*> m_vecDocModules;
    bool m_bIsDataModuleInited;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocManager)
};
 
#endif // __DATAMANAGER__HH__