#include "JuceHeader.h"
#pragma comment(lib, "User32.lib")

namespace juce {

    CXDialogWindow::CXDialogWindow (const String& name, bool escapeCloses, bool addToDesktop)
        : CXDocumentWindow (name, ProjectInfo::colorBlueBlack, CXDocumentWindow::closeButton, addToDesktop, juce::BorderSize<int>(4,4,4,4)),
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
        : CXDialogWindow (juce::String::empty, options.escapeKeyTriggersCloseButton, true)
    {
        setUsingNativeTitleBar (false);
        setTitleBarHeight(31);

        setTitle(options.dialogTitle, options.titleLayout);

        if (options.content.willDeleteObject())
            setContentOwned (options.content.release(), true);
        else
            setContentNonOwned (options.content.release(), true);

        centreAroundComponent (options.componentToCentreAround, getWidth(), getHeight());
        setResizable (options.resiedEnable, false);
    }

    void closeButtonPressed() override
    {
        setVisible (false);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DefaultDialogWindow)
};

CXDialogWindow::LaunchOptions::LaunchOptions() noexcept
    : componentToCentreAround (nullptr),
      escapeKeyTriggersCloseButton (true),
      titleLayout(Justification::centred),
      callback(nullptr)
{
    
}

CXDialogWindow* CXDialogWindow::LaunchOptions::create()
{
    jassert (content != nullptr);
    return new DefaultDialogWindow (*this);
}

CXDialogWindow* CXDialogWindow::LaunchOptions::launchAsync()
{
    CXDialogWindow* const d = create();
    d->enterModalState (true, callback, true);
    return d;
}

int CXDialogWindow::LaunchOptions::runModal()
{
    return launchAsync()->runModalLoop();
}

void CXDialogWindow::showDialog (const String& dialogTitle,
                                 Component* contentComponent,
                                 Component* componentToCentreAround,
                                 bool bModal,
                                 juce::Justification layout,
                                 bool bResized)
{
    LaunchOptions o;
    o.dialogTitle = dialogTitle;
    o.content.setOwned (contentComponent);
    o.componentToCentreAround = componentToCentreAround;
    o.escapeKeyTriggersCloseButton = true;
    o.titleLayout = layout;
    o.resiedEnable = bResized;

    if (o.componentToCentreAround)
    {
        class ComponentMask : public juce::Component
        {
        private:
            virtual void paint(juce::Graphics& g) override
            {
                g.fillAll(juce::Colour(0x3F000000));
            }
            JUCE_LEAK_DETECTOR (ComponentMask)
        };

        class ComponentMaskCallBack : public ModalComponentManager::Callback
        {
        public:
            ComponentMaskCallBack(ComponentMask* mask) : maskComponent(mask) {}
        private:
            juce::ScopedPointer<ComponentMask> maskComponent;
            virtual void modalStateFinished (int returnValue) override
            {
                juce::Component* parentComp = maskComponent->getParentComponent();
                if (maskComponent && parentComp)
                {
                    ComponentAnimator& animate = Desktop::getInstance().getAnimator();
                    animate.fadeOut(maskComponent, 100);
                    parentComp->removeChildComponent(maskComponent);
                    CXDocumentWindow* parent = parentComp->findParentComponentOfClass<CXDocumentWindow>();
                    if (parent) parent->setEnabled(true);
                }
            }
            JUCE_LEAK_DETECTOR (ComponentMaskCallBack)
        };

        CXDocumentWindow* parent;
        if (!(parent = dynamic_cast<CXDocumentWindow*>(o.componentToCentreAround)))
            parent = o.componentToCentreAround->findParentComponentOfClass<CXDocumentWindow>();

        if (parent)
        {
            parent->setEnabled(false);
            Component* comp = parent->getContentComponent();
            ComponentMask* mask = new ComponentMask;
            mask->setBounds(comp->getLocalBounds());
            o.callback = new ComponentMaskCallBack(mask);
            comp->addChildComponent(mask, 100000);
            ComponentAnimator& animate = Desktop::getInstance().getAnimator();
            animate.fadeIn(mask, 200);
        }
    }

    if (!bModal)
        o.launchAsync();
    else
        o.runModal();
}

}