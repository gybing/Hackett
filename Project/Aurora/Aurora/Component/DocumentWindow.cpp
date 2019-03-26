#include "stdafx.h"

namespace juce
{

class CXDocumentWindow::ButtonListenerProxy  : public ButtonListener // (can't use Button::Listener due to idiotic VC2005 bug)
{
public:
    ButtonListenerProxy (CXDocumentWindow& w) : owner (w) {}

    void buttonClicked (Button* button) override
    {
        if      (button == owner.getMinimiseButton())  owner.minimiseButtonPressed();
        else if (button == owner.getMaximiseButton())  owner.maximiseButtonPressed();
        else if (button == owner.getCloseButton())     owner.closeButtonPressed();
    }

private:
    CXDocumentWindow& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonListenerProxy)
};

//==============================================================================
CXDocumentWindow::CXDocumentWindow (const String& title,
                                Colour backgroundColour,
                                int requiredButtons_,
                                bool addToDesktop_)
    : ResizableWindow (title, backgroundColour, addToDesktop_),
      titleBarHeight (26),
      menuBarHeight (24),
      requiredButtons (requiredButtons_),
     #if JUCE_MAC
      positionTitleBarButtonsOnLeft (true),
     #else
      positionTitleBarButtonsOnLeft (false),
     #endif
      drawTitleTextCentred (true),
      menuBarModel (nullptr)
{
    setResizeLimits (128, 128, 32768, 32768);

    CXDocumentWindow::lookAndFeelChanged();
}

CXDocumentWindow::~CXDocumentWindow()
{
    // Don't delete or remove the resizer components yourself! They're managed by the
    // CXDocumentWindow, and you should leave them alone! You may have deleted them
    // accidentally by careless use of deleteAllChildren()..?
    jassert (menuBar == nullptr || getIndexOfChildComponent (menuBar) >= 0);
    jassert (titleBarButtons[0] == nullptr || getIndexOfChildComponent (titleBarButtons[0]) >= 0);
    jassert (titleBarButtons[1] == nullptr || getIndexOfChildComponent (titleBarButtons[1]) >= 0);
    jassert (titleBarButtons[2] == nullptr || getIndexOfChildComponent (titleBarButtons[2]) >= 0);

    for (int i = numElementsInArray (titleBarButtons); --i >= 0;)
        titleBarButtons[i] = nullptr;

    menuBar = nullptr;
}

//==============================================================================
void CXDocumentWindow::repaintTitleBar()
{
    repaint (getTitleBarArea());
}

void CXDocumentWindow::setName (const String& newName)
{
    if (newName != getName())
    {
        Component::setName (newName);
        repaintTitleBar();
    }
}

void CXDocumentWindow::setIcon (const Image& imageToUse)
{
    titleBarIcon = imageToUse;
    repaintTitleBar();
}

void CXDocumentWindow::setTitleBarHeight (const int newHeight)
{
    titleBarHeight = newHeight;
    resized();
    repaintTitleBar();
}

void CXDocumentWindow::setTitleBarButtonsRequired (const int buttons, const bool onLeft)
{
    requiredButtons = buttons;
    positionTitleBarButtonsOnLeft = onLeft;
    lookAndFeelChanged();
}

void CXDocumentWindow::setTitleBarTextCentred (const bool textShouldBeCentred)
{
    drawTitleTextCentred = textShouldBeCentred;
    repaintTitleBar();
}

//==============================================================================
void CXDocumentWindow::setMenuBar (MenuBarModel* newMenuBarModel, const int newMenuBarHeight)
{
    if (menuBarModel != newMenuBarModel)
    {
        menuBar = nullptr;

        menuBarModel = newMenuBarModel;
        menuBarHeight = newMenuBarHeight > 0 ? newMenuBarHeight
                                             : getLookAndFeel().getDefaultMenuBarHeight();

        if (menuBarModel != nullptr)
            setMenuBarComponent (new MenuBarComponent (menuBarModel));

        resized();
    }
}

Component* CXDocumentWindow::getMenuBarComponent() const noexcept
{
    return menuBar;
}

void CXDocumentWindow::setMenuBarComponent (Component* newMenuBarComponent)
{
    // (call the Component method directly to avoid the assertion in ResizableWindow)
    Component::addAndMakeVisible (menuBar = newMenuBarComponent);

    if (menuBar != nullptr)
        menuBar->setEnabled (isActiveWindow());

    resized();
}

//==============================================================================
void CXDocumentWindow::closeButtonPressed()
{
    /*  If you've got a close button, you have to override this method to get
        rid of your window!

        If the window is just a pop-up, you should override this method and make
        it delete the window in whatever way is appropriate for your app. E.g. you
        might just want to call "delete this".

        If your app is centred around this window such that the whole app should quit when
        the window is closed, then you will probably want to use this method as an opportunity
        to call JUCEApplicationBase::quit(), and leave the window to be deleted later by your
        JUCEApplicationBase::shutdown() method. (Doing it this way means that your window will
        still get cleaned-up if the app is quit by some other means (e.g. a cmd-Q on the mac
        or closing it via the taskbar icon on Windows).
    */
    jassertfalse;
}

void CXDocumentWindow::minimiseButtonPressed()
{
    setMinimised (true);
}

void CXDocumentWindow::maximiseButtonPressed()
{
    setFullScreen (! isFullScreen());
}

//==============================================================================
void CXDocumentWindow::paint (Graphics& g)
{
    ResizableWindow::paint (g);

    if (resizableBorder == nullptr)
    {
        RectangleList<int> border (getLocalBounds());
        border.subtract (getBorderThickness().subtractedFrom (getLocalBounds()));

        g.setColour (getBackgroundColour().overlaidWith (Colour (0x80000000)));
        g.fillRectList (border);
    }

    const juce::Rectangle<int> titleBarArea (getTitleBarArea());
    g.reduceClipRegion (titleBarArea);
    g.setOrigin (titleBarArea.getPosition());

    int titleSpaceX1 = 6;
    int titleSpaceX2 = titleBarArea.getWidth() - 6;

    for (int i = 0; i < 3; ++i)
    {
        if (Button* const b = titleBarButtons[i])
        {
            if (positionTitleBarButtonsOnLeft)
                titleSpaceX1 = jmax (titleSpaceX1, b->getRight() + (getWidth() - b->getRight()) / 8);
            else
                titleSpaceX2 = jmin (titleSpaceX2, b->getX() - (b->getX() / 8));
        }
    }

    ((LAFBase*)&getLookAndFeel())->drawCXDocumentWindowTitleBar (*this, g,
                                                 titleBarArea.getWidth(),
                                                 titleBarArea.getHeight(),
                                                 titleSpaceX1,
                                                 jmax (1, titleSpaceX2 - titleSpaceX1),
                                                 titleBarIcon.isValid() ? &titleBarIcon : 0,
                                                 ! drawTitleTextCentred);
}

void CXDocumentWindow::resized()
{
    ResizableWindow::resized();

    if (Button* const b = getMaximiseButton())
        b->setToggleState (isFullScreen(), dontSendNotification);

    const juce::Rectangle<int> titleBarArea (getTitleBarArea());

    ((LAFBase*)&getLookAndFeel())
        ->positionCXDocumentWindowButtons (*this,
                                        titleBarArea.getX(), titleBarArea.getY(),
                                        titleBarArea.getWidth(), titleBarArea.getHeight(),
                                        titleBarButtons[0],
                                        titleBarButtons[1],
                                        titleBarButtons[2],
                                        positionTitleBarButtonsOnLeft);

    if (menuBar != nullptr)
        menuBar->setBounds (titleBarArea.getX(), titleBarArea.getBottom(),
                            titleBarArea.getWidth(), menuBarHeight);
}

BorderSize<int> CXDocumentWindow::getBorderThickness()
{
    return ResizableWindow::getBorderThickness();
}

BorderSize<int> CXDocumentWindow::getContentComponentBorder()
{
    BorderSize<int> border (getBorderThickness());

    if (! isKioskMode())
        border.setTop (border.getTop()
                        + (isUsingNativeTitleBar() ? 0 : titleBarHeight)
                        + (menuBar != nullptr ? menuBarHeight : 0));

    return border;
}

int CXDocumentWindow::getTitleBarHeight() const
{
    return isUsingNativeTitleBar() ? 0 : jmin (titleBarHeight, getHeight() - 4);
}

Rectangle<int> CXDocumentWindow::getTitleBarArea()
{
    const BorderSize<int> border (getBorderThickness());

    if (isKioskMode())
        return Rectangle<int>();

    return Rectangle<int> (border.getLeft(), border.getTop(),
                           getWidth() - border.getLeftAndRight(), getTitleBarHeight());
}

Button* CXDocumentWindow::getCloseButton()    const noexcept  { return titleBarButtons[2]; }
Button* CXDocumentWindow::getMinimiseButton() const noexcept  { return titleBarButtons[0]; }
Button* CXDocumentWindow::getMaximiseButton() const noexcept  { return titleBarButtons[1]; }

int CXDocumentWindow::getDesktopWindowStyleFlags() const
{
    int styleFlags = ResizableWindow::getDesktopWindowStyleFlags();

    if ((requiredButtons & minimiseButton) != 0)  styleFlags |= ComponentPeer::windowHasMinimiseButton;
    if ((requiredButtons & maximiseButton) != 0)  styleFlags |= ComponentPeer::windowHasMaximiseButton;
    if ((requiredButtons & closeButton)    != 0)  styleFlags |= ComponentPeer::windowHasCloseButton;

    return styleFlags;
}

void CXDocumentWindow::lookAndFeelChanged()
{
    for (int i = numElementsInArray (titleBarButtons); --i >= 0;)
        titleBarButtons[i] = nullptr;

    if (! isUsingNativeTitleBar())
    {
        LookAndFeel& lf = getLookAndFeel();

        if ((requiredButtons & minimiseButton) != 0)  titleBarButtons[0] = lf.createDocumentWindowButton (minimiseButton);
        if ((requiredButtons & maximiseButton) != 0)  titleBarButtons[1] = lf.createDocumentWindowButton (maximiseButton);
        if ((requiredButtons & closeButton)    != 0)  titleBarButtons[2] = lf.createDocumentWindowButton (closeButton);

        for (int i = 0; i < 3; ++i)
        {
            if (Button* const b = titleBarButtons[i])
            {
                if (buttonListener == nullptr)
                    buttonListener = new ButtonListenerProxy (*this);

                b->addListener (buttonListener);
                b->setWantsKeyboardFocus (false);

                // (call the Component method directly to avoid the assertion in ResizableWindow)
                Component::addAndMakeVisible (b);
            }
        }

        if (Button* const b = getCloseButton())
        {
           #if JUCE_MAC
            b->addShortcut (KeyPress ('w', ModifierKeys::commandModifier, 0));
           #else
            b->addShortcut (KeyPress (KeyPress::F4Key, ModifierKeys::altModifier, 0));
           #endif
        }
    }

    activeWindowStatusChanged();

    ResizableWindow::lookAndFeelChanged();
}

void CXDocumentWindow::parentHierarchyChanged()
{
    lookAndFeelChanged();
}

void CXDocumentWindow::activeWindowStatusChanged()
{
    ResizableWindow::activeWindowStatusChanged();

    for (int i = numElementsInArray (titleBarButtons); --i >= 0;)
        if (Button* const b = titleBarButtons[i])
            b->setEnabled (isActiveWindow());

    if (menuBar != nullptr)
        menuBar->setEnabled (isActiveWindow());
}

void CXDocumentWindow::mouseDoubleClick (const MouseEvent& e)
{
    Button* const maximise = getMaximiseButton();

    if (maximise != nullptr && getTitleBarArea().contains (e.x, e.y))
        maximise->triggerClick();
}

void CXDocumentWindow::userTriedToCloseWindow()
{
    closeButtonPressed();
}

}