#pragma once
#include "../Info/ReadInfoComponent.h"

enum BookViewType { BVT_VER, BVT_HOR, BVT_HORL, BVT_HORR};

class BookView : public juce::Component
{
public:
    BookView();
    ~BookView();

    virtual void gotoPage(int page) = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual void pagePrev() = 0;
    virtual void pageNext() = 0;
    virtual void setProtectEye(bool eye) = 0;
    virtual void setExpandShow(bool show) = 0;
    virtual void reload() = 0;
    virtual void update() = 0;

protected:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookView)
};

class BookBrowser
    : public juce::Component, public juce::VerticalToolMenu::VToolListener
    , public IHWSubjectRead
{
public:
    BookBrowser();
    ~BookBrowser();

    BookView* getBookView();
    void changeBookViewType();

protected:
    void paint (juce::Graphics&) override;
    void resized() override;
    virtual void onVToolMenuItemClicked(juce::VerticalToolMenu*, juce::Button* b) override;
    virtual void OnBookOpened() override;

protected:
    void showExpandWindow(bool show);
    void setBookViewType(bool bHor);
    void updateCollectButtonState();

private:
    juce::ScopedPointer<ReadInfoComponent> readInfoContent;
    juce::ScopedPointer<BookView> bookView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookBrowser)
};