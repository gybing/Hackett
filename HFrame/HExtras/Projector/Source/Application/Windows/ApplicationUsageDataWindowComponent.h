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


//==============================================================================
class ApplicationUsageDataWindowComponent    : public Component
{
public:
    ApplicationUsageDataWindowComponent (bool showCheckbox)
    {
        addAndMakeVisible (headerLabel);
        headerLabel.setText ("Application Usage Analytics", dontSendNotification);
        headerLabel.setFont (Font (20.0f, Font::FontStyleFlags::bold));
        headerLabel.setJustificationType (Justification::centred);

        auto textToShow = String ("We use analytics services to understand how developers use our software in order for H to improve its software and services. ");

        if (! showCheckbox)
            textToShow += String (" Analytics can be disabled with an Indie or Pro license. ");

        textToShow += String ("For more information, please read the H EULA and Privacy policy:");

        addAndMakeVisible (bodyLabel);
        bodyLabel.setText (textToShow, dontSendNotification);
        bodyLabel.setFont (Font (14.0f));
        bodyLabel.setJustificationType (Justification::centredLeft);

        addAndMakeVisible (HEULALink);
        HEULALink.setButtonText ("H EULA");
        HEULALink.setFont (Font (14.0f), false);
        HEULALink.setURL (URL ("https://H.com/H-5-license"));

        addAndMakeVisible (privacyPolicyLink);
        privacyPolicyLink.setButtonText ("Privacy Policy");
        privacyPolicyLink.setFont (Font (14.0f), false);
        privacyPolicyLink.setURL (URL ("https://H.com/H-5-privacy-policy"));

        addAndMakeVisible (okButton);

        if (showCheckbox)
        {
            shareApplicationUsageDataToggle.reset (new ToggleButton());
            addAndMakeVisible (shareApplicationUsageDataToggle.get());

            auto* controller = ProjectorApplication::getApp().licenseController.get();

            if (controller != nullptr && controller->getState().applicationUsageDataState == LicenseState::ApplicationUsageData::disabled)
                shareApplicationUsageDataToggle->setToggleState (false, dontSendNotification);
            else
                shareApplicationUsageDataToggle->setToggleState (true, dontSendNotification);

            addAndMakeVisible (shareApplicationUsageDataLabel);
            shareApplicationUsageDataLabel.setFont (Font (14.0f));
            shareApplicationUsageDataLabel.setMinimumHorizontalScale (1.0f);
        }
        else
        {
            addAndMakeVisible (upgradeLicenseButton);
            upgradeLicenseButton.setColour (TextButton::buttonColourId, findColour (secondaryButtonBackgroundColourId));

            upgradeLicenseButton.onClick = []
            {
                if (auto* controller = ProjectorApplication::getApp().licenseController.get())
                    controller->chooseNewLicense();
            };
        }
    }

    ~ApplicationUsageDataWindowComponent() override
    {
        if (auto* controller = ProjectorApplication::getApp().licenseController.get())
        {
            auto newApplicationUsageDataState = LicenseState::ApplicationUsageData::enabled;

            if (shareApplicationUsageDataToggle != nullptr && ! shareApplicationUsageDataToggle->getToggleState())
                newApplicationUsageDataState = LicenseState::ApplicationUsageData::disabled;

            controller->setApplicationUsageDataState (newApplicationUsageDataState);
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (20);
        headerLabel.setBounds (bounds.removeFromTop (40));
        bodyLabel.setBounds (bounds.removeFromTop (75));

        bounds.removeFromTop (10);

        auto linkBounds = bounds.removeFromTop (20);
        HEULALink.setBounds (linkBounds.removeFromLeft (linkBounds.getWidth() / 2).reduced (2));
        privacyPolicyLink.setBounds (linkBounds.reduced (2));

        if (shareApplicationUsageDataToggle != nullptr)
        {
            bounds.removeFromTop (10);

            auto toggleBounds = bounds.removeFromTop (40);
            shareApplicationUsageDataToggle->setBounds (toggleBounds.removeFromLeft (40).reduced (5));
            shareApplicationUsageDataLabel.setBounds (toggleBounds);
        }

        bounds.removeFromTop (10);

        auto buttonW = 125;
        auto buttonH = 40;

        if (upgradeLicenseButton.isShowing())
        {
            auto left = bounds.removeFromLeft (bounds.getWidth() / 2);

            upgradeLicenseButton.setSize (buttonW, buttonH);
            upgradeLicenseButton.setCentrePosition (left.getCentreX(), left.getCentreY());
        }

        okButton.setSize (buttonW, buttonH);
        okButton.setCentrePosition (bounds.getCentreX(), bounds.getCentreY());
        okButton.onClick = [] { ProjectorApplication::getApp().dismissApplicationUsageDataAgreementPopup(); };
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (backgroundColourId));
    }

private:
    Label headerLabel, bodyLabel;
    HyperlinkButton HEULALink, privacyPolicyLink;
    Label shareApplicationUsageDataLabel { {}, "Help H to improve its software and services by sharing my application usage data" };
    std::unique_ptr<ToggleButton> shareApplicationUsageDataToggle;
    TextButton okButton { "OK" }, upgradeLicenseButton { "Upgrade License" };

    void lookAndFeelChanged() override
    {
        upgradeLicenseButton.setColour (TextButton::buttonColourId, findColour (secondaryButtonBackgroundColourId));
    }

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ApplicationUsageDataWindowComponent)
};
