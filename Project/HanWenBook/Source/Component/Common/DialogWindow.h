#pragma once

namespace juce {

class CXDialogWindow : public CXDocumentWindow
{
public:
    CXDialogWindow (const String& name,
                  bool escapeKeyTriggersCloseButton,
                  bool addToDesktop);

    ~CXDialogWindow();

    struct LaunchOptions
    {
        LaunchOptions() noexcept;

        String dialogTitle;
        OptionalScopedPointer <Component> content;
        Component* componentToCentreAround;
        bool escapeKeyTriggersCloseButton;
        bool resiedEnable;
        juce::Justification titleLayout;
        ModalComponentManager::Callback* callback;

        int runModal();
        CXDialogWindow* launchAsync();
        CXDialogWindow* create();
    };

    static void showDialog (const String& dialogTitle,
                            Component* contentComponent,
                            Component* componentToCentreAround = nullptr,
                            bool bModal = true,
                            juce::Justification layout = juce::Justification::centred,
                            bool bResized = false);

protected:
    void resized() override;
    bool keyPressed (const KeyPress&) override;

private:
    bool escapeKeyTriggersCloseButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXDialogWindow)
};

}