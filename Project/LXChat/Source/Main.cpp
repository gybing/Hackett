#include "JuceHeader.h"
#include "MainComponent.h"

void putData(int i, int j, int k)
{
    int a = j + k + i + 5;
    printf("%d\n", a);
}

//==============================================================================
class LXChatApplication  : public JUCEApplication
{
public:
    //==============================================================================
    LXChatApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(L"Î¢ÈíÑÅºÚ");
        File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().setAsCurrentWorkingDirectory();
        ignoreUnused(commandLine);
        mainWindow = new MainWindow (getApplicationName());
    }

    void shutdown() override
    {

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        ignoreUnused(commandLine);
    }

    //==============================================================================
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name) : DocumentWindow(name, Colours::white, 0)
        {
            setUsingNativeTitleBar (false);
            setTitleBarHeight(0);
            setContentOwned (new MainContentComponent(), true);
            getContentComponent()->setInterceptsMouseClicks(false, true);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        BorderSize<int> getBorderThickness() override
        {
            return BorderSize<int>(0);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION (LXChatApplication)
