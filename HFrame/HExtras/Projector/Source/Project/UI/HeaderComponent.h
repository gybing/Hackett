/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../../Application/Headers.h"
#include "../../Utility/UI/IconButton.h"
#include "../../Utility/UI/UserSettingsPopup.h"

class Project;

//==============================================================================
class HeaderComponent    : public Component,
                           private ValueTree::Listener,
                           private ChangeListener,
                           private Value::Listener,
                           private Timer
{
public:
    HeaderComponent();
    ~HeaderComponent() override;

    //==========================================================================
    void resized() override;
    void paint (Graphics&) override;

    //==========================================================================
    void setCurrentProject (Project*) noexcept;

    //==========================================================================
    void updateExporters() noexcept;
    String getSelectedExporterName() const noexcept;
    bool canCurrentExporterLaunchProject() const noexcept;

    //==========================================================================
    int getUserButtonWidth() const noexcept;
    void sidebarTabsWidthChanged (int newWidth) noexcept;

    //==========================================================================
    void showUserSettings() noexcept;

private:
    //==========================================================================
    void lookAndFeelChanged() override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void valueChanged (Value&) override;
    void timerCallback() override;

    //==========================================================================
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree&) override        { updateIfNeeded (parentTree); }
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree&, int) override { updateIfNeeded (parentTree); }
    void valueTreeChildOrderChanged (ValueTree& parentTree, int, int) override   { updateIfNeeded (parentTree); }

    void updateIfNeeded (ValueTree tree) noexcept
    {
        if (tree == exportersTree)
            updateExporters();
    }

    //==========================================================================
    void initialiseButtons() noexcept;

    void updateName() noexcept;
    void updateExporterButton() noexcept;
    void updateUserAvatar() noexcept;

    //==========================================================================
    void buildPing();
    void buildFinished (bool);
    void setRunAppButtonState (bool);

    //==========================================================================
    int tabsWidth = 200;
    bool isBuilding = false;

    Project* project = nullptr;
    ValueTree exportersTree;

    Value projectNameValue;

    ComboBox exporterBox;
    Label configLabel  { "Config Label", "Selected exporter" },
    projectNameLabel;

    std::unique_ptr<ImageComponent> HIcon;
    std::unique_ptr<IconButton> projectSettingsButton, saveAndOpenInIDEButton, userSettingsButton, runAppButton;

    SafePointer<CallOutBox> userSettingsWindow;

    ReferenceCountedObjectPtr<CompileEngineChildProcess> childProcess;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
