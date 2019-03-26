#include "JuceHeader.h"
#include "BookPageSign.h"

BookPageSign::BookPageSign(BookPage& o)
    : owner(o)
{
    HWDocBookSign::getInstance()->Register(this);
}

BookPageSign::~BookPageSign()
{
    HWDocBookSign::getInstance()->UnRegister(this);
}

void BookPageSign::resized()
{
    signDrawLines.clear();
    signCacheLines.clear();
    repaint();
    initSignPageBlocks();
}

void BookPageSign::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
}

void BookPageSign::onPageComplate()
{
    signDrawLines.clear();
    signCacheLines.clear();
    repaint();

    pageObject = owner.getPageObject();
    initSignPageBlocks();
}

void BookPageSign::initSignPageBlocks()
{
    if (pageObject == nullptr)
        return;

    PageObject* pageObj = pageObject->getPageObject();
    if (pageObj == nullptr)
        return;

    std::vector<HWTBookSign*> vecSigns;
    HWDocBookSign::getInstance()->GetPageSignList(owner.getPageNo(), vecSigns);
    if (vecSigns.empty())
        return;

    int nRemain; // 跨页剩余
    for (unsigned int i=0; i<vecSigns.size(); ++i)
    {
        HWTBookSign* pSign = vecSigns[i];
        const PageBlockGroup* pPageBlockGroup = pageObj->Highlight(pSign->nBe[0], pSign->nBe[1], nRemain);
        if (!pPageBlockGroup || pPageBlockGroup->empty())
            continue;

        float fX0, fY0, fX1, fY1;
        for (unsigned int j =0 ; j < pPageBlockGroup->size(); ++j)
        {
            const PageBlock* pb = (*pPageBlockGroup)[j];
            if (((PageBlockLine*)pb)->mode)
            {
                pageObj->Inverse(((PageBlockLine*)pb)->x0, pb->y0, fX0, fY0);
                pageObj->Inverse(((PageBlockLine*)pb)->x0, pb->y1, fX1, fY1);
            }
            else
            {
                int destY = juce::jmax(((PageBlockLine*)pb)->y0, ((PageBlockLine*)pb)->y1); 
                pageObj->Inverse(pb->x0, destY, fX0, fY0);
                pageObj->Inverse(pb->x1, destY, fX1, fY1);
            }

            signCacheLines.push_back(std::pair<FloatLine,int>(FloatLine(fX0, fY0, fX1, fY1), pSign->nId));
        }
    }

    caculateSignDrawLines();
}

void BookPageSign::caculateSignDrawLines()
{
    if (pageObject == nullptr)
        return;

    PageObject* pageObj = pageObject->getPageObject();
    if (pageObj == nullptr)
        return;

    unsigned int nAllCount = signCacheLines.size();

    int nOffsetX = getX();
    int nOffsetY = getY();

    int nX, nY;
    FloatLine line;

    for (unsigned int i=0; i<signCacheLines.size(); ++i)
    {
        FloatLine& floatline = signCacheLines[i].first;

        pageObj->Transfer(floatline.x0, floatline.y0, nX, nY);
        line.x0	= nX; line.y0 = nY;

        pageObj->Transfer(floatline.x1, floatline.y1, nX, nY);
        line.x1	= nX; line.y1 = nY;

        line.Offset(nOffsetX, nOffsetY);

        // line.x0, line.y0, line.x1, line.y1
        if (line.y0==line.y1)
        {
            signDrawLines.push_back(std::pair<juce::Line<float>,int>(juce::Line<float>(line.x0, --line.y0, line.x1, line.y1), signCacheLines[i].second));
        }
        else if (line.x0==line.x1)
        {
            signDrawLines.push_back(std::pair<juce::Line<float>,int>(juce::Line<float>(line.x0, line.y0, ++line.x1, line.y1), signCacheLines[i].second));
        }
        else
        {
            jassert(false);
        }
    }
    
    repaint();
}

void BookPageSign::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFFFF0000));
    for (unsigned int i=0; i<signDrawLines.size(); ++i)
    {
        const juce::Line<float>& line = signDrawLines[i].first;
        g.drawLine(line, 1);
    }
}

void BookPageSign::OnBookSignListChanged()
{
    initSignPageBlocks();
}

void BookPageSign::OnBookSignAdd()
{

}

void BookPageSign::OnBookSignDel()
{

}
