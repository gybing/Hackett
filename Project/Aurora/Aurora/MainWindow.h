#pragma once
#include "AudioCapture/PlaybackAudioCapture.h"
#include "AudioLiveScrollingDisplay.h"

class MainAppWindow : public CXDocumentWindow
{
public:
    MainAppWindow();
    ~MainAppWindow();

    void closeButtonPressed() override;
    void resized() override;
    
protected:
    virtual VOID OnCatpureStart(DWORD dwInterval);
    virtual VOID OnCaptureStop();

    virtual VOID OnAdjustCaptureFormat(WAVEFORMATEX* pFormat);
    virtual VOID OnCatpureData(LPBYTE pData, INT nDataLen);

private:
    ScopedPointer<LiveScrollingAudioDisplay> m_pLiveDisplay;
    CPlaybackCapture m_Capture;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)
};
