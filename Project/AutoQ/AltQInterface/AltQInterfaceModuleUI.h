#pragma once
#include "AltQDefine.h"
#include "AltQInterfaceModule.h"
#include "AltQBase.h"

typedef juce::Component AltQWindow;

class IAltQModuleUI
{
public:
    // ��ʼ��
    virtual bool Initialize(const IAltQModulePtr& module) = 0;

    // ˢ����������б���ʾ��, ���Է��ؿա�
    // �������Ҫ��ʾʱ���ã�����ܸ����ڴ����
    // existingComponentToUpdate��Ϊ�ձ�ʾ��Ҫˢ��
    // ��ʱӦ��ʼ�յ���static_cast<YourAltQWindow*>(existingComponentToUpdate)ת��
    // ���ת��ʧ��Ӧ�ô����µ� YourAltQWindow
    virtual AltQWindow* RefreshResultItemComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) = 0;

    // ˢ������Ԥ������
    virtual AltQWindow* RefreshResultDetailComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) = 0;

    // ������ʾ�б�����Ҽ��˵�
    virtual bool ShowPopupMenu(const IAltQModuleResultPtr& result) = 0;

    // �û�˫����ĳһ�����ĳһ����˻س���
    virtual bool OnResultItemExcute(const IAltQModuleResultPtr& result) = 0;

    // ж��
    // ������AltCore��Finalizeǰִ�д�Finalize.
    virtual void Finalize() = 0;
};
typedef std::shared_ptr<IAltQModuleUI> IAltQModuleUIPtr;

// ÿ��AltQUIģ�����ʵ�ִ˽ӿ�
extern "C" ALTQ__API bool CreateAltQModuleUI(IAltQModuleUIPtr* module);

