#pragma once
#include "BookPage.h"

class BookPageSign
    : public HWRequestor
    , public IBookPageInfo
    , public IHWSubjectBookSign
{
public:
    BookPageSign(BookPage& o);
    ~BookPageSign();

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);
    virtual void onPageComplate() override;
    virtual void OnBookSignAdd() override;
    virtual void OnBookSignDel() override;
    virtual void OnBookSignListChanged() override;

    void initSignPageBlocks();
    void caculateSignDrawLines();

    // ±ê¼ÇÇøÓò
    typedef struct tagFloatLine
    {
        float x0, y0;
        float x1, y1;

        tagFloatLine() : x0(0.0f), y0(0.0f), x1(0.0f), y1(0.0f) {}
        tagFloatLine(float X0, float Y0, float X1, float Y1):x0(X0),y0(Y0),x1(X1),y1(Y1){}
        void Offset(float x, float y) { x0+=x; x1+=x; y0+=y; y1+=y; }
        void Offset(int x, int y) { Offset(1.0f * x, 1.0f * y); }
    }FloatLine;

private:
    BookPage& owner;
    HWPageObjectUI::Ptr pageObject;
    std::vector<std::pair<FloatLine, int>> signCacheLines;
    std::vector<std::pair<juce::Line<float>, int>> signDrawLines;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookPageSign)
};