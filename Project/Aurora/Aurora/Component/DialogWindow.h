#pragma once

namespace juce
{

class CXDialogWindow : public CXDocumentWindow
{
public:
    CXDialogWindow (const String& name,
                  Colour backgroundColour,
                  bool escapeKeyTriggersCloseButton,
                  bool addToDesktop = true);

    ~CXDialogWindow();

    struct LaunchOptions
    {
        LaunchOptions() noexcept;

        String dialogTitle;
        Colour dialogBackgroundColour;

        // 显示在对话框中的 Component. 不能为空!
        // 使用 OptionalScopedPointer 为了在对话框结束的时候自动删除内容.
        OptionalScopedPointer <Component> content;

        // 如果不为空，表明你想把对话框放在这个Component的前面. 参考 DocumentWindow::centreAroundComponent().
        Component* componentToCentreAround;

        // ESC 键是否可以关闭对话框.
        bool escapeKeyTriggersCloseButton;
        bool useNativeTitleBar;
        bool resizable;
        bool useBottomRightCornerResizer;

        CXDialogWindow* create();

        CXDialogWindow* launchAsync();
        int runModal();
    };

    static void showDialog (const String& dialogTitle,
                            Component* contentComponent,
                            Component* componentToCentreAround,
                            Colour backgroundColour,
                            bool escapeKeyTriggersCloseButton,
                            bool shouldBeResizable = false,
                            bool useBottomRightCornerResizer = false);

    static int showModalDialog (const String& dialogTitle,
                                Component* contentComponent,
                                Component* componentToCentreAround,
                                Colour backgroundColour,
                                bool escapeKeyTriggersCloseButton,
                                bool shouldBeResizable = false,
                                bool useBottomRightCornerResizer = false);


protected:
    void resized() override;
    bool keyPressed (const KeyPress&) override;

private:
    bool escapeKeyTriggersCloseButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CXDialogWindow)
};

}