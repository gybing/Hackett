#pragma once

class ColorButton : public Button
{
public:
    ColorButton ();
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override;

    enum ColourIds
    {
        buttonColourNormalId            = 0x1110000,
        buttonColourHoverId             = 0x1110001,
        buttonColourDownId              = 0x1110002,
        textColourNormalId              = 0x1110003,
        textColourHoverId               = 0x1110004,
        textColourDownId                = 0x1110005,
        buttonBackgroundId              = 0x1110006
    };

    // 设置线框大小 0代表全色填充
    void setButtonBorder(int lineThickness, float corner = 0.0f);
    //==============================================================================
    void changeWidthToFitText();
    void changeWidthToFitText (int newHeight);
    int getBestWidthForHeight (int buttonHeight);
private:
    int drawButtonThickness;
    float drawCornerSize;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColorButton)
};

class ToolButton : public Button
{
public:
    ToolButton (const String& name, int height, const Drawable* norImage,
        const Drawable* ovrImage = nullptr, const Drawable* dwnImage = nullptr
        , bool imageAboveTextLabel = true);

    ToolButton (const String& name, bool imageAboveTextLabel = true);

    ~ToolButton();

    void setImages (const Drawable* norImage, const Drawable* ovrImage = nullptr, const Drawable* dwnImage = nullptr);
    Drawable* getCurrentImage() const noexcept;
    Drawable* getNormalImage() const noexcept;
    Drawable* getOverImage() const noexcept;
    Drawable* getDownImage() const noexcept;
    int getButtonHeight() const;
    void setButtonHeight(int h);
    void setButtonTextPos(int x);
    virtual Rectangle<float> getImageBounds() const;

    enum ColourIds
    {
        textColourId             = 0x1004010,  /**< The colour to use for the button's text label. */
        textColourOnId           = 0x1004013,  /**< The colour to use for the button's text.when the button's toggle state is "on". */
        backgroundColourId       = 0x1004011,  
        backgroundOnColourId     = 0x1004012
    };

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override;
    void buttonStateChanged() override;
    void resized() override;
    void enablementChanged() override;
    void colourChanged() override;

private:
    Drawable* currentImage;
    int textHeight, textPosX;
    ScopedPointer<Drawable> normalImage, overImage, downImage;
    bool imageAboveTextLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolButton)
};

class VerticalToolMenu : public Component, public Button::Listener
{
public:
    ~VerticalToolMenu();

    void appand(bool front);
    Button* appand(bool front, int id, Button* btn);
    Button* appand(bool front, int id, const String& strLabel, Image norImage, Image ovrImage, Image dwnImage);
    Component* getItemByID(int id);

    void setItemInset(int nHeight);
    void showItemEdge(const juce::String& edge = juce::String::empty);

    class VToolListener
    {
    public:
        virtual void onVToolMenuItemClicked(VerticalToolMenu*, Button*) = 0;
    };

    VerticalToolMenu(VerticalToolMenu::VToolListener* l = nullptr);

    enum ColourIds
    {
        backgroundId              = 0x1110015
    };

    virtual void paint(Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (Button*) override;

private:
    VToolListener* eventListener;
    int itemInset;
    juce::String itemEdge;
    ScopedPointer<ImageComponent> itemEdgeComp;
    OwnedArray<Component> menusFront;
    OwnedArray<Component> menusBack;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalToolMenu)
};

class ListBoxMouseHoverSelector : public MouseListener
{
public:
    class Listener { public: 
    virtual void listBoxRowHover() = 0;
    };

    ListBoxMouseHoverSelector (ListBox& lb, ListBoxMouseHoverSelector::Listener* l = nullptr) 
        : listbox (lb)
        , rowNumber(-1)
        , listener(l)
    {
        lb.addMouseListener (this, true);
    }

    void mouseExit (const MouseEvent& e) override { mouseMove (e); }
    void mouseMove (const MouseEvent& e) override
    {
        const MouseEvent e2 (e.getEventRelativeTo (&listbox));

        const int row = (e2.x < listbox.getVisibleContentWidth()) ? listbox.getRowContainingPosition (e2.x, e2.y) : -1;

        if (lastRowNumber != rowNumber ||
            rowNumber != row)
        {
            lastRowNumber = rowNumber;
            rowNumber = row;

            if (listener) listener->listBoxRowHover();
        }
    }

    int rowNumber;
    int lastRowNumber;

private:
    ListBox& listbox;
    Listener* listener;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListBoxMouseHoverSelector)
};

class PacketListBox 
    : public Component
    , public Button::Listener
    , public juce::ListBoxModel
{
public:
    ~PacketListBox();

    void addPanel(const juce::String& title);
    void removeAllPanel();

    ColorButton* getTitleButton(int idx);
    ListBox* getPanelItem(int idx);

    void setTitleHeight(int h);
    int getTitleHeight() const;

    int getSelected() const;

    class Listener
    {
    public:
        virtual int getNumRows() = 0;
        virtual void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) = 0;
        virtual void expandSelectChanged() = 0;
    };

    PacketListBox(PacketListBox::Listener* l);

protected:
    virtual void resized() override;
    virtual void buttonClicked (Button*) override;
    virtual int getNumRows() override;
    virtual void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

    int getTitleButtonIndex(Button*);
    void updateLayout(unsigned int first = 0, unsigned int last = -1);

private:

    int titleHeight, currentSelectIndex;
    PacketListBox::Listener* listener;
    OwnedArray<juce::ColorButton> titleButtons;
    juce::ScopedPointer<juce::ListBox> listboxPanel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PacketListBox)
};
