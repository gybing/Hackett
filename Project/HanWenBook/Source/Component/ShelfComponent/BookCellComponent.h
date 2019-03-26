#pragma once

class BookCellComponent : public juce::Component , public HWRequestor, public juce::ReferenceCountedObject
{
public:
    BookCellComponent(ShelfViewComponent& sv);
    ~BookCellComponent();

    void setData(const HWTUserBook* pUserBook, int idx);
    bool isValidBookCell() const;

    const HWTUserBook& getUserBook() const;

protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    virtual void paint(juce::Graphics&);

private:
    ShelfViewComponent& owner;
    int itemIndex;
    HWTUserBook m_UserBook;
    juce::Image m_imgBookFace;
    juce::Image m_imgBookShadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookCellComponent)
};

typedef juce::ReferenceCountedObjectPtr<BookCellComponent> BookCellPtr;
