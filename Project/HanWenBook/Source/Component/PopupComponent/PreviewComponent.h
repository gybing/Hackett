#pragma once

class PreviewComponent : public juce::Component, public HWRequestor, public juce::ButtonListener
{
public:
    PreviewComponent(const juce::String& strBook);
    ~PreviewComponent();


    static void showPreviewWindow(juce::Component* parent, const juce::String& strBook);

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (juce::Button*);

    void bookFaceClicked(juce::Button*);
protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    void initDefaultComponent();
    void RequestBookInfo();
    void RequestBookBrief();

private:
    juce::ScopedPointer<juce::TabbedComponent> m_pTabbedComponent;
    juce::ScopedPointer<juce::TextEditor> m_pTextBrief;
    juce::ScopedPointer<juce::ImageButton> m_pImageBookFaceView;
    juce::ScopedPointer<juce::ColorButton> m_pButtonCtrl[5];

    juce::String m_strBook;
    juce::String m_strBookName;
    juce::Rectangle<int> m_rcBookInfo;
    juce::AttributedString m_attrBookInfo[6];
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreviewComponent)
};