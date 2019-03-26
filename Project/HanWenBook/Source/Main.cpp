#include "JuceHeader.h"
#include "Component/MainComponent.h"
#include "Component/LoginComponent.h"

//==============================================================================
class HanWenBookApplication  : public juce::JUCEApplication
{
public:
    HanWenBookApplication() {}

    //==============================================================================
    void initialise (const juce::String&) override
    {
        juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().setAsCurrentWorkingDirectory();
        juce::Desktop::getInstance().setOrientationsEnabled (juce::Desktop::allOrientations);
        juce::Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel);

#if JUCE_WINDOWS
        juce::Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(L"微软雅黑");
        CEFClientApp::getInstance()->Initlize(&MainComponent::OnWebItemClick);
#endif

        CXOption::getInstance()->Initialize();
        HWDocManager::getInstance()->CreateDataModules();
        HWDocManager::getInstance()->Initialize();

        // debug mode
        {
            juce::File file_debug_mode(juce::File::getCurrentWorkingDirectory().getFullPathName() + L"\\cx_debug_mode");
            CXOption::getInstance()->setApplicationDebugMode(file_debug_mode.isDirectory());
        }
        

        if (!LoginWindowHelper::getInstance()->checkVersionAndLogin())
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }

    void Finalize()
    {
        HWDocManager::getInstance()->Finalize();
        HWDocManager::getInstance()->DestoryDataModules();
        HWDocManager::deleteInstance();
        CXOption::deleteInstance();

#if JUCE_WINDOWS
        CEFClientApp::getInstance()->Finalize();
#endif
    }

    void shutdown() override
    {
        ReadWindowHelper::getInstance()->closeWindow();
        ReadWindowHelper::deleteInstance();
        LoginWindowHelper::deleteInstance();
        Finalize();
    }

    void systemRequestedQuit() override
    {
        int dwType = CXOption::getInstance()->GetSystemExitType();
        if (dwType & CXSET_ASK)
        {
            if (!juce::AlertWindow::showOkCancelBox(juce::AlertWindow::AlertIconType::QuestionIcon, L"退出", L"是否退出？", L"退出", L"取消"))
                return;
        }

        dwType = CXOption::getInstance()->GetSystemExitType();

//         if (dwType & CXSET_HIDE)
//         {
//             mainWindow->setMinimised(true);
//             ReadWindowHelper::getInstance()->setMinimised(true);
//         }
//         else if (dwType & CXSET_EXIT)
        {
            quit();
        }
    }

    //==============================================================================
    const juce::String getApplicationName() override
    {
        return ProjectInfo::projectTitle;
    }

    const juce::String getApplicationVersion() override
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        juce::String strUserID, strUserToken;
        if (CXOption::ParseArgv(strUserID, strUserToken, commandLine.toUTF8()))
        {
            //Login(strUserID, strUserToken, HWStartupType_Lexin);
        }
    }

private:
    juce::LookAndFeelBase lookAndFeel;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(HanWenBookApplication)
