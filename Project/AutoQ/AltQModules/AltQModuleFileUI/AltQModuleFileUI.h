#pragma once
#include <string>
#include "AltQBase.h"
#include "AltQInterfaceModuleUI.h"
using namespace juce;

Image updateIcon(const File& file);

class AltQModuleFileUI : public IAltQModuleUI
{
public:
    typedef std::shared_ptr<AltQModuleFileUI> Ptr;

    AltQModuleFileUI();
    ~AltQModuleFileUI();

protected:
    virtual bool Initialize(const IAltQModulePtr& module) override;
    virtual AltQWindow* RefreshResultItemComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) override;
    virtual AltQWindow* RefreshResultDetailComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) override;
    virtual bool ShowPopupMenu(const IAltQModuleResultPtr& result) override;
    virtual bool OnResultItemExcute(const IAltQModuleResultPtr& result) override;
    virtual void Finalize() override;
    
protected:
    bool openAndSelectDirectoryFile(const File& file);
    bool openDirectoryFile(const File& file);

private:
    IAltQModulePtr modulePtr;
};

class AltQFileResultItem : public Component
{
public:
    AltQFileResultItem();
    ~AltQFileResultItem();

    bool setContent(const IAltQModuleResultPtr& result);

protected:
    virtual void paint(Graphics& g) override;
    
private:
    String resultTitle;
    String resultDescription;
    Image resultIconImage;
};

class AltQFileDetailWindow : public Component
{
public:
    AltQFileDetailWindow();
    ~AltQFileDetailWindow();

    bool setContent(const IAltQModuleResultPtr& result);

protected:
    virtual void paint(Graphics& g) override;

private:
    String resultTitle;
    String resultDescription;
    Image resultIconImage;
    File targetFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQFileDetailWindow)
};
