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

        // ��ʾ�ڶԻ����е� Component. ����Ϊ��!
        // ʹ�� OptionalScopedPointer Ϊ���ڶԻ��������ʱ���Զ�ɾ������.
        OptionalScopedPointer <Component> content;

        // �����Ϊ�գ���������ѶԻ���������Component��ǰ��. �ο� DocumentWindow::centreAroundComponent().
        Component* componentToCentreAround;

        // ESC ���Ƿ���ԹرնԻ���.
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