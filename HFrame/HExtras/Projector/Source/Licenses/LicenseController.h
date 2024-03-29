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

struct LicenseThread;
class LicenseWebview;

//==============================================================================
struct LicenseState
{
    enum class Type
    {
        notLoggedIn,  // only used when the webview is open and the user has not logged in yet
        noLicenseChosenYet,

        GPL,          // GPL is used when the user enables the GPL compile flag
        personal,
        edu,
        indie,
        pro
    };

    enum class ApplicationUsageData
    {
        notChosenYet,

        enabled,
        disabled
    };

    Type type = Type::notLoggedIn;
    ApplicationUsageData applicationUsageDataState = ApplicationUsageData::notChosenYet;
    String username;
    String email;
    String authToken;

    static const char* licenseTypeToString (Type licenseType);

    bool isPaidOrGPL() const noexcept     { return (type == Type::GPL || type == Type::indie || type == Type::pro); }

    Image avatar;
};

//==============================================================================
class LicenseController
{
public:
    //==============================================================================
    struct StateChangedCallback
    {
        virtual ~StateChangedCallback() {}
        virtual void licenseStateChanged (const LicenseState&) = 0;
    };

    //==============================================================================
    LicenseController();
    ~LicenseController();

    void startWebviewIfNeeded();

    //==============================================================================
    LicenseState getState() const noexcept;
    void logout();
    void chooseNewLicense();
    void setApplicationUsageDataState (LicenseState::ApplicationUsageData newState);

    //==============================================================================
    void addLicenseStatusChangedCallback    (StateChangedCallback* callback) { listeners.add    (callback); }
    void removeLicenseStatusChangedCallback (StateChangedCallback* callback) { listeners.remove (callback); }

private:
    //==============================================================================
    void updateState (const LicenseState&);

    static LicenseState licenseStateFromOldSettings (XmlElement*);
    static LicenseState licenseStateFromSettings (PropertiesFile&);
    static void licenseStateToSettings (const LicenseState&, PropertiesFile&);

   #if ! HR_ENABLE_GPL_MODE
    //==============================================================================
    struct ModalCompletionCallback;
    friend struct LicenseThread;

    //==============================================================================
    void closeWebview (int);
    void modalStateFinished (int);
    void ensureLicenseWebviewIsOpenWithPage (const String&);
    void queryWebview (const String&, const String&, HashMap<String, String>&);

    //==============================================================================
    std::unique_ptr<LicenseThread> thread;
    LicenseWebview* licenseWebview = nullptr;
   #endif

    LicenseState state;
    ListenerList<LicenseController::StateChangedCallback> listeners;
    bool guiNotInitialisedYet = true;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicenseController)
};
