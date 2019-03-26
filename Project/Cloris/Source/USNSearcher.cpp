#include "USNSearcher.h"

juce_ImplementSingleton(USNSearcher);

USNSearcher::USNSearcher()
    : Thread(L"search_thread")
{
}

USNSearcher::~USNSearcher()
{
    for each (auto v in volumes)
    {
        v->stopThread(9999999);
    }
}

void USNSearcher::openNTFSVolume()
{
    Array<File> drives;
    File::findFileSystemRoots(drives);

    wchar_t szVolumeName[32];
    wchar_t fileSysBuf[8];
    juce::String szRootName, szVolumePath;

    for each (auto d in drives)
    {
        if (d.isOnHardDisk())
        {
            DWORD dwMaxComLen, dwFileSysFlag;
            GetVolumeInformationW(d.getFullPathName().toWideCharPointer(), szVolumeName, 32, NULL, &dwMaxComLen, &dwFileSysFlag, fileSysBuf, 8);
            if (fileSysBuf[0] == L'N' && fileSysBuf[1] == L'T' && fileSysBuf[2] == L'F' && fileSysBuf[3] == L'S')
            {
                wchar_t cDevice = d.getFullPathName().toWideCharPointer()[0];

                szRootName.clear();
                szVolumePath.clear();
                szRootName << szVolumeName << L"(" + cDevice << L")";
                szVolumePath << L"\\\\.\\" << cDevice << L":";

                HANDLE hVolume = CreateFileW(szVolumePath.toWideCharPointer(),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
                if (INVALID_HANDLE_VALUE == hVolume)
                {
                    juce::String error;
                    error << L"´´½¨¾í" << cDevice << L"Ê§°Ü: " << (int)GetLastError();
                    juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, L"´íÎó", error);
                    continue;
                }

                volumes.add(new USNVolume(hVolume));
            }
        }
    }
}

void USNSearcher::run()
{
    openNTFSVolume();

    for each (auto var in volumes)
    {
        var->startThread();
    }
}
