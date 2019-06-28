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

#include "HeaderComponent.h"

#include "../../ProjectSaving/ProjectExporter.h"
#include "../../Project/UI/ProjectContentComponent.h"

#include "../../LiveBuildEngine/MessageIDs.h"
#include "../../LiveBuildEngine/SourceCodeRange.h"
#include "../../LiveBuildEngine/ClassDatabase.h"
#include "../../LiveBuildEngine/DiagnosticMessage.h"
#include "../../LiveBuildEngine/CompileEngineClient.h"

//======================================================================
HeaderComponent::HeaderComponent()
{
    addAndMakeVisible (configLabel);
    addAndMakeVisible (exporterBox);

    exporterBox.onChange = [this] { updateExporterButton(); };

    HIcon.reset (new ImageComponent ("icon"));
    addAndMakeVisible (HIcon.get());
    HIcon->setImage (ImageCache::getFromMemory (BinaryData::icon_png, BinaryData::icon_pngSize),
                        RectanglePlacement::centred);

    projectNameLabel.setText ({}, dontSendNotification);
    addAndMakeVisible (projectNameLabel);

    initialiseButtons();
}

HeaderComponent::~HeaderComponent()
{
    if (userSettingsWindow != nullptr)
        userSettingsWindow->dismiss();

    if (childProcess != nullptr)
    {
        childProcess->activityList.removeChangeListener(this);
        childProcess->errorList.removeChangeListener (this);
    }
}

//======================================================================
void HeaderComponent::resized()
{
    auto bounds = getLocalBounds();
    configLabel.setFont ({ bounds.getHeight() / 3.0f });

    //======================================================================
    {
        auto headerBounds = bounds.removeFromLeft (tabsWidth);

        const int buttonSize = 25;
        auto buttonBounds = headerBounds.removeFromRight (buttonSize);

        projectSettingsButton->setBounds (buttonBounds.removeFromBottom (buttonSize).reduced (2));

        HIcon->setBounds (headerBounds.removeFromLeft (headerBounds.getHeight()).reduced (2));

        headerBounds.removeFromRight (5);
        projectNameLabel.setBounds (headerBounds);
    }

    //======================================================================
    auto exporterWidth = jmin (400, bounds.getWidth() / 2);
    Rectangle<int> exporterBounds (0, 0, exporterWidth, bounds.getHeight());

    exporterBounds.setCentre (bounds.getCentre());

    runAppButton->setBounds (exporterBounds.removeFromRight (exporterBounds.getHeight()).reduced (2));
    saveAndOpenInIDEButton->setBounds (exporterBounds.removeFromRight (exporterBounds.getHeight()).reduced (2));

    exporterBounds.removeFromRight (5);
    exporterBox.setBounds (exporterBounds.removeFromBottom (roundToInt (exporterBounds.getHeight() / 1.8f)));
    configLabel.setBounds (exporterBounds);

    bounds.removeFromRight (5);
    userSettingsButton->setBounds (bounds.removeFromRight (bounds.getHeight()).reduced (2));
}

void HeaderComponent::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColourId));

    if (isBuilding)
        getLookAndFeel().drawSpinningWaitAnimation (g, findColour (treeIconColourId),
                                                    runAppButton->getX(), runAppButton->getY(),
                                                    runAppButton->getWidth(), runAppButton->getHeight());
}

//======================================================================
void HeaderComponent::setCurrentProject (Project* p) noexcept
{
    project = p;

    exportersTree = project->getExporters();
    exportersTree.addListener (this);
    updateExporters();

    projectNameValue.referTo (project->getProjectValue (Ids::name));
    projectNameValue.addListener (this);
    updateName();

    isBuilding = false;
    stopTimer();
    repaint();

    childProcess = ProjectorApplication::getApp().childProcessCache->getExisting (*project);

    if (childProcess != nullptr)
    {
        childProcess->activityList.addChangeListener (this);
        childProcess->errorList.addChangeListener (this);

        runAppButton->setTooltip ({});
        runAppButton->setEnabled (true);
    }
    else
    {
        runAppButton->setTooltip ("Enable live-build engine to launch application");
        runAppButton->setEnabled (false);
    }
}

//======================================================================
void HeaderComponent::updateExporters() noexcept
{
    auto selectedName = getSelectedExporterName();

    exporterBox.clear();
    auto preferredExporterIndex = -1;

    int i = 0;
    for (Project::ExporterIterator exporter (*project); exporter.next(); ++i)
    {
        exporterBox.addItem (exporter->getName(), i + 1);

        if (selectedName == exporter->getName())
            exporterBox.setSelectedId (i + 1);

        if (exporter->getName().contains (ProjectExporter::getCurrentPlatformExporterName()) && preferredExporterIndex == -1)
            preferredExporterIndex = i;
    }

    if (exporterBox.getSelectedItemIndex() == -1)
    {
        if (preferredExporterIndex == -1)
        {
            i = 0;
            for (Project::ExporterIterator exporter (*project); exporter.next(); ++i)
            {
                if (exporter->canLaunchProject())
                {
                    preferredExporterIndex = i;
                    break;
                }
            }
        }

        exporterBox.setSelectedItemIndex (preferredExporterIndex != -1 ? preferredExporterIndex : 0);
    }

    updateExporterButton();
}

String HeaderComponent::getSelectedExporterName() const noexcept
{
    return exporterBox.getItemText (exporterBox.getSelectedItemIndex());
}

bool HeaderComponent::canCurrentExporterLaunchProject() const noexcept
{
    for (Project::ExporterIterator exporter (*project); exporter.next();)
        if (exporter->getName() == getSelectedExporterName() && exporter->canLaunchProject())
            return true;

    return false;
}

//======================================================================
int HeaderComponent::getUserButtonWidth() const noexcept
{
    return userSettingsButton->getWidth();
}

void HeaderComponent::sidebarTabsWidthChanged (int newWidth) noexcept
{
    tabsWidth = newWidth;
    resized();
}

//======================================================================
void HeaderComponent::showUserSettings() noexcept
{
   #if HR_ENABLE_GPL_MODE
    auto settingsPopupHeight = 100;
    auto settingsPopupWidth = 200;
   #else
    auto settingsPopupHeight = 150;
    auto settingsPopupWidth = 250;
   #endif

    auto* content = new UserSettingsPopup (false);

    content->setSize (settingsPopupWidth, settingsPopupHeight);

    userSettingsWindow = &CallOutBox::launchAsynchronously (content, userSettingsButton->getScreenBounds(), nullptr);
}

//==========================================================================
void HeaderComponent::lookAndFeelChanged()
{
    if (userSettingsWindow != nullptr)
        userSettingsWindow->sendLookAndFeelChange();
}

void HeaderComponent::changeListenerCallback (ChangeBroadcaster*)
{
    if (childProcess != nullptr)
    {
        if (childProcess->activityList.getNumActivities() > 0)
            buildPing();
        else
            buildFinished (childProcess->errorList.getNumErrors() == 0);
    }
}

void HeaderComponent::valueChanged (Value&)
{
    updateName();
}

void HeaderComponent::timerCallback()
{
    repaint();
}

//======================================================================
static void sendProjectButtonAnalyticsEvent (StringRef label)
{
    StringPairArray data;
    data.set ("label", label);

    Analytics::getInstance()->logEvent ("Project Button",  data, ProjectorAnalyticsEvent::projectEvent);
}

void HeaderComponent::initialiseButtons() noexcept
{
    auto& icons = getIcons();

    projectSettingsButton.reset (new IconButton ("Project Settings", &icons.settings));
    addAndMakeVisible (projectSettingsButton.get());
    projectSettingsButton->onClick = [this]
    {
        sendProjectButtonAnalyticsEvent ("Project Settings");

        if (auto* pcc = findParentComponentOfClass<ProjectContentComponent>())
            pcc->showProjectSettings();
    };

    saveAndOpenInIDEButton.reset (new IconButton ("Save and Open in IDE", nullptr));
    addAndMakeVisible (saveAndOpenInIDEButton.get());
    saveAndOpenInIDEButton->isIDEButton = true;
    saveAndOpenInIDEButton->onClick = [this]
    {
        sendProjectButtonAnalyticsEvent ("Save and Open in IDE (" + exporterBox.getText() + ")");

        if (auto* pcc = findParentComponentOfClass<ProjectContentComponent>())
            pcc->openInSelectedIDE (true);
    };

    userSettingsButton.reset (new IconButton ("User Settings", &icons.user));
    addAndMakeVisible (userSettingsButton.get());
    userSettingsButton->isUserButton = true;
    userSettingsButton->onClick = [this]
    {
        sendProjectButtonAnalyticsEvent ("User Settings");

        if (findParentComponentOfClass<ProjectContentComponent>() != nullptr)
            showUserSettings();
    };

    runAppButton.reset (new IconButton ("Run Application", &icons.play));
    addAndMakeVisible (runAppButton.get());
    runAppButton->onClick = [this]
    {
        sendProjectButtonAnalyticsEvent ("Run Application");

        if (childProcess != nullptr)
            childProcess->launchApp();
    };

    updateExporterButton();
    updateUserAvatar();
}

void HeaderComponent::updateName() noexcept
{
    projectNameLabel.setText (project->getDocumentTitle(), dontSendNotification);
}

void HeaderComponent::updateExporterButton() noexcept
{
    auto currentExporterName = getSelectedExporterName();

    for (auto info : ProjectExporter::getExporterTypes())
    {
        if (currentExporterName.contains (info.name))
        {
            saveAndOpenInIDEButton->iconImage = info.getIcon();
            saveAndOpenInIDEButton->repaint();
            saveAndOpenInIDEButton->setEnabled (canCurrentExporterLaunchProject());
        }
    }
}

void HeaderComponent::updateUserAvatar() noexcept
{
    if (auto* controller = ProjectorApplication::getApp().licenseController.get())
    {
        auto state = controller->getState();

        userSettingsButton->iconImage = state.avatar;
        userSettingsButton->repaint();
    }
}

//======================================================================
void HeaderComponent::buildPing()
{
    if (! isTimerRunning())
    {
        isBuilding = true;
        runAppButton->setEnabled (false);
        runAppButton->setTooltip ("Building...");

        startTimer (50);
    }
}

void HeaderComponent::buildFinished (bool success)
{
    stopTimer();
    isBuilding = false;

    repaint();

    setRunAppButtonState (success);
}

void HeaderComponent::setRunAppButtonState (bool buildWasSuccessful)
{
    bool shouldEnableButton = false;

    if (buildWasSuccessful)
    {
        if (childProcess != nullptr)
        {
            if (childProcess->isAppRunning() || (! childProcess->isAppRunning() && childProcess->canLaunchApp()))
            {
                runAppButton->setTooltip ("Launch application");
                shouldEnableButton = true;
            }
            else
            {
                runAppButton->setTooltip ("Application can't be launched");
            }
        }
        else
        {
            runAppButton->setTooltip ("Enable live-build engine to launch application");
        }
    }
    else
    {
        runAppButton->setTooltip ("Error building application");
    }

    runAppButton->setEnabled (shouldEnableButton);
}
