

AudioIODeviceType::AudioIODeviceType (const String& name)
    : typeName (name)
{
}

AudioIODeviceType::~AudioIODeviceType()
{
}

//==============================================================================
void AudioIODeviceType::addListener (Listener* l)      { listeners.add (l); }
void AudioIODeviceType::removeListener (Listener* l)   { listeners.remove (l); }

void AudioIODeviceType::callDeviceChangeListeners()
{
    listeners.call ([] (Listener& l) { l.audioDeviceListChanged(); });
}

//==============================================================================
#if ! HMAC
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_CoreAudio()       { return nullptr; }
#endif

#if ! HIOS
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_iOSAudio()        { return nullptr; }
#endif

#if ! (HWINDOWS && HWASAPI)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_WASAPI (bool)     { return nullptr; }
#endif

#if ! (HWINDOWS && HDIRECTSOUND)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_DirectSound()     { return nullptr; }
#endif

#if ! (HWINDOWS && HASIO)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_ASIO()            { return nullptr; }
#endif

#if ! (HLINUX && HALSA)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_ALSA()            { return nullptr; }
#endif

#if ! (HLINUX && HJACK)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_JACK()            { return nullptr; }
#endif

#if ! (HLINUX && HBELA)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_Bela()            { return nullptr; }
#endif

#if ! HANDROID
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_Android()         { return nullptr; }
#endif

#if ! (HANDROID && HUSE_ANDROID_OPENSLES)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_OpenSLES()        { return nullptr; }
#endif

#if ! (HANDROID && HUSE_ANDROID_OBOE)
AudioIODeviceType* AudioIODeviceType::createAudioIODeviceType_Oboe()            { return nullptr; }
#endif


