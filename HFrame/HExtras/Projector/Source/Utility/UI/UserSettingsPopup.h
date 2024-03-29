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

#include "../../Licenses/LicenseController.h"
#include "../../Application/Application.h"

//==============================================================================
class UserSettingsPopup    : public Component
                            #if ! HR_ENABLE_GPL_MODE
                           , private LicenseController::StateChangedCallback
                            #endif
{
public:
    UserSettingsPopup (bool isShownInsideWebview)
       #if ! HR_ENABLE_GPL_MODE
        : isInsideWebview (isShownInsideWebview)
       #endif
    {
       #if HR_ENABLE_GPL_MODE
        ignoreUnused (isShownInsideWebview);
       #endif

        auto standardFont = Font (16.0f);

        loggedInUsernameLabel.reset (new Label ("Username Label"));
        addAndMakeVisible (loggedInUsernameLabel.get());

        loggedInUsernameLabel->setFont (standardFont);
        loggedInUsernameLabel->setJustificationType (Justification::centred);
        loggedInUsernameLabel->setMinimumHorizontalScale (0.75f);

       #if HR_ENABLE_GPL_MODE
        loggedInUsernameLabel->setText ("GPL Mode: Re-compile with HR_ENABLE_GPL_MODE=0 to enable login!",
                                        NotificationType::dontSendNotification);
       #else
        licenseTypeLabel.reset (new Label ("License Type Label"));
        addAndMakeVisible (licenseTypeLabel.get());

        licenseTypeLabel->setFont (standardFont);
        licenseTypeLabel->setJustificationType (Justification::centred);
        licenseTypeLabel->setMinimumHorizontalScale (1.0f);

        logoutButton.reset (new TextButton (isInsideWebview ? "Select different account..." : "Sign out"));
        addAndMakeVisible (logoutButton.get());
        logoutButton->setColour (TextButton::buttonColourId, findColour (secondaryButtonBackgroundColourId));

        logoutButton->onClick = [this]
        {
            dismissCalloutBox();
            ProjectorApplication::getApp().doLogout();
        };

        if (! isInsideWebview)
        {
            switchLicenseButton.reset (new TextButton ("Switch License"));
            addAndMakeVisible (switchLicenseButton.get());
            switchLicenseButton->onClick = [this]
            {
                dismissCalloutBox();

                if (auto* controller = ProjectorApplication::getApp().licenseController.get())
                    controller->chooseNewLicense();
            };
        }

        if (auto* controller = ProjectorApplication::getApp().licenseController.get())
            licenseStateChanged (controller->getState());
       #endif
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (secondaryBackgroundColourId));
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (10);

       #if HR_ENABLE_GPL_MODE
        loggedInUsernameLabel->setBounds (bounds);
       #else
        loggedInUsernameLabel->setBounds (bounds.removeFromTop (hasLicenseType ? 25 : 75));

        if (hasLicenseType)
        {
            bounds.removeFromTop (10);
            licenseTypeLabel->setBounds (bounds.removeFromTop (25));
        }

        bounds.removeFromBottom (5);
        auto buttonArea = bounds.removeFromBottom (40);

        if (! isInsideWebview)
            switchLicenseButton->setBounds (buttonArea.removeFromRight (buttonArea.getWidth() / 2).reduced (2));

        logoutButton->setBounds (buttonArea.reduced (2));
       #endif
    }

private:
    //==============================================================================
    void dismissCalloutBox()
    {
        if (auto* parent = findParentComponentOfClass<CallOutBox>())
            parent->dismiss();
    }

   #if ! HR_ENABLE_GPL_MODE
    void licenseStateChanged (const LicenseState& state) override
    {
        hasLicenseType = (state.type != LicenseState::Type::noLicenseChosenYet);
        licenseTypeLabel->setVisible (hasLicenseType);
        loggedInUsernameLabel->setText (state.username, NotificationType::dontSendNotification);
        licenseTypeLabel->setText (LicenseState::licenseTypeToString (state.type), NotificationType::dontSendNotification);
    }

    void lookAndFeelChanged() override
    {
        if (logoutButton != nullptr)
            logoutButton->setColour (TextButton::buttonColourId, findColour (secondaryButtonBackgroundColourId));
    }
   #endif

    //==============================================================================
    std::unique_ptr<Label> loggedInUsernameLabel;

   #if ! HR_ENABLE_GPL_MODE
    std::unique_ptr<Label> licenseTypeLabel;
    std::unique_ptr<TextButton> logoutButton, switchLicenseButton;
    bool hasLicenseType = false;
    bool isInsideWebview;
   #endif

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserSettingsPopup)
};
