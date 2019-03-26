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

// HW���ݶ���Ķ��ġ�����ģʽģ��
// ������ݶ�����Ҫʵ�ֱ�������ģ���̳���HWSubject<�Զ��巢���ص��ӿ�X>
// ���UIҪ�������ݶ���ı��֪ͨ������̳��������ӿ�X
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

// ��̨���������UI���װ
class HWParaUI
{
public:
    virtual ~HWParaUI(){}
    
    // ��������
    // ͨ�������ҵ���������������HWPara��
    virtual const HWParas* ToHWPara() = 0;

    // ��������
    // ͨ��HWPara����
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

// ��������
// �ɷ��������UI����̳�, ������OnReqFinish����
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

    // ����������
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

    // ��Ӧ�ɹ�ʱ������
    // �˺����ں�̨�߳���ִ�У�Ϊ������Doc���м���������Ӧ�������ݵ�Ԥ�ӹ�����
    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* m_dict) = 0;

    // ��Ӧ�ɹ�ʱ������
    // �˺��������߳���ִ�У�����Ԥ�ӹ���������ݽ��data���޸ı��ػ������ݡ�
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data) = 0;

    // ������Ӧ�ɹ����������ã� �������ء�
    // �˺��������߳���ִ�У��˺�������DocManager���������б�������.
    virtual void OnReqFinish(HWEnum_ReqID id, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWParas* paras, HWRequestor* pRequestor, HWContext ctx);

protected:
    void Initialize() { OnInit(); }
    void Finalize() { OnFinal(); }

    virtual void OnInit() = 0;
    virtual void OnFinal() = 0;

    // ������Ӧ����
    // ÿ��UI��������ǰ���˺����ᱻ���ã�����Ӧ�������������鿴�Ƿ񱾵ؿ���������Ӧ��
    // ������ԣ��򷵻�true�����򷵻�false������ᷢ�����󵽺�̨��
    virtual bool LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx ){ return false; }

    // ����д�������������ʧ�ܣ���ᵼ��UI��������ʧ�ܣ�����DocBase���������ͺ�̨����
    virtual bool LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx ){ return true; }

    // �������UI����ת��
    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para) = 0;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocBase)
};

