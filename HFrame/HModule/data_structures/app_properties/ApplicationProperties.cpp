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

ApplicationProperties::ApplicationProperties()
{
}

ApplicationProperties::~ApplicationProperties()
{
    closeFiles();
}

//==============================================================================
void ApplicationProperties::setStorageParameters (const PropertiesFile::Options& newOptions)
{
    options = newOptions;
}

//==============================================================================
void ApplicationProperties::openFiles()
{
    // You need to call setStorageParameters() before trying to get hold of the properties!
    HAssert (options.applicationName.isNotEmpty());

    if (options.applicationName.isNotEmpty())
    {
        PropertiesFile::Options o (options);

        if (userProps == nullptr)
        {
            o.commonToAllUsers = false;
            userProps.reset (new PropertiesFile (o));
        }

        if (commonProps == nullptr)
        {
            o.commonToAllUsers = true;
            commonProps.reset (new PropertiesFile (o));
        }

        userProps->setFallbackPropertySet (commonProps.get());
    }
}

PropertiesFile* ApplicationProperties::getUserSettings()
{
    if (userProps == nullptr)
        openFiles();

    return userProps.get();
}

PropertiesFile* ApplicationProperties::getCommonSettings (const bool returnUserPropsIfReadOnly)
{
    if (commonProps == nullptr)
        openFiles();

    if (returnUserPropsIfReadOnly)
    {
        if (commonSettingsAreReadOnly == 0)
            commonSettingsAreReadOnly = commonProps->save() ? -1 : 1;

        if (commonSettingsAreReadOnly > 0)
            return userProps.get();
    }

    return commonProps.get();
}

bool ApplicationProperties::saveIfNeeded()
{
    return (userProps == nullptr || userProps->saveIfNeeded())
         && (commonProps == nullptr || commonProps->saveIfNeeded());
}

void ApplicationProperties::closeFiles()
{
    userProps.reset();
    commonProps.reset();
}


