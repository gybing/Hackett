#pragma once
#include "HWBase.h"
#include <list>
#include <set>
#include "HWDocManager.h"
#include "HWDocStruct.h"
#include "HWDocHelper.h"

#define HWNOTIFY(func) \
for (unsigned int _uIdx=0; _uIdx<m_Listeners.size(); ++_uIdx) \
{ m_Listeners[_uIdx]->func; } \

#define HWNOTIFYR(func) \
    for (unsigned int _uIdx=m_Listeners.size()-1; _uIdx<m_Listeners.size(); --_uIdx) \
{ m_Listeners[_uIdx]->func; } \

// HW数据对象的订阅—发布模式模板
// 如果数据对象需要实现变更被订阅，则继承自HWSubject<自定义发布回调接口X>
// 如果UI要接收数据对象的变更通知，则需继承自上述接口X
template<class T>
class HWSubject 
{
public:
    ~HWSubject()
    {
        m_Listeners.clear();
    }

    void Register(T* pListener)
    {
        m_Listeners.push_back(pListener);
    }

    void UnRegister(T* pListener)
    {
        typename std::vector<T*>::iterator iter = m_Listeners.begin();
        while(iter != m_Listeners.end())
        {
            if(pListener == *iter)
            {
                iter = m_Listeners.erase(iter);
                continue;
            }
            iter++;
        }
    }

protected:
    HWSubject(){}
    std::vector<T*> m_Listeners;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWSubject)
};

// 后台请求参数的UI层封装
class HWParaUI
{
public:
    virtual ~HWParaUI(){}
    
    // 子类重载
    // 通过子类的业务请求参数，生成HWPara。
    virtual const HWParas* ToHWPara() = 0;

    // 子类重载
    // 通过HWPara构造
    virtual bool FromHWPara(const HWParas* para) = 0;

protected:
    HWParaUI(){}
    friend class HWDocBase;
    HWParas m_Para;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUI)
};

class HWParaUIEmpty : public HWParaUI
{
protected:
    const HWParas* ToHWPara(){ return &m_Para; }
    bool FromHWPara(const HWParas* para){ return true; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaUIEmpty)
};

// 请求发送者
// 由发送请求的UI对象继承, 并重载OnReqFinish函数
class HWRequestor
{
public:
    static void OnResponse(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
    {
        std::set<HWRequestor*>::iterator iter = m_setObjs.find(pRequestor);
        if(iter != m_setObjs.end()) 
        {
            pRequestor->OnResponse(reqID, err, busi, ctx, para);
            return;
        }
        //jassert(false);
    }

protected:
    HWRequestor()
    {
        m_setObjs.insert(this);
    }

    virtual ~HWRequestor()
    {
        std::set<HWRequestor*>::iterator iter = m_setObjs.find(this);
        if(iter != m_setObjs.end()) m_setObjs.erase(iter); 
    }

    // 派生类重载
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) = 0;

    virtual void ClearResponse()
    {
        std::set<HWRequestor*>::iterator iter = m_setObjs.find(this);
        if(iter != m_setObjs.end()) m_setObjs.erase(iter); 
    }

protected:
    static std::set<HWRequestor*> m_setObjs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWRequestor)
};

class HWDocBase
{
public:
    HWDocBase();
    virtual ~HWDocBase();

    bool SubmitRequest(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx, bool bPrior = false, bool bResp = true);

protected:
    friend class CXResponse;
    friend class HWDocManager;

    // 响应成功时被调用
    // 此函数在后台线程中执行，为避免在Doc类中加锁，这里应仅做数据的预加工处理
    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict) = 0;

    // 响应成功时被调用
    // 此函数在主线程中执行，根据预加工处理的数据结果data，修改本地缓存数据。
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data) = 0;

    // 无论响应成功与否均被调用， 不可重载。
    // 此函数在主线程中执行，此函数会在DocManager处理流程中被最后调用.
    virtual void OnReqFinish(HWEnum_ReqID id, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWParas* paras, HWRequestor* pRequestor, HWContext ctx);

protected:
    void Initialize() { OnInit(); }
    void Finalize() { OnFinal(); }

    virtual void OnInit() = 0;
    virtual void OnFinal() = 0;

    // 本地响应处理
    // 每当UI发起请求前，此函数会被调用，子类应检查请求参数，查看是否本地可以立即响应，
    // 如果可以，则返回true，否则返回false，基类会发送请求到后台。
    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx ){ return false; }

    // 本地写操作，如果返回失败，则会导致UI发送请求失败，否则，DocBase类会继续发送后台请求。
    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx ){ return true; }

    // 子类完成UI参数转换
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para) = 0;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBase)
};

