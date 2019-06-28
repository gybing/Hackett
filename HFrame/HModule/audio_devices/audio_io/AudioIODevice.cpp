

AudioIODevice::AudioIODevice (const String& deviceName, const String& deviceTypeName)
    : name (deviceName), typeName (deviceTypeName)
{
}

AudioIODevice::~AudioIODevice() {}

void AudioIODeviceCallback::audioDeviceError (const String&)    {}
bool AudioIODevice::setAudioPreprocessingEnabled (bool)         { return false; }
bool AudioIODevice::hasControlPanel() const                     { return false; }
int  AudioIODevice::getXRunCount() const noexcept               { return -1; }

bool AudioIODevice::showControlPanel()
{
    HAssertfalse;    // this should only be called for devices which return true from
                     // their hasControlPanel() method.
    return false;
}


