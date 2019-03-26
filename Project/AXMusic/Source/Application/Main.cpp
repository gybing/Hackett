#include "../JuceLibraryCode/JuceHeader.h"
#include "../AXBase/AXNetwork.h"
#include "../AXDouban/AXDouban.h"

Component* createMainContentComponent();

//==============================================================================
class AXMusicApplication  : public JUCEApplication
{
public:
    //==============================================================================
    AXMusicApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        HNetManager::getInstance();
        HDouBanMusic::getInstance();
        mainWindow = new MainWindow (getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        HDouBanMusic::deleteInstance();
        HNetManager::deleteInstance();
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
    }

    //==============================================================================
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name) : DocumentWindow(name, Colours::white, minimiseButton | closeButton)
        {
#ifdef JUCE_WINDOWS
            juce::Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(CharPointer_UTF8("\xe5\xbe\xae\xe8\xbd\xaf\xe9\x9b\x85\xe9\xbb\x91"));
#else
            juce::Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("STHeitiSC-Light");
#endif // JUCE_WINDOWS
            setUsingNativeTitleBar (true);
            setTitleBarHeight(20);
            setContentOwned (createMainContentComponent(), true);
            setResizable (false, false);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#endif
        }

        BorderSize<int> getBorderThickness() override
        {
            return BorderSize<int>(0);
        }

        void closeButtonPressed()
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
START_JUCE_APPLICATION (AXMusicApplication)
