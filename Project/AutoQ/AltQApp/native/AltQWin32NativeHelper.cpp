#include "AltQNativeHelper.h"

#ifdef JUCE_WINDOWS

#include "windows.h"

bool Win10SetWindowTransparent(void* handle)
{
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (handle && hUser)
    {
        typedef enum _WINDOWCOMPOSITIONATTRIB
        {
            WCA_UNDEFINED = 0,
            WCA_NCRENDERING_ENABLED = 1,
            WCA_NCRENDERING_POLICY = 2,
            WCA_TRANSITIONS_FORCEDISABLED = 3,
            WCA_ALLOW_NCPAINT = 4,
            WCA_CAPTION_BUTTON_BOUNDS = 5,
            WCA_NONCLIENT_RTL_LAYOUT = 6,
            WCA_FORCE_ICONIC_REPRESENTATION = 7,
            WCA_EXTENDED_FRAME_BOUNDS = 8,
            WCA_HAS_ICONIC_BITMAP = 9,
            WCA_THEME_ATTRIBUTES = 10,
            WCA_NCRENDERING_EXILED = 11,
            WCA_NCADORNMENTINFO = 12,
            WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
            WCA_VIDEO_OVERLAY_ACTIVE = 14,
            WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
            WCA_DISALLOW_PEEK = 16,
            WCA_CLOAK = 17,
            WCA_CLOAKED = 18,
            WCA_ACCENT_POLICY = 19,
            WCA_FREEZE_REPRESENTATION = 20,
            WCA_EVER_UNCLOAKED = 21,
            WCA_VISUAL_OWNER = 22,
            WCA_LAST = 23
        } WINDOWCOMPOSITIONATTRIB;

        typedef struct _WINDOWCOMPOSITIONATTRIBDATA
        {
            WINDOWCOMPOSITIONATTRIB dwAttrib;
            PVOID pvData;
            SIZE_T cbData;
        } WINDOWCOMPOSITIONATTRIBDATA;

        typedef enum _ACCENT_STATE
        {
            ACCENT_DISABLED = 0,
            ACCENT_ENABLE_GRADIENT = 1,
            ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
            ACCENT_ENABLE_BLURBEHIND = 3,
            ACCENT_INVALID_STATE = 4
        } ACCENT_STATE;

        typedef struct _ACCENT_POLICY
        {
            ACCENT_STATE AccentState;
            DWORD AccentFlags;
            DWORD GradientColor;
            DWORD AnimationId;
        } ACCENT_POLICY;

        typedef BOOL(WINAPI*pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

        pfnSetWindowCompositionAttribute setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute)
        {
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.dwAttrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            return setWindowCompositionAttribute((HWND)handle, &data) == TRUE;
        }
    }
    return false;
}

bool Win7SetWindowTransparent(void* handle)
{
    // DwmEnableBlurBehindWindow
    HMODULE hUser = GetModuleHandleW(L"dwmapi.dll");
    if (handle && hUser)
    {
        typedef struct _DWM_BLURBEHIND
        {
            DWORD dwFlags;
            BOOL fEnable;
            HRGN hRgnBlur;
            BOOL fTransitionOnMaximized;
        } DWM_BLURBEHIND, *PDWM_BLURBEHIND;

        typedef HRESULT(*pfnDwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND*);
        pfnDwmEnableBlurBehindWindow dwmEnableBlurBehindWindow = (pfnDwmEnableBlurBehindWindow)GetProcAddress(hUser, "DwmEnableBlurBehindWindow");
        if (dwmEnableBlurBehindWindow)
        {
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = 0x00000001 | 0x00000002; // DWM_BB_ENABLE | DWM_BB_BLURREGION;
            bb.fEnable = true;
            bb.hRgnBlur = NULL;
            dwmEnableBlurBehindWindow((HWND)handle, &bb);
        }
    }
    return false;
}

bool AltQNative::SetWindowTransparent(void* handle)
{
    switch (juce::SystemStats::getOperatingSystemType())
    {
    case juce::SystemStats::Windows8_0:
    case juce::SystemStats::Windows8_1:
    case juce::SystemStats::Windows10:
        return Win10SetWindowTransparent(handle);
    case juce::SystemStats::Windows7:
    case juce::SystemStats::WinVista:
//        return Win7SetWindowTransparent(handle);
    case juce::SystemStats::Win2000:
    case juce::SystemStats::WinXP:
    default:
        break;
    }
    return false;
}

bool AltQNative::RegisterHotKey(void* handle, int id, unsigned int fsModifiers, unsigned int vk)
{
    if (!::RegisterHotKey((HWND)handle, id, fsModifiers, vk))
    {
        return false;
    }
    return true;
}

bool AltQNative::UnregisterHotKey(void* handle, int id)
{
    if (!::UnregisterHotKey((HWND)handle, id))
    {
        return false;
    }
    return true;
}

String AltQNative::GetStoragePath()
{
    File path(File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(ProjectInfo::projectName));
    path.createDirectory();

    return path.getFullPathName();
}

ApplicationProperties* AltQNative::GetAltQProperties()
{
    static ApplicationProperties* prop = nullptr;
    if (!prop)
    {
        prop = new ApplicationProperties;
        PropertiesFile::Options options;
        options.applicationName = ProjectInfo::projectName;
        options.filenameSuffix = ProjectInfo::settingFileName;
        options.folderName = GetStoragePath();
        prop->setStorageParameters(options);
    }
    return prop;
}

bool AltQNative::SetStartupBoot(bool s)
{
    String key("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    key << "\\";
    key << ProjectInfo::projectName;

    if (s)
    {
        return WindowsRegistry::setValue(key, File::getSpecialLocation(File::currentApplicationFile).getFullPathName());
    }

    WindowsRegistry::deleteValue(key);
    return true;
}

bool AltQNative::GetStartupBoot()
{
    String key("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    key << "\\";
    key << ProjectInfo::projectName;

    return WindowsRegistry::valueExists(key);
}

bool AltQNative::elevateToAdminRights(String commandLineArgs)
{
    
    wchar_t szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
    {
        // Re-launch ourselves with admin rights
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;
        sei.lpParameters = commandLineArgs.toWideCharPointer();
        if (!ShellExecuteEx(&sei))
        {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED)
            {
                // User refused to allow privileges elevation.
                return false;
            }
        }
        else
        {
            _exit(1);
        }
    }
    return false;
}
#endif