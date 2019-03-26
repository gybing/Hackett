#pragma once
#include <string>
#include "AltQBase.h"
#include "AltQInterfaceModuleUI.h"
using namespace juce;

Image updateIcon(const File& file);

class AltQModuleRadioUI : public IAltQModuleUI
{
public:
    typedef std::shared_ptr<AltQModuleRadioUI> Ptr;

    AltQModuleRadioUI();
    ~AltQModuleRadioUI();

protected:
    virtual bool Initialize(const IAltQModulePtr& module) override;
    virtual AltQWindow* RefreshResultItemComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) override;
    virtual AltQWindow* RefreshResultDetailComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) override;
    virtual bool ShowPopupMenu(const IAltQModuleResultPtr& result) override;
    virtual bool OnResultItemExcute(const IAltQModuleResultPtr& result) override;
    virtual void Finalize() override;

private:
    IAltQModulePtr modulePtr;
};

class AltQRadioResultItem : public Component
{
public:
    AltQRadioResultItem();
    ~AltQRadioResultItem();

    bool setContent(const IAltQModuleResultPtr& result);

protected:
    virtual void paint(Graphics& g) override;

private:
    String resultTitle;
    String resultDescription;
    Image resultIconImage;
};

class AltQRadioDetailWindow : public Component
{
public:
    AltQRadioDetailWindow();
    ~AltQRadioDetailWindow();

    bool setContent(const IAltQModuleResultPtr& result);

protected:
    virtual void paint(Graphics& g) override;

private:
    String resultTitle;
    String resultDescription;
    Image resultIconImage;
    File targetFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQRadioDetailWindow)
};
