#pragma once

class MainComponent;
class CXTaskbarComponent;

class MainMenuCallBack
{
public:
    virtual void OnMainMenuCallBack(int nID) = 0;
    virtual void OnSysBtnCallBack(int nID) = 0;
    virtual void OnSysInfoCallBack(int nID) = 0;
};

class MenuComponent;

class MainAppWindow : public juce::CXDocumentWindow
{
public:
    MainAppWindow();
    ~MainAppWindow();

protected:
    void closeButtonPressed() override;

private:
    MainComponent* mainContentComponent;
    MenuComponent* menuBar;
    juce::ScopedPointer<CXTaskbarComponent> taskbar;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)
};

extern MainAppWindow* theMainApp;

class MainComponent 
    : public juce::TabbedComponent
    , public HWRequestor
    , public MainMenuCallBack
    , public IHWSubjectRead
{
public:
    MainComponent(MenuComponent& menu);
    ~MainComponent();


#if 1
    static void OnWebItemClick(int para, const std::vector<juce::String>& vecString);
#else
    static jsValue JS_CALL OnWebItemClick(jsExecState es);
#endif

protected:
    virtual void OnBookOpened() override;
    virtual void OnReadHistoryDel() override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual void OnMainMenuCallBack(int nID) override;
    virtual void OnSysBtnCallBack(int nID) override;
    virtual void OnSysInfoCallBack(int nID) override;

protected:
    void OnSysInfoPopupMenu(int nID);
    void requestReadHistoryList();
    void requestClassesInfos();
    void updateSysInfoHistory();
    void changeMainTabPage(int nID);
    void setTabWebUrl(int idx, juce::String url);
    void updateReadHistoryButtonInfo();
    void onAsyncWebItemClick(int id, const std::vector<juce::String>& paras);

private:
    class AsyncWebItemClick;
    friend class AsyncWebItemClick;
    friend class MainAppWindow;
    MenuComponent& menuBar;
    std::vector<HWTClassInfo> vecClassInfos;
    std::vector<std::pair<juce::String, juce::String>> vecSysInfoHostory;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

class MenuComponent
    : public juce::Component
    , public juce::Button::Listener
    , public juce::TextEditor::Listener
{
public:
    MenuComponent();
    ~MenuComponent();

    void setCallback(juce::MouseListener*, MainMenuCallBack*);

protected:
    virtual void buttonClicked (juce::Button*);
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;
    virtual void textEditorTextChanged (juce::TextEditor&) override;
    virtual void textEditorReturnKeyPressed (juce::TextEditor&) override;

private:
    friend class MainComponent;
    MainMenuCallBack* menuCallback;
    juce::ScopedPointer<juce::TextButton> buttonMenu[5];
    juce::ScopedPointer<juce::TextEditor> searchTextEditor;
    juce::ScopedPointer<juce::ImageComponent> iconSearch;
    juce::ScopedPointer<juce::ImageButton> btnSys[2];
    juce::ScopedPointer<juce::ImageButton> btnInfo[4];
    juce::ScopedPointer<juce::ImageButton> buttonClassExp;
    juce::ScopedPointer<juce::TextButton> buttonReadHistory;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};

