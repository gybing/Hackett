#pragma once
#include "Browser/ReadBrowser.h"

class ReadComponent;
class ReadAppWindow : public juce::CXDocumentWindow
{
public:
    ReadAppWindow();
    ~ReadAppWindow();

    ReadComponent* getContent();

private:
    virtual void closeButtonPressed() override;

    ReadComponent* m_pContent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadAppWindow)
};

class ReadComponent : public juce::Component, public juce::Button::Listener
{
public:
    ReadComponent(juce::CXDocumentWindow* window);
    ~ReadComponent();

    BookBrowser* getReadBrowser();
    juce::VerticalToolMenu* getLeftToolMenu();
    juce::VerticalToolMenu* getRightToolMenu();

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (juce::Button*);

protected:
    void createLeftToolMenu();
    void createRightToolMenu();

private:
    bool bookInfoExpendIsShow;
    juce::ScopedPointer<juce::VerticalToolMenu> toolMenuLeft;
    juce::ScopedPointer<juce::VerticalToolMenu> toolMenuRight;
    juce::ScopedPointer<BookBrowser> bookBrowser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadComponent)
};

class ReadWindowHelper : public HWRequestor
{
public:
    juce_DeclareSingleton(ReadWindowHelper, false)
    ReadWindowHelper();
    ~ReadWindowHelper();

    void readBook(const juce::String& strBook, int pageno = ProjectInfo::PagenoInvalid);
    void reload();
    void gotoPage(int pageno);

    void showWindow();
    void closeWindow();
    void setMinimised(bool bmin);
    ReadAppWindow* getWindow();
    BookView* getBookView();
    BookBrowser* getBookBrowser();
    juce::ToolButton* findReadMenuByID(int id);

protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

private:
    juce::ScopedPointer<ReadAppWindow> readContentComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadWindowHelper)
};
