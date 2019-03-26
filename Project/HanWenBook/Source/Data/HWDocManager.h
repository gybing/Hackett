#ifndef __DATAMANAGER__HH__
#define __DATAMANAGER__HH__

#include "HWBase.h"

/*
    HWDataManager���̨������
    ��UI���̨���׸���
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

    // ��������ģ��
    void CreateDataModules();
    void DestoryDataModules();

    // ����ģ���ʼ��
    bool Initialize();
    bool Finalize();

    // ��������
    // paras�ɵ������ͷţ�����Ϊջ���󼴿ɣ���������������������ΪNULL.
    virtual bool SubmitRequest(HWDocBase* respReceiver, HWEnum_ReqID reqID, const HWParas* paras, 
        HWRequestor* pRequestor, HWContext ctx = 0, bool bPrior = false, bool bResp = true);

protected:
    // ��̨������ʵ��,������Ӧ����Ϣ��֪ͨ��
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