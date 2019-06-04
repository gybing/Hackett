/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "%%filename%%"

%%component_begin%%
class Application    : public HApplication
{
public:
    //==============================================================================
    Application() {}

    const String getApplicationName() override       { return "%%project_name%%"; }
    const String getApplicationVersion() override    { return "%%project_version%%"; }

    void initialise (const String&) override         { %%startup%% }
    void shutdown() override                         { %%shutdown%% }

private:
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (const String& name, Component* c, HApplication& a)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if HANDROID || HIOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        HApplication& app;

        //==============================================================================
        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_HAPPLICATION (Application)
%%component_end%%

%%audioprocessor_begin%%
//==============================================================================
AudioProcessor* CALLTYPE createPluginFilter()
{
    return new %%class_name%%();
}
%%audioprocessor_end%%

%%console_begin%%
%%console_end%%
