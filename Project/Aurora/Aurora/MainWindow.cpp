#include "stdafx.h"
#include "MainWindow.h"

MainAppWindow::MainAppWindow()
    : CXDocumentWindow (JUCEApplication::getInstance()->getApplicationName(), Colours::lightgrey, NULL)
{
    setUsingNativeTitleBar (false);
    setResizable (false, false);
    RECT rcTray;
    ::GetWindowRect(::FindWindow(L"Shell_TrayWnd", L""), &rcTray);
    setBounds (rcTray.left, rcTray.top-300, rcTray.right-rcTray.left, rcTray.bottom-rcTray.top);
    Component::addAndMakeVisible (m_pLiveDisplay = new LiveScrollingAudioDisplay());

    m_Capture.Initialize(m_pLiveDisplay);
    m_Capture.Start();
}

MainAppWindow::~MainAppWindow()
{
    if (m_Capture.IsInited())
    {
        if (m_Capture.IsCapturing())
            m_Capture.Stop();
        m_Capture.Destroy();
    }
    m_pLiveDisplay = nullptr;
}

void MainAppWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

VOID MainAppWindow::OnCatpureStart(DWORD dwInterval)
{

}

VOID MainAppWindow::OnCaptureStop()
{

}

VOID MainAppWindow::OnAdjustCaptureFormat(WAVEFORMATEX* pFormat)
{

}

VOID MainAppWindow::OnCatpureData(LPBYTE pData, INT nDataLen)
{
    wchar_t out[1024];
    wsprintf(out, L"OnCaptureData: %d \n", nDataLen);
    OutputDebugString(out);
}

void MainAppWindow::resized()
{
    if (m_pLiveDisplay)
    {
        m_pLiveDisplay->setBounds (0,0,getBounds().getWidth(), getBounds().getHeight());
    }
    
}