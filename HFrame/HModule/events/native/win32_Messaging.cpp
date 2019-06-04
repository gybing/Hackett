

extern HWND messageWindowHandle;

using CheckEventBlockedByModalComps = bool (*)(const MSG&);
CheckEventBlockedByModalComps isEventBlockedByModalComps = nullptr;

using SettingChangeCallbackFunc = void (*)(void);
SettingChangeCallbackFunc settingChangeCallback = nullptr;

#if HMODULE_AVAILABLE_audio_plugin_client && JucePlugin_Build_Unity
 bool isRunningInUnity();
#endif

//==============================================================================
namespace WindowsMessageHelpers
{
    const unsigned int customMessageID = WM_USER + 123;
    const unsigned int broadcastMessageMagicNumber = 0xc403;

    const TCHAR messageWindowName[] = _T("HWindow");
    std::unique_ptr<HiddenMessageWindow> messageWindow;

    void dispatchMessageFromLParam (LPARAM lParam)
    {
        if (auto message = reinterpret_cast<MessageManager::MessageBase*> (lParam))
        {
            HTRY
            {
                message->messageCallback();
            }
            HCATCH_EXCEPTION

            message->decReferenceCount();
        }
    }

    BOOL CALLBACK broadcastEnumWindowProc (HWND hwnd, LPARAM lParam)
    {
        if (hwnd != messageWindowHandle)
        {
            TCHAR windowName[64] = { 0 }; // no need to read longer strings than this
            GetWindowText (hwnd, windowName, 63);

            if (String (windowName) == messageWindowName)
                reinterpret_cast<Array<HWND>*> (lParam)->add (hwnd);
        }

        return TRUE;
    }

    void handleBroadcastMessage (const COPYDATASTRUCT* data)
    {
        if (data != nullptr && data->dwData == broadcastMessageMagicNumber)
        {
            struct BroadcastMessage  : public CallbackMessage
            {
                BroadcastMessage (CharPointer_UTF32 text, size_t length) : message (text, length) {}
                void messageCallback() override { MessageManager::getInstance()->deliverBroadcastMessage (message); }

                String message;
            };

            (new BroadcastMessage (CharPointer_UTF32 ((const CharPointer_UTF32::CharType*) data->lpData),
                                   data->cbData / sizeof (CharPointer_UTF32::CharType)))
                ->post();
        }
    }

    //==============================================================================
    LRESULT CALLBACK messageWndProc (HWND h, UINT message, WPARAM wParam, LPARAM lParam) noexcept
    {
        if (h == messageWindowHandle)
        {
            if (message == customMessageID)
            {
                // (These are trapped early in our dispatch loop, but must also be checked
                // here in case some 3rd-party code is running the dispatch loop).
                dispatchMessageFromLParam (lParam);
                return 0;
            }

            if (message == WM_COPYDATA)
            {
                handleBroadcastMessage (reinterpret_cast<const COPYDATASTRUCT*> (lParam));
                return 0;
            }

            if (message == WM_SETTINGCHANGE)
                if (settingChangeCallback != nullptr)
                    settingChangeCallback();
        }

        return DefWindowProc (h, message, wParam, lParam);
    }
}

LRESULT offerEventToActiveXControl (::MSG&);

//==============================================================================
bool MessageManager::dispatchNextMessageOnSystemQueue (bool returnIfNoPendingMessages)
{
    using namespace WindowsMessageHelpers;
    MSG m;

    if (returnIfNoPendingMessages && ! PeekMessage (&m, (HWND) 0, 0, 0, PM_NOREMOVE))
        return false;

    if (GetMessage (&m, (HWND) 0, 0, 0) >= 0)
    {
        if (offerEventToActiveXControl (m) != S_FALSE)
            return true;

        if (m.message == customMessageID && m.hwnd == messageWindowHandle)
        {
            dispatchMessageFromLParam (m.lParam);
        }
        else if (m.message == WM_QUIT)
        {
            if (auto* app = HApplicationBase::getInstance())
                app->systemRequestedQuit();
        }
        else if (isEventBlockedByModalComps == nullptr || ! isEventBlockedByModalComps (m))
        {
            if ((m.message == WM_LBUTTONDOWN || m.message == WM_RBUTTONDOWN)
                  && ! JuceWindowIdentifier::isHWindow (m.hwnd))
            {
                // if it's someone else's window being clicked on, and the focus is
                // currently on a H window, pass the kb focus over..
                auto currentFocus = GetFocus();

                if (currentFocus == 0 || JuceWindowIdentifier::isHWindow (currentFocus))
                    SetFocus (m.hwnd);
            }

            TranslateMessage (&m);
            DispatchMessage (&m);
        }
    }

    return true;
}

bool MessageManager::postMessageToSystemQueue (MessageManager::MessageBase* const message)
{
    message->incReferenceCount();

   #if HMODULE_AVAILABLE_audio_plugin_client && JucePlugin_Build_Unity
    if (isRunningInUnity())
        return SendNotifyMessage (messageWindowHandle, WindowsMessageHelpers::customMessageID, 0, (LPARAM) message) != 0;
   #endif

    return PostMessage (messageWindowHandle, WindowsMessageHelpers::customMessageID, 0, (LPARAM) message) != 0;
}

void MessageManager::broadcastMessage (const String& value)
{
    auto localCopy = value;

    Array<HWND> windows;
    EnumWindows (&WindowsMessageHelpers::broadcastEnumWindowProc, (LPARAM) &windows);

    for (int i = windows.size(); --i >= 0;)
    {
        COPYDATASTRUCT data;
        data.dwData = WindowsMessageHelpers::broadcastMessageMagicNumber;
        data.cbData = (localCopy.length() + 1) * sizeof (CharPointer_UTF32::CharType);
        data.lpData = (void*) localCopy.toUTF32().getAddress();

        DWORD_PTR result;
        SendMessageTimeout (windows.getUnchecked (i), WM_COPYDATA,
                            (WPARAM) messageWindowHandle,
                            (LPARAM) &data,
                            SMTO_BLOCK | SMTO_ABORTIFHUNG, 8000, &result);
    }
}

//==============================================================================
void MessageManager::doPlatformSpecificInitialisation()
{
    OleInitialize (0);

    using namespace WindowsMessageHelpers;
    messageWindow.reset (new HiddenMessageWindow (messageWindowName, (WNDPROC) messageWndProc));
    messageWindowHandle = messageWindow->getHWND();
}

void MessageManager::doPlatformSpecificShutdown()
{
    WindowsMessageHelpers::messageWindow = nullptr;

    OleUninitialize();
}

//==============================================================================
struct MountedVolumeListChangeDetector::Pimpl   : private DeviceChangeDetector
{
    Pimpl (MountedVolumeListChangeDetector& d) : DeviceChangeDetector (L"MountedVolumeList"), owner (d)
    {
        File::findFileSystemRoots (lastVolumeList);
    }

    void systemDeviceChanged() override
    {
        Array<File> newList;
        File::findFileSystemRoots (newList);

        if (lastVolumeList != newList)
        {
            lastVolumeList = newList;
            owner.mountedVolumeListChanged();
        }
    }

    MountedVolumeListChangeDetector& owner;
    Array<File> lastVolumeList;
};

MountedVolumeListChangeDetector::MountedVolumeListChangeDetector()  { pimpl.reset (new Pimpl (*this)); }
MountedVolumeListChangeDetector::~MountedVolumeListChangeDetector() {}


