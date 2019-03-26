#pragma once

namespace juce {

class CXDocumentWindow   : public juce::ResizableWindow
{
public:
    enum TitleBarButtons
    {
        minimiseButton = 1,
        maximiseButton = 2,
        closeButton = 4,
        allButtons = 7
    };

    CXDocumentWindow (const juce::String& name,
                    juce::Colour backgroundColour,
                    int requiredButtons,
                    bool addToDesktop = true,
                    juce::BorderSize<int> size=juce::BorderSize<int>(-1));

    ~CXDocumentWindow();

    void setTitle (const juce::String& newName, juce::Justification justification = juce::Justification::centred);
    const juce::String& getTitle() const;
    void setIcon (const juce::Image& imageToUse);
    void setTitleBarHeight (int newHeight);
    int getTitleBarHeight() const;
    void setTitleBarButtonsRequired (int requiredButtons, bool positionTitleBarButtonsOnLeft);
    void setTitleBarTextCentred (bool textShouldBeCentred);
    Component* getMenuBarComponent() const noexcept;
    void setMenuBarComponent (Component* newMenuBarComponent);
    void setBorderThickness(juce::BorderSize<int> border);

    virtual void closeButtonPressed();
    virtual void minimiseButtonPressed();
    virtual void maximiseButtonPressed();

    juce::Button* getCloseButton() const noexcept;
    juce::Button* getMinimiseButton() const noexcept;
    juce::Button* getMaximiseButton() const noexcept;

    enum ColourIds
    {
        textColourId                = 0x1005701,  /**< The colour to draw any text with. It's up to the look
                                                       and feel class how this is used. */
    };

    struct  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() {}
        virtual void drawCXDocumentWindowTitleBar (CXDocumentWindow&,
                                                 juce::Graphics&, int w, int h,
                                                 int titleSpaceX, int titleSpaceW,
                                                 const Image* icon,
                                                 bool drawTitleTextOnLeft,
                                                 juce::Justification) = 0;
        virtual juce::Button* createCXDocumentWindowButton (int buttonType) = 0;
        virtual void positionCXDocumentWindowButtons (CXDocumentWindow&,
                                                    int titleBarX, int titleBarY, int titleBarW, int titleBarH,
                                                    Button* minimiseButton,
                                                    Button* maximiseButton,
                                                    Button* closeButton,
                                                    bool positionTitleBarButtonsOnLeft) = 0;
    };

    //==============================================================================
   #ifndef DOXYGEN
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void lookAndFeelChanged() override;
    /** @internal */
    BorderSize<int> getBorderThickness() override;
    /** @internal */
    BorderSize<int> getContentComponentBorder() override;
    /** @internal */
    void mouseDoubleClick (const MouseEvent&) override;
    /** @internal */
    void userTriedToCloseWindow() override;
    /** @internal */
    void activeWindowStatusChanged() override;
    /** @internal */
    int getDesktopWindowStyleFlags() const override;
    /** @internal */
    void parentHierarchyChanged() override;
    /** @internal */
    juce::Rectangle<int> getTitleBarArea();
   #endif

private:
    //==============================================================================
    int titleBarHeight, requiredButtons;
    bool positionTitleBarButtonsOnLeft, drawTitleTextCentred;
    ScopedPointer <Button> titleBarButtons [3];
    Image titleBarIcon;
    ScopedPointer <Component> menuBar;
    BorderSize<int> borderSize;
    class ButtonListenerProxy;
    friend struct ContainerDeletePolicy<ButtonListenerProxy>;
    ScopedPointer<ButtonListenerProxy> buttonListener;
    juce::Justification titleJustification;
    String title;
    void repaintTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXDocumentWindow)
};

}