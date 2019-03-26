#include "stdafx.h"

namespace juce
{

CXDialogWindow::CXDialogWindow (const String& name, Colour colour,
                            const bool escapeCloses, const bool onDesktop)
    : CXDocumentWindow (name, colour, CXDocumentWindow::closeButton, onDesktop),
      escapeKeyTriggersCloseButton (escapeCloses)
{
}

CXDialogWindow::~CXDialogWindow()
{
}

bool CXDialogWindow::keyPressed (const KeyPress& key)
{
    if (escapeKeyTriggersCloseButton && key == KeyPress::escapeKey)
    {
        setVisible (false);
        return true;
    }

    return CXDocumentWindow::keyPressed (key);
}

void CXDialogWindow::resized()
{
    CXDocumentWindow::resized();

    if (escapeKeyTriggersCloseButton)
    {
        if (Button* const close = getCloseButton())
        {
            const KeyPress esc (KeyPress::escapeKey, 0, 0);

            if (! close->isRegisteredForShortcut (esc))
                close->addShortcut (esc);
        }
    }
}

//==============================================================================
class DefaultDialogWindow   : public CXDialogWindow
{
public:
    DefaultDialogWindow (LaunchOptions& options)
        : CXDialogWindow (options.dialogTitle, options.dialogBackgroundColour,
                        options.escapeKeyTriggersCloseButton, true)
    {
        setUsingNativeTitleBar (options.useNativeTitleBar);
        setAlwaysOnTop (juce_areThereAnyAlwaysOnTopWindows());

        if (options.content.willDeleteObject())
            setContentOwned (options.content.release(), true);
        else
            setContentNonOwned (options.content.release(), true);

        centreAroundComponent (options.componentToCentreAround, getWidth(), getHeight());
        setResizable (options.resizable, options.useBottomRightCornerResizer);
    }

    void closeButtonPressed() override
    {
        setVisible (false);
    }

private:
    JUCE_DECLARE_NON_COPYABLE (DefaultDialogWindow)
};

CXDialogWindow::LaunchOptions::LaunchOptions() noexcept
    : dialogBackgroundColour (Configuration::colorLightBlue),
      componentToCentreAround (nullptr),
      escapeKeyTriggersCloseButton (true),
      useNativeTitleBar (true),
      resizable (true),
      useBottomRightCornerResizer (false)
{
}

CXDialogWindow* CXDialogWindow::LaunchOptions::create()
{
    jassert (content != nullptr); // You need to provide some kind of content for the dialog!

    return new DefaultDialogWindow (*this);
}

CXDialogWindow* CXDialogWindow::LaunchOptions::launchAsync()
{
    CXDialogWindow* const d = create();
    d->enterModalState (true, nullptr, true);
    return d;
}

int CXDialogWindow::LaunchOptions::runModal()
{
    return launchAsync()->runModalLoop();
}

void CXDialogWindow::showDialog (const String& dialogTitle,
                               Component* const contentComponent,
                               Component* const componentToCentreAround,
                               Colour backgroundColour,
                               const bool escapeKeyTriggersCloseButton,
                               const bool resizable,
                               const bool useBottomRightCornerResizer)
{
    LaunchOptions o;
    o.dialogTitle = dialogTitle;
    o.content.setNonOwned (contentComponent);
    o.componentToCentreAround = componentToCentreAround;
    o.dialogBackgroundColour = backgroundColour;
    o.escapeKeyTriggersCloseButton = escapeKeyTriggersCloseButton;
    o.useNativeTitleBar = false;
    o.resizable = resizable;
    o.useBottomRightCornerResizer = useBottomRightCornerResizer;

    o.launchAsync();
}

int CXDialogWindow::showModalDialog (const String& dialogTitle,
                                   Component* const contentComponent,
                                   Component* const componentToCentreAround,
                                   Colour backgroundColour,
                                   const bool escapeKeyTriggersCloseButton,
                                   const bool resizable,
                                   const bool useBottomRightCornerResizer)
{
    LaunchOptions o;
    o.dialogTitle = dialogTitle;
    o.content.setNonOwned (contentComponent);
    o.componentToCentreAround = componentToCentreAround;
    o.dialogBackgroundColour = backgroundColour;
    o.escapeKeyTriggersCloseButton = escapeKeyTriggersCloseButton;
    o.useNativeTitleBar = false;
    o.resizable = resizable;
    o.useBottomRightCornerResizer = useBottomRightCornerResizer;

    return o.runModal();
}

}