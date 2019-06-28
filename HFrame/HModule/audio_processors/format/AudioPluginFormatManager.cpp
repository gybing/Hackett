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

namespace PluginFormatManagerHelpers
{
    struct ErrorCallbackOnMessageThread : public CallbackMessage
    {
        ErrorCallbackOnMessageThread (const String& inError,
                                      AudioPluginFormat::InstantiationCompletionCallback* c)
            : error (inError), callback (c)
        {
        }

        void messageCallback() override          { callback->completionCallback (nullptr, error); }

        String error;
        std::unique_ptr<AudioPluginFormat::InstantiationCompletionCallback> callback;
    };

    struct ErrorLambdaOnMessageThread : public CallbackMessage
    {
        ErrorLambdaOnMessageThread (const String& inError,
                                    std::function<void(AudioPluginInstance*, const String&)> f)
            : error (inError), lambda (f)
        {
        }

        void messageCallback() override          { lambda (nullptr, error); }

        String error;
        std::function<void(AudioPluginInstance*, const String&)> lambda;
    };
}

AudioPluginFormatManager::AudioPluginFormatManager() {}
AudioPluginFormatManager::~AudioPluginFormatManager() {}

//==============================================================================
void AudioPluginFormatManager::addDefaultFormats()
{
   #if HDEBUG
    // you should only call this method once!
    for (auto* format : formats)
    {
        ignoreUnused (format);

       #if HPLUGINHOST_VST && (HMAC || HWINDOWS || HLINUX || HIOS)
        HAssert (dynamic_cast<VSTPluginFormat*> (format) == nullptr);
       #endif

       #if HPLUGINHOST_VST3 && (HMAC || HWINDOWS)
        HAssert (dynamic_cast<VST3PluginFormat*> (format) == nullptr);
       #endif

       #if HPLUGINHOST_AU && (HMAC || HIOS)
        HAssert (dynamic_cast<AudioUnitPluginFormat*> (format) == nullptr);
       #endif

       #if HPLUGINHOST_LADSPA && HLINUX
        HAssert (dynamic_cast<LADSPAPluginFormat*> (format) == nullptr);
       #endif
    }
   #endif

   #if HPLUGINHOST_AU && (HMAC || HIOS)
    formats.add (new AudioUnitPluginFormat());
   #endif

   #if HPLUGINHOST_VST && (HMAC || HWINDOWS || HLINUX || HIOS)
    formats.add (new VSTPluginFormat());
   #endif

   #if HPLUGINHOST_VST3 && (HMAC || HWINDOWS)
    formats.add (new VST3PluginFormat());
   #endif

   #if HPLUGINHOST_LADSPA && HLINUX
    formats.add (new LADSPAPluginFormat());
   #endif
}

int AudioPluginFormatManager::getNumFormats()
{
    return formats.size();
}

AudioPluginFormat* AudioPluginFormatManager::getFormat (int index)
{
    return formats[index];
}

void AudioPluginFormatManager::addFormat (AudioPluginFormat* format)
{
    formats.add (format);
}

AudioPluginInstance* AudioPluginFormatManager::createPluginInstance (const PluginDescription& description, double rate,
                                                                     int blockSize, String& errorMessage) const
{
    if (auto* format = findFormatForDescription (description, errorMessage))
        return format->createInstanceFromDescription (description, rate, blockSize, errorMessage);

    return nullptr;
}

void AudioPluginFormatManager::createPluginInstanceAsync (const PluginDescription& description,
                                                          double initialSampleRate,
                                                          int initialBufferSize,
                                                          AudioPluginFormat::InstantiationCompletionCallback* callback)
{
    String error;

    if (auto* format = findFormatForDescription (description, error))
        return format->createPluginInstanceAsync (description, initialSampleRate, initialBufferSize, callback);

    (new PluginFormatManagerHelpers::ErrorCallbackOnMessageThread (error, callback))->post();
}

void AudioPluginFormatManager::createPluginInstanceAsync (const PluginDescription& description,
                                                          double initialSampleRate,
                                                          int initialBufferSize,
                                                          std::function<void(AudioPluginInstance*, const String&)> f)
{
    String error;

    if (auto* format = findFormatForDescription (description, error))
        return format->createPluginInstanceAsync (description, initialSampleRate, initialBufferSize, f);

    (new PluginFormatManagerHelpers::ErrorLambdaOnMessageThread (error, f))->post();
}

AudioPluginFormat* AudioPluginFormatManager::findFormatForDescription (const PluginDescription& description,
                                                                       String& errorMessage) const
{
    errorMessage = {};

    for (auto* format : formats)
        if (format->getName() == description.pluginFormatName
              && format->fileMightContainThisPluginType (description.fileOrIdentifier))
            return format;

    errorMessage = NEEDS_TRANS ("No compatible plug-in format exists for this plug-in");

    return nullptr;
}

bool AudioPluginFormatManager::doesPluginStillExist (const PluginDescription& description) const
{
    for (auto* format : formats)
        if (format->getName() == description.pluginFormatName)
            return format->doesPluginStillExist (description);

    return false;
}


