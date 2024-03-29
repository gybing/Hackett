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

bool PluginDescription::isDuplicateOf (const PluginDescription& other) const noexcept
{
    return fileOrIdentifier == other.fileOrIdentifier
            && uid == other.uid;
}

static String getPluginDescSuffix (const PluginDescription& d)
{
    return "-" + CharacterFunctions::hexToString (d.fileOrIdentifier.hashCode())
         + "-" + CharacterFunctions::hexToString (d.uid);
}

bool PluginDescription::matchesIdentifierString (const String& identifierString) const
{
    return identifierString.endsWithIgnoreCase (getPluginDescSuffix (*this));
}

String PluginDescription::createIdentifierString() const
{
    return pluginFormatName + "-" + name + getPluginDescSuffix (*this);
}

std::unique_ptr<XmlElement> PluginDescription::createXml() const
{
    auto e = std::make_unique<XmlElement> ("PLUGIN");

    e->setAttribute ("name", name);

    if (descriptiveName != name)
        e->setAttribute ("descriptiveName", descriptiveName);

    e->setAttribute ("format", pluginFormatName);
    e->setAttribute ("category", category);
    e->setAttribute ("manufacturer", manufacturerName);
    e->setAttribute ("version", version);
    e->setAttribute ("file", fileOrIdentifier);
    e->setAttribute ("uid", CharacterFunctions::hexToString (uid));
    e->setAttribute ("isInstrument", isInstrument);
    e->setAttribute ("fileTime", CharacterFunctions::hexToString (lastFileModTime.toMilliseconds()));
    e->setAttribute ("infoUpdateTime", CharacterFunctions::hexToString (lastInfoUpdateTime.toMilliseconds()));
    e->setAttribute ("numInputs", numInputChannels);
    e->setAttribute ("numOutputs", numOutputChannels);
    e->setAttribute ("isShell", hasSharedContainer);

    return e;
}

bool PluginDescription::loadFromXml (const XmlElement& xml)
{
    if (xml.hasTagName ("PLUGIN"))
    {
        name                = xml.getStringAttribute ("name");
        descriptiveName     = xml.getStringAttribute ("descriptiveName", name);
        pluginFormatName    = xml.getStringAttribute ("format");
        category            = xml.getStringAttribute ("category");
        manufacturerName    = xml.getStringAttribute ("manufacturer");
        version             = xml.getStringAttribute ("version");
        fileOrIdentifier    = xml.getStringAttribute ("file");
        uid                 = xml.getStringAttribute ("uid").getHexValue32();
        isInstrument        = xml.getBoolAttribute ("isInstrument", false);
        lastFileModTime     = Time (xml.getStringAttribute ("fileTime").getHexValue64());
        lastInfoUpdateTime  = Time (xml.getStringAttribute ("infoUpdateTime").getHexValue64());
        numInputChannels    = xml.getIntAttribute ("numInputs");
        numOutputChannels   = xml.getIntAttribute ("numOutputs");
        hasSharedContainer  = xml.getBoolAttribute ("isShell", false);

        return true;
    }

    return false;
}


