#pragma once

namespace juce{

class CXDocumentWindow   : public ResizableWindow
{
public:
    enum TitleBarButtons
    {
        minimiseButton = 1,
        maximiseButton = 2,
        closeButton = 4,

        /** A combination of all the buttons above. */
        allButtons = 7
    };

    CXDocumentWindow (const String& name,
                    Colour backgroundColour,
                    int requiredButtons,
                    bool addToDesktop = true);

    ~CXDocumentWindow();

    void setName (const String& newName);
    void setIcon (const Image& imageToUse);
    void setTitleBarHeight (int newHeight);
    int getTitleBarHeight() const;
    void setTitleBarButtonsRequired (int requiredButtons,
                                     bool positionTitleBarButtonsOnLeft);
    void setTitleBarTextCentred (bool textShouldBeCentred);
    void setMenuBar (MenuBarModel* menuBarModel,
                     int menuBarHeight = 0);
    Component* getMenuBarComponent() const noexcept;
    void setMenuBarComponent (Component* newMenuBarComponent);

    virtual void closeButtonPressed();
    virtual void minimiseButtonPressed();
    virtual void maximiseButtonPressed();

    Button* getCloseButton() const noexcept;
    Button* getMinimiseButton() const noexcept;
    Button* getMaximiseButton() const noexcept;

    enum ColourIds
    {
        textColourId                = 0x1005701,  /**< The colour to draw any text with. It's up to the look
                                                       and feel class how this is used. */
    };

    struct  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() {}

        virtual void drawCXDocumentWindowTitleBar (CXDocumentWindow&,
                                                 Graphics&, int w, int h,
                                                 int titleSpaceX, int titleSpaceW,
                                                 const Image* icon,
                                                 bool drawTitleTextOnLeft) = 0;

        virtual Button* createCXDocumentWindowButton (int buttonType) = 0;

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
    int titleBarHeight, menuBarHeight, requiredButtons;
    bool positionTitleBarButtonsOnLeft, drawTitleTextCentred;
    ScopedPointer <Button> titleBarButtons [3];
    Image titleBarIcon;
    ScopedPointer <Component> menuBar;
    MenuBarModel* menuBarModel;

    class ButtonListenerProxy;
    friend struct ContainerDeletePolicy<ButtonListenerProxy>;
    ScopedPointer<ButtonListenerProxy> buttonListener;

    void repaintTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXDocumentWindow)
};

}
