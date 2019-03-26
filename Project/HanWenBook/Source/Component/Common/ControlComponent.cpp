#include "JuceHeader.h"
#include "ControlComponent.h"

namespace juce
{

    ColorButton::ColorButton ()
        : Button(String::empty)
        , drawButtonThickness(1)
        , drawCornerSize(0.0f)
    {
    }

    void ColorButton::paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        if (drawButtonThickness != 0)
        {
            g.fillAll(findColour(ColourIds::buttonBackgroundId));
            g.setColour(isEnabled() ? findColour(isButtonDown ? 
                ColourIds::buttonColourDownId : (
                isMouseOverButton ? ColourIds::buttonColourHoverId :
                ColourIds:: buttonColourNormalId)) : ProjectInfo::colorLightGrey);

            if (drawCornerSize > 0.0f)
                g.drawRoundedRectangle(0.0f, 0.0f, (float)getWidth(), (float)getHeight(), drawCornerSize, drawButtonThickness*1.0f);
            else
                g.drawRect(0, 0, getWidth(), getHeight(), drawButtonThickness);
        }
        else
        {
            g.fillAll(isEnabled() ? findColour(isButtonDown ? 
                ColourIds::buttonColourDownId : (
                isMouseOverButton ? ColourIds::buttonColourHoverId :
                ColourIds:: buttonColourNormalId)) : ProjectInfo::colorLightGrey);
        }

        g.setColour(findColour(isButtonDown ? 
            ColourIds::textColourDownId : (
            isMouseOverButton ? ColourIds::textColourHoverId :
            ColourIds::textColourNormalId)));

        float fontHeight = jmin(getHeight(), getWidth()) * 0.6f;
        g.setFont (Font(fontHeight));

        StringArray s;
        s.addTokens (getButtonText(), String(L"\n"), String(L"\n"));
        s.trim();
        s.removeEmptyStrings();

        int height = (getHeight() - (s.size()*fontHeight))/2.0f;
        for (int i=0; i<s.size(); ++i)
        {
            g.drawText (s[i], 0, height+(i*fontHeight), getWidth(), fontHeight,Justification::centred, false);
        }
    }

    // 设置线框大小 0代表全色填充
    void ColorButton::setButtonBorder(int lineThickness, float corner /*= 0.0f*/)
    {
        drawButtonThickness = lineThickness;
        drawCornerSize = corner;
        repaint();
    }

    void ColorButton::changeWidthToFitText()
    {
        changeWidthToFitText (getHeight());
    }

    void ColorButton::changeWidthToFitText (const int newHeight)
    {
        setSize (getBestWidthForHeight (newHeight), newHeight);
    }

    int ColorButton::getBestWidthForHeight (int buttonHeight)
    {
        return Font (buttonHeight * 0.9f).getStringWidth (getButtonText()) + buttonHeight;
    }

    //////////////////////////////////////////////////////////////////////////
    // VToolButton
    ToolButton::ToolButton (const String& name, int height, 
        const Drawable* norImage, 
        const Drawable* ovrImage /*= nullptr*/, 
        const Drawable* dwnImage /*= nullptr*/,
        bool aboveText /*= false*/)
        : Button (name)
        , textHeight(height)
        , imageAboveTextLabel(aboveText)
        , textPosX(0)
    {
        setImages(norImage, ovrImage, dwnImage);
    }

    ToolButton::ToolButton (const String& name, bool aboveText/* = true*/)
        : Button (name)
        , textHeight(0)
        , imageAboveTextLabel(aboveText)
        , currentImage(nullptr)
        , textPosX(0)
    {
    }

    ToolButton::~ToolButton()
    {

    }

    void ToolButton::setImages (const Drawable* norImage, 
        const Drawable* ovrImage /*= nullptr*/,
        const Drawable* dwnImage /*= nullptr*/)
    {
        normalImage = norImage != nullptr ? norImage->createCopy() : nullptr;
        overImage = ovrImage != nullptr ? ovrImage->createCopy() : nullptr;
        downImage = dwnImage != nullptr ? dwnImage->createCopy() : nullptr;
        currentImage = nullptr;
        buttonStateChanged();
    }

    Drawable* ToolButton::getCurrentImage() const noexcept
    {
        return isDown() ? getDownImage() : (getToggleState() || isOver() ? getOverImage() : getNormalImage());
    }

    Drawable* ToolButton::getNormalImage() const noexcept
    {
        return normalImage;
    }

    Drawable* ToolButton::getOverImage() const noexcept
    {
        return overImage == nullptr ? normalImage : overImage;
    }

    Drawable* ToolButton::getDownImage() const noexcept
    {
        return downImage == nullptr ? normalImage : downImage;
    }

    int ToolButton::getButtonHeight() const
    {
        StringArray s;
        s.addTokens (getButtonText(), String(L"\n"), String(L"\n"));
        s.trim();
        s.removeEmptyStrings();
        return (s.size() * textHeight) + normalImage->getHeight() + 2;
    }

    void ToolButton::setButtonHeight(int h)
    {
        textHeight = h;
        repaint();
    }

    void ToolButton::setButtonTextPos(int x)
    {
        textPosX = x;
        repaint();
    }

    Rectangle<float> ToolButton::getImageBounds() const
    {
        Rectangle<int> r (getLocalBounds());
        r.setHeight(jmin(normalImage->getHeight(), r.getHeight()));
        return r.toFloat();
    }

    void ToolButton::paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        bool toggleState = getToggleState();
        const int width = getWidth();

        g.fillAll (findColour (toggleState ? ToolButton::backgroundOnColourId
            : ToolButton::backgroundColourId));

        if (imageAboveTextLabel)
        {
            if (textHeight > 0)
            {
                g.setFont (juce::Font((float) textHeight));

                g.setColour (findColour (toggleState || isMouseOverButton || isButtonDown ? ToolButton::textColourOnId
                    : ToolButton::textColourId)
                    .withMultipliedAlpha (isEnabled() ? 1.0f : 0.4f));

                StringArray s;
                s.addTokens (getButtonText(), String(L"\n"), String(L"\n"));
                s.trim();
                s.removeEmptyStrings();

                int height = getHeight() - (s.size()*textHeight) - 1;
                for (int i=0; i<s.size(); ++i)
                {
                    g.drawText (s[i], 2, height+(i*textHeight), width - 4, textHeight,Justification::centred, false);
                }
            }
        }
        else
        {
            g.setFont (juce::Font((float) textHeight > 0 ? textHeight : getHeight()));

            g.setColour (findColour (toggleState || isMouseOverButton || isButtonDown ? ToolButton::textColourOnId
                : ToolButton::textColourId)
                .withMultipliedAlpha (isEnabled() ? 1.0f : 0.4f));

            float x = textPosX;

            if (x == 0)
            {
                x = getHeight();

                if (normalImage != nullptr)
                {
                    x = ((float)getHeight() * normalImage->getWidth() / normalImage->getHeight()) + 5.0f;
                }

                const juce::String& strText = getButtonText();
                int strW = g.getCurrentFont().getStringWidth(strText);
                if (width - x < strW)
                {
                    x = width - strW;
                }
            }
            
            g.drawFittedText(getButtonText(), juce::Rectangle<int>(x, 0, width - x, getHeight()), Justification::centredLeft, 1, 1.0f);
        }
    }

    void ToolButton::buttonStateChanged()
    {
        repaint();

        Drawable* imageToDraw = nullptr;
        float opacity = 1.0f;

        if (isEnabled())
        {
            imageToDraw = getCurrentImage();
        }
        else
        {
            if (imageToDraw == nullptr)
            {
                opacity = 0.4f;
                imageToDraw = getNormalImage();
            }
        }

        if (imageToDraw != currentImage)
        {
            removeChildComponent (currentImage);
            currentImage = imageToDraw;

            if (currentImage != nullptr)
            {
                currentImage->setInterceptsMouseClicks (false, false);
                addAndMakeVisible (currentImage);
                resized();
            }
        }

        if (currentImage != nullptr)
            currentImage->setAlpha (opacity);
    }

    void ToolButton::resized()
    {
        Button::resized();

        if (currentImage != nullptr)
        {
            currentImage->setTransformToFit (getImageBounds(), imageAboveTextLabel ? RectanglePlacement::centred : RectanglePlacement::xLeft);
        }
    }

    void ToolButton::enablementChanged()
    {
        Button::enablementChanged();
        buttonStateChanged();
    }

    void ToolButton::colourChanged()
    {
        repaint();
    }


    //////////////////////////////////////////////////////////////////////////
    // VerticalToolMenu
    VerticalToolMenu::VerticalToolMenu(VerticalToolMenu::VToolListener* l /*= nullptr*/) 
        : itemInset(10)
        , eventListener(l)
        , itemEdge(-1)
    {

    }
    VerticalToolMenu::~VerticalToolMenu()
    {

    }

    void VerticalToolMenu::appand(bool front)
    {
        if (front) menusFront.add(nullptr);
        else menusBack.add(nullptr);
    }

    Button* VerticalToolMenu::appand(bool front, int id, Button* btn)
    {
        if (btn)
        {
            addChildAndSetID(btn, String(id));
            btn->addListener(this);

            if (front) 
                menusFront.add(btn);
            else 
                menusBack.add(btn);

            return btn;
        }

        return nullptr;
    }

    Button* VerticalToolMenu::appand(bool front, int id, const String& strLabel, Image norImage, Image ovrImage, Image dwnImage)
    {
        DrawableImage drawNor, drawOvr, drawDwn;
        drawNor.setImage(norImage);
        drawOvr.setImage(ovrImage);
        drawDwn.setImage(dwnImage);

        ToolButton* btn = new ToolButton(String::empty, 18, &drawNor, &drawOvr, &drawDwn);
        btn->setButtonText(strLabel);
        btn->setColour(ToolButton::ColourIds::textColourId, Colour(0xFF979797));
        btn->setColour(ToolButton::ColourIds::textColourOnId, Colours::white);
        btn->addListener(this);
        addChildAndSetID(btn, String(id));

        if (front) 
            menusFront.add(btn);
        else 
            menusBack.add(btn);

        return btn;
    }


    Component* VerticalToolMenu::getItemByID(int id)
    {
        String ID(id);
        for (int i=0; i<menusFront.size(); ++i)
        {
            if (menusFront.getUnchecked(i) && menusFront.getUnchecked(i)->getComponentID() == ID)
            {
                return menusFront.getUnchecked(i);
            }
        }
        for (int i=0; i<menusBack.size(); ++i)
        {
            if (menusBack.getUnchecked(i) && menusBack.getUnchecked(i)->getComponentID() == ID)
            {
                return menusBack.getUnchecked(i);
            }
        }
        return nullptr;
    }

    void VerticalToolMenu::setItemInset(int nHeight)
    {
        itemInset = nHeight;
        resized();
    }

    void VerticalToolMenu::showItemEdge(const juce::String& edge)
    {
        if (itemEdge != edge)
        {
            if (juce::String::empty != (itemEdge = edge))
            {
                if (!itemEdgeComp)
                {
                    addAndMakeVisible(itemEdgeComp = new ImageComponent);
                    itemEdgeComp->setImage(CXIMAGE_CACHE_LOAD(L"read_info_edge.png"));
                    itemEdgeComp->setSize(10, 15);
                }
            }
            else
            {
                itemEdgeComp = nullptr;
            }
            resized();
        }
    }

    void VerticalToolMenu::paint(Graphics& g)
    {
        g.fillAll(findColour (VerticalToolMenu::backgroundId));
    }

    void VerticalToolMenu::resized()
    {
        Rectangle<int> bounds = getLocalBounds();
        bounds.removeFromTop(20);
        bounds.removeFromBottom(20);

        int height = bounds.getY();

        for (int i=0; i<menusFront.size(); ++i)
        {
            if (menusFront[i] && menusFront[i]->isVisible())
            {
                if (const ToolButton* b = dynamic_cast<ToolButton*>(menusFront[i]))
                {
                    const int ih = b->getButtonHeight();
                    menusFront[i]->setBounds(0, height, bounds.getWidth(), ih);
                    height += itemInset + ih;
                }
                else
                {
                    const Rectangle<int> bds = menusFront[i]->getBounds();
                    const int ih = bds.getHeight();
                    menusFront[i]->setBounds(bds.getX(), height, bds.getWidth(), ih);
                    height += itemInset + ih;
                }
            }
            else
            {
                height += itemInset;
            }
        }

        height = bounds.getBottom();

        for (int i=0; i<menusBack.size(); ++i)
        {
            if (menusBack[i] && menusBack[i]->isVisible())
            {
                if (const ToolButton* b = dynamic_cast<ToolButton*>(menusBack[i]))
                {
                    const int ih = b->getButtonHeight();
                    height -= itemInset + ih;
                    menusBack[i]->setBounds(0, height, bounds.getWidth(), ih);
                }
                else
                {
                    const Rectangle<int> bds = menusBack[i]->getBounds();
                    const int ih = bds.getHeight();
                    height -= itemInset + ih;
                    menusBack[i]->setBounds(bds.getX(), height, bds.getWidth(), ih);
                }
            }
            else
            {
                height -= itemInset;
            }
        }

        if (itemEdge != juce::String::empty && itemEdgeComp)
        {
            for (int i=0; i<menusFront.size(); ++i)
            {
                if (menusFront.getUnchecked(i)->getComponentID() == itemEdge)
                {
                    const juce::Rectangle<int> area = menusFront.getUnchecked(i)->getBoundsInParent();
                    itemEdgeComp->setTopRightPosition(area.getRight(), area.getCentreY() - (itemEdgeComp->getHeight()/2));
                    return;
                }
            }
            for (int i=0; i<menusBack.size(); ++i)
            {
                if (menusBack.getUnchecked(i)->getComponentID() == itemEdge)
                {
                    const juce::Rectangle<int> area = menusBack.getUnchecked(i)->getBoundsInParent();
                    itemEdgeComp->setTopRightPosition(area.getRight(), area.getCentreY() - (itemEdgeComp->getHeight()/2));
                    return;
                }
            }
        }
    }

    void VerticalToolMenu::buttonClicked(Button* b)
    {
        if (eventListener)
        {
            eventListener->onVToolMenuItemClicked(this, b);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //

    PacketListBox::PacketListBox(PacketListBox::Listener* l)
        : currentSelectIndex(0)
        , titleHeight(30)
        , listener(l)
    {
        addAndMakeVisible(listboxPanel = new ListBox);
        listboxPanel->setRowHeight(50);
        listboxPanel->getViewport()->setScrollBarsShown(true, false, true, true);
        listboxPanel->setModel(this);
        listboxPanel->getViewport()->setScrollBarsShown(true, false, true, true);
        listboxPanel->getViewport()->setScrollBarThickness(10);
        listboxPanel->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    }

    PacketListBox::~PacketListBox()
    {

    }

    void PacketListBox::resized()
    {
        updateLayout();
    }

    void PacketListBox::updateLayout(unsigned int first /*= 0*/, unsigned int last /*= -1*/)
    {
        const juce::Rectangle<int> area = getBounds();
        const int w = area.getWidth();
        const int h = area.getHeight();
        const int count = titleButtons.size();

        for (int i=first; i<count && i<last; ++i)
        {
            if (i < currentSelectIndex)
            {
                
                titleButtons[i]->setBounds(juce::Rectangle<int>(0, i*titleHeight, w, titleHeight));
            }
            else if (i == currentSelectIndex)
            {
                titleButtons[i]->setBounds(juce::Rectangle<int>(0, i*titleHeight, w, titleHeight));
                listboxPanel->setBounds(juce::Rectangle<int>(0, (i+1)*titleHeight, w, h - count * titleHeight));
                listboxPanel->updateContent();
                listboxPanel->repaint();
            }
            else
            {
                titleButtons[i]->setBounds(juce::Rectangle<int>(0, (i*titleHeight)+(h - count * titleHeight), w, titleHeight));
            }
        }
    }

    void PacketListBox::setTitleHeight(int h)
    {
        titleHeight = h;
        resized();
    }

    void PacketListBox::addPanel(const juce::String& title)
    {
        ColorButton* btn = new ColorButton;
        btn->setButtonText(title);
        btn->setColour(juce::ColorButton::ColourIds::buttonBackgroundId, juce::Colours::transparentBlack);
        btn->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightBlue);
        btn->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.2f));
        btn->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.4f));
        btn->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::white);
        btn->setColour(juce::ColorButton::ColourIds::textColourHoverId, juce::Colours::white);
        btn->setColour(juce::ColorButton::ColourIds::textColourDownId, juce::Colours::white);
        btn->setButtonBorder(0);
        btn->setBounds(10,0,30,80);
        btn->addListener(this);
        addAndMakeVisible(btn);

        titleButtons.add(btn);
        updateLayout();
    }

    void PacketListBox::removeAllPanel()
    {
        titleButtons.removeRange(0, titleButtons.size());
        listboxPanel->updateContent();
    }

    void PacketListBox::buttonClicked(Button* b)
    {
        const int idx = getTitleButtonIndex(b);
        currentSelectIndex = idx;
        resized();

        if (idx != -1)
            listener->expandSelectChanged();
    }

    int PacketListBox::getTitleButtonIndex(Button* b)
    {
        for (int i=0; i<titleButtons.size(); ++i)
        {
            if (titleButtons[i] == b)
                return i;
        }
        return -1;
    }

    int PacketListBox::getTitleHeight() const
    {
        return titleHeight;
    }

    ColorButton* PacketListBox::getTitleButton(int idx)
    {
        if (idx > 0 && idx < titleButtons.size())
            return titleButtons[idx];

        return nullptr;
    }

    ListBox* PacketListBox::getPanelItem(int idx)
    {
        return listboxPanel;
    }

    int PacketListBox::getNumRows()
    {
        if (listener && currentSelectIndex >=0 && currentSelectIndex <titleButtons.size())
            return listener->getNumRows();

        return 0;
    }

    void PacketListBox::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
    {
        if (listener && currentSelectIndex >=0 && currentSelectIndex <titleButtons.size())
            listener->paintListBoxItem(rowNumber, g, width, height, rowIsSelected);
    }

    int PacketListBox::getSelected() const
    {
        return currentSelectIndex;
    }


}


