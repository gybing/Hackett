#pragma once

class ReadInfoBrief 
    : public juce::Component
    , public HWRequestor
    , public IHWSubjectRead
{
public:
    ReadInfoBrief();
    ~ReadInfoBrief();

protected:
    void resized() override;
    virtual void visibilityChanged();
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual void OnBookOpened() override;

    void initData();

private: 
    bool hasNeedInitData, justShowChapter;
    juce::String strBook;
    juce::ScopedPointer<juce::ImageComponent> bookFace;
    juce::ScopedPointer<juce::Label> bookName;
    juce::ScopedPointer<juce::Label> bookAuthor;
    juce::ScopedPointer<juce::Label> bookPublish;
    juce::ScopedPointer<juce::Label> bookPubData;
    juce::ScopedPointer<juce::TextEditor> textBrief;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoBrief)
};