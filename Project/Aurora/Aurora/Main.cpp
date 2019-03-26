#include "stdafx.h"
#include "MainWindow.h"

HINSTANCE g_hInstance = NULL;
//==============================================================================
class HanWenBookApplication  : public JUCEApplication
{
public:
    HanWenBookApplication() {}

    //==============================================================================
    void initialise (const String& str) override
    {
        Desktop::getInstance().setOrientationsEnabled (Desktop::allOrientations);
        Desktop::getInstance().setDefaultLookAndFeel(LAFBase::GetCustamLookAndFeel());
        mainWindow = new MainAppWindow();
        mainWindow->setVisible(true);

        g_hInstance = GetModuleHandle(0);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    
    void systemRequestedQuit() override
    {
        quit();
    }

    //==============================================================================
    const String getApplicationName() override
    {
        return String(L"¼«¹â");
    }

    const String getApplicationVersion() override
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void anotherInstanceStarted (const String& /*commandLine*/) override
    {
        int a = 0;
    }

private:
    ScopedPointer<MainAppWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(HanWenBookApplication)
