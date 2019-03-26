#ifndef __DATA_DRAW__
#define __DATA_DRAW__
#include "HWDocBase.h"
#include "KnlRender.h"
#include "json.h"
#include "knlBlock.h"

class HWPageObjectUI : public juce::ReferenceCountedObject
{
public:
    typedef juce::ReferenceCountedObjectPtr<HWPageObjectUI> Ptr;
    
    PageObject* getPageObject();

    HWPageObjectUI(PageObject* po);
    ~HWPageObjectUI();

private:
    PageObject* object;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWPageObjectUI)
};

class HWParaPageDraw : public HWParaUI
{
public:
    HWParaPageDraw(const juce::String& strBook, int nPageNo, 
        int nWidth, int nHeight, float fHvzp, 
        HWDrawMode nMode, int uBack, int nAlign, 
        HWDrawBitOrder bOrder, int nLeft, int nUp, int nRight, int nDown)
        : m_strBook(strBook)
        , m_nPageNo(nPageNo)
        , m_nWidth(nWidth)
        , m_nHeight(nHeight)
        , m_fHvzp(fHvzp)
        , m_nMode(nMode)
        , m_uBack(uBack)
        , m_nAlign(nAlign)
        , m_nBitOrder(bOrder)
        , m_nClipLeft(nLeft)
        , m_nClipUp(nUp)
        , m_nClipRight(nRight)
        , m_nClipDown(nDown)
    {
    }

protected:
    friend class HWDocDraw;
    HWParaPageDraw(){}
    virtual const HWParas* ToHWPara()
    {
        m_Para.Add("guid", m_strBook.toUTF8());
        m_Para.Add("pageno", m_nPageNo);
        m_Para.Add("width", m_nWidth);
        m_Para.Add("height", m_nHeight);
        m_Para.Add("hvzp", m_fHvzp);
        m_Para.Add("mode", (int)m_nMode);
        m_Para.Add("back", (int)m_uBack);
        m_Para.Add("align", m_nAlign);
        m_Para.Add("bitorder", (int)m_nBitOrder);
        m_Para.Add("left", m_nClipLeft);
        m_Para.Add("up", m_nClipUp);
        m_Para.Add("right", m_nClipRight);
        m_Para.Add("down", m_nClipDown);

        return &m_Para;
    }

    virtual bool FromHWPara(const HWParas* para)
    {
        m_strBook = para->StrVal("guid");
        m_nPageNo = para->IntVal("pageno");
        m_nWidth = para->IntVal("width");
        m_nHeight = para->IntVal("height");
        m_fHvzp = para->FloatVal("hvzp");
        m_nMode = (HWDrawMode)para->IntVal("mode");
        m_uBack = para->IntVal("back");
        m_nAlign = para->IntVal("align");
        m_nBitOrder = (HWDrawBitOrder)para->IntVal("bitorder");
        m_nClipLeft = para->IntVal("left");
        m_nClipUp = para->IntVal("up");
        m_nClipRight = para->IntVal("right");
        m_nClipDown = para->IntVal("down");
        return true;
    }

public:
    juce::String m_strBook;
    int m_nPageNo;
    int m_nWidth;
    int m_nHeight;
    float m_fHvzp;
    HWDrawMode m_nMode;
    int m_uBack;
    int m_nAlign;
    HWDrawBitOrder m_nBitOrder;
    int m_nClipLeft;
    int m_nClipUp;
    int m_nClipRight;
    int m_nClipDown;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWParaPageDraw)
};

// 用户数据列表 头像 用户资料
class HWDocDraw : 
    public HWDocBase
{
public:
    juce_DeclareSingleton(HWDocDraw, false)
    // 数据方法定义
    HWPageObjectUI::Ptr GetPageObject();

public:
    HWDocDraw();
    virtual ~HWDocDraw();

protected:
    virtual void OnInit();
    virtual void OnFinal();

    virtual HWParaUI* MakeHWParaUI(HWEnum_ReqID reqID, const HWParas* para);

    virtual void* OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict);
    virtual void OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data);

protected:
    HWPageObjectUI::Ptr m_pPageObject;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocDraw)
};

#endif // __DATA_DRAW__