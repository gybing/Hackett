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

extern int64 getMouseEventTime();

HDECLARE_UUID_GETTER (IOleObject,       "00000112-0000-0000-C000-000000000046")
HDECLARE_UUID_GETTER (IOleWindow,       "00000114-0000-0000-C000-000000000046")
HDECLARE_UUID_GETTER (IOleInPlaceSite,  "00000119-0000-0000-C000-000000000046")

namespace ActiveXHelpers
{
    //==============================================================================
    struct JuceIStorage   : public ComBaseClassHelper<IStorage>
    {
        JuceIStorage() {}

        HCOMRESULT CreateStream (const WCHAR*, DWORD, DWORD, DWORD, IStream**)           { return E_NOTIMPL; }
        HCOMRESULT OpenStream (const WCHAR*, void*, DWORD, DWORD, IStream**)             { return E_NOTIMPL; }
        HCOMRESULT CreateStorage (const WCHAR*, DWORD, DWORD, DWORD, IStorage**)         { return E_NOTIMPL; }
        HCOMRESULT OpenStorage (const WCHAR*, IStorage*, DWORD, SNB, DWORD, IStorage**)  { return E_NOTIMPL; }
        HCOMRESULT CopyTo (DWORD, IID const*, SNB, IStorage*)                            { return E_NOTIMPL; }
        HCOMRESULT MoveElementTo (const OLECHAR*,IStorage*, const OLECHAR*, DWORD)       { return E_NOTIMPL; }
        HCOMRESULT Commit (DWORD)                                                        { return E_NOTIMPL; }
        HCOMRESULT Revert()                                                              { return E_NOTIMPL; }
        HCOMRESULT EnumElements (DWORD, void*, DWORD, IEnumSTATSTG**)                    { return E_NOTIMPL; }
        HCOMRESULT DestroyElement (const OLECHAR*)                                       { return E_NOTIMPL; }
        HCOMRESULT RenameElement (const WCHAR*, const WCHAR*)                            { return E_NOTIMPL; }
        HCOMRESULT SetElementTimes (const WCHAR*, FILETIME const*, FILETIME const*, FILETIME const*)    { return E_NOTIMPL; }
        HCOMRESULT SetClass (REFCLSID)                                                   { return S_OK; }
        HCOMRESULT SetStateBits (DWORD, DWORD)                                           { return E_NOTIMPL; }
        HCOMRESULT Stat (STATSTG*, DWORD)                                                { return E_NOTIMPL; }
    };

    //==============================================================================
    struct JuceOleInPlaceFrame   : public ComBaseClassHelper<IOleInPlaceFrame>
    {
        JuceOleInPlaceFrame (HWND hwnd)   : window (hwnd) {}

        HCOMRESULT GetWindow (HWND* lphwnd)                                 { *lphwnd = window; return S_OK; }
        HCOMRESULT ContextSensitiveHelp (BOOL)                              { return E_NOTIMPL; }
        HCOMRESULT GetBorder (LPRECT)                                       { return E_NOTIMPL; }
        HCOMRESULT RequestBorderSpace (LPCBORDERWIDTHS)                     { return E_NOTIMPL; }
        HCOMRESULT SetBorderSpace (LPCBORDERWIDTHS)                         { return E_NOTIMPL; }
        HCOMRESULT SetActiveObject (IOleInPlaceActiveObject* a, LPCOLESTR)  { activeObject = a; return S_OK; }
        HCOMRESULT InsertMenus (HMENU, LPOLEMENUGROUPWIDTHS)                { return E_NOTIMPL; }
        HCOMRESULT SetMenu (HMENU, HOLEMENU, HWND)                          { return S_OK; }
        HCOMRESULT RemoveMenus (HMENU)                                      { return E_NOTIMPL; }
        HCOMRESULT SetStatusText (LPCOLESTR)                                { return S_OK; }
        HCOMRESULT EnableModeless (BOOL)                                    { return S_OK; }
        HCOMRESULT TranslateAccelerator (LPMSG, WORD)                       { return E_NOTIMPL; }

        HRESULT OfferKeyTranslation (LPMSG lpmsg)
        {
            if (activeObject != nullptr)
                return activeObject->TranslateAcceleratorW (lpmsg);

            return S_FALSE;
        }

        HWND window;
        ComSmartPtr<IOleInPlaceActiveObject> activeObject;
    };

    //==============================================================================
    struct JuceIOleInPlaceSite   : public ComBaseClassHelper<IOleInPlaceSite>
    {
        JuceIOleInPlaceSite (HWND hwnd)
            : window (hwnd),
              frame (new JuceOleInPlaceFrame (window))
        {}

        ~JuceIOleInPlaceSite()
        {
            frame->Release();
        }

        HCOMRESULT GetWindow (HWND* lphwnd)      { *lphwnd = window; return S_OK; }
        HCOMRESULT ContextSensitiveHelp (BOOL)   { return E_NOTIMPL; }
        HCOMRESULT CanInPlaceActivate()          { return S_OK; }
        HCOMRESULT OnInPlaceActivate()           { return S_OK; }
        HCOMRESULT OnUIActivate()                { return S_OK; }

        HCOMRESULT GetWindowContext (LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO lpFrameInfo)
        {
            /* Note: If you call AddRef on the frame here, then some types of object (e.g. web browser control) cause leaks..
               If you don't call AddRef then others crash (e.g. QuickTime).. Bit of a catch-22, so letting it leak is probably preferable.
            */
            if (lplpFrame != nullptr) { frame->AddRef(); *lplpFrame = frame; }
            if (lplpDoc != nullptr)   *lplpDoc = nullptr;
            lpFrameInfo->fMDIApp = FALSE;
            lpFrameInfo->hwndFrame = window;
            lpFrameInfo->haccel = 0;
            lpFrameInfo->cAccelEntries = 0;
            return S_OK;
        }

        HCOMRESULT Scroll (SIZE)                 { return E_NOTIMPL; }
        HCOMRESULT OnUIDeactivate (BOOL)         { return S_OK; }
        HCOMRESULT OnInPlaceDeactivate()         { return S_OK; }
        HCOMRESULT DiscardUndoState()            { return E_NOTIMPL; }
        HCOMRESULT DeactivateAndUndo()           { return E_NOTIMPL; }
        HCOMRESULT OnPosRectChange (LPCRECT)     { return S_OK; }

        LRESULT offerEventToActiveXControl (::MSG& msg)
        {
            if (frame != nullptr)
                return frame->OfferKeyTranslation (&msg);

            return S_FALSE;
        }

        HWND window;
        JuceOleInPlaceFrame* frame;
    };

    //==============================================================================
    struct JuceIOleClientSite  : public ComBaseClassHelper<IOleClientSite>
    {
        JuceIOleClientSite (HWND window)  : inplaceSite (new JuceIOleInPlaceSite (window))
        {}

        ~JuceIOleClientSite()
        {
            inplaceSite->Release();
        }

        HCOMRESULT QueryInterface (REFIID type, void** result)
        {
            if (type == __uuidof (IOleInPlaceSite))
            {
                inplaceSite->AddRef();
                *result = static_cast<IOleInPlaceSite*> (inplaceSite);
                return S_OK;
            }

            return ComBaseClassHelper <IOleClientSite>::QueryInterface (type, result);
        }

        HCOMRESULT SaveObject()                                  { return E_NOTIMPL; }
        HCOMRESULT GetMoniker (DWORD, DWORD, IMoniker**)         { return E_NOTIMPL; }
        HCOMRESULT GetContainer (LPOLECONTAINER* ppContainer)    { *ppContainer = nullptr; return E_NOINTERFACE; }
        HCOMRESULT ShowObject()                                  { return S_OK; }
        HCOMRESULT OnShowWindow (BOOL)                           { return E_NOTIMPL; }
        HCOMRESULT RequestNewObjectLayout()                      { return E_NOTIMPL; }

        LRESULT offerEventToActiveXControl (::MSG& msg)
        {
            if (inplaceSite != nullptr)
                return inplaceSite->offerEventToActiveXControl (msg);

            return S_FALSE;
        }

        JuceIOleInPlaceSite* inplaceSite;
    };

    //==============================================================================
    static Array<ActiveXControlComponent*> activeXComps;

    static inline HWND getHWND (const ActiveXControlComponent* const component)
    {
        HWND hwnd = {};
        const IID iid = __uuidof (IOleWindow);

        if (auto* window = (IOleWindow*) component->queryInterface (&iid))
        {
            window->GetWindow (&hwnd);
            window->Release();
        }

        return hwnd;
    }

    static inline void offerActiveXMouseEventToPeer (ComponentPeer* peer, HWND hwnd, UINT message, LPARAM lParam)
    {
        switch (message)
        {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            {
                RECT activeXRect, peerRect;
                GetWindowRect (hwnd, &activeXRect);
                GetWindowRect ((HWND) peer->getNativeHandle(), &peerRect);

                peer->handleMouseEvent (MouseInputSource::InputSourceType::mouse,
                                        { (float) (GET_X_LPARAM (lParam) + activeXRect.left - peerRect.left),
                                          (float) (GET_Y_LPARAM (lParam) + activeXRect.top  - peerRect.top) },
                                        ComponentPeer::getCurrentModifiersRealtime(),
                                        MouseInputSource::invalidPressure,
                                        MouseInputSource::invalidOrientation,
                                        getMouseEventTime());
                break;
            }

            default:
                break;
        }
    }
}

//==============================================================================
class ActiveXControlComponent::Pimpl  : public ComponentMovementWatcher
                                     #if HWIN_PER_MONITOR_DPI_AWARE
                                      , public ComponentPeer::ScaleFactorListener
                                     #endif
{
public:
    Pimpl (HWND hwnd, ActiveXControlComponent& activeXComp)
        : ComponentMovementWatcher (&activeXComp),
          owner (activeXComp),
          storage (new ActiveXHelpers::JuceIStorage()),
          clientSite (new ActiveXHelpers::JuceIOleClientSite (hwnd))
    {
    }

    ~Pimpl()
    {
        if (control != nullptr)
        {
            control->Close (OLECLOSE_NOSAVE);
            control->Release();
        }

        clientSite->Release();
        storage->Release();

       #if HWIN_PER_MONITOR_DPI_AWARE
        for (int i = 0; i < ComponentPeer::getNumPeers(); ++i)
            if (auto* peer = ComponentPeer::getPeer (i))
                peer->removeScaleFactorListener (this);
        #endif
    }

    void setControlBounds (Rectangle<int> newBounds) const
    {
        if (controlHWND != 0)
        {
           #if HWIN_PER_MONITOR_DPI_AWARE
            if (auto* peer = owner.getTopLevelComponent()->getPeer())
                newBounds = (newBounds.toDouble() * peer->getPlatformScaleFactor()).toNearestInt();
           #endif

            MoveWindow (controlHWND, newBounds.getX(), newBounds.getY(), newBounds.getWidth(), newBounds.getHeight(), TRUE);
        }

    }

    void setControlVisible (bool shouldBeVisible) const
    {
        if (controlHWND != 0)
            ShowWindow (controlHWND, shouldBeVisible ? SW_SHOWNA : SW_HIDE);
    }

    //==============================================================================
    void componentMovedOrResized (bool /*wasMoved*/, bool /*wasResized*/) override
    {
        if (auto* peer = owner.getTopLevelComponent()->getPeer())
            setControlBounds (peer->getAreaCoveredBy (owner));
    }

    void componentPeerChanged() override
    {
        componentMovedOrResized (true, true);

       #if HWIN_PER_MONITOR_DPI_AWARE
        if (auto* peer = owner.getTopLevelComponent()->getPeer())
            peer->addScaleFactorListener (this);
       #endif
    }

    void componentVisibilityChanged() override
    {
        setControlVisible (owner.isShowing());
        componentPeerChanged();
    }

   #if HWIN_PER_MONITOR_DPI_AWARE
    void nativeScaleFactorChanged (double /*newScaleFactor*/) override
    {
        componentMovedOrResized (true, true);
    }
   #endif

    // intercepts events going to an activeX control, so we can sneakily use the mouse events
    static LRESULT CALLBACK activeXHookWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        for (auto* ax : ActiveXHelpers::activeXComps)
        {
            if (ax->control != nullptr && ax->control->controlHWND == hwnd)
            {
                switch (message)
                {
                    case WM_MOUSEMOVE:
                    case WM_LBUTTONDOWN:
                    case WM_MBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_LBUTTONUP:
                    case WM_MBUTTONUP:
                    case WM_RBUTTONUP:
                    case WM_LBUTTONDBLCLK:
                    case WM_MBUTTONDBLCLK:
                    case WM_RBUTTONDBLCLK:
                        if (ax->isShowing())
                        {
                            if (auto* peer = ax->getPeer())
                            {
                                ActiveXHelpers::offerActiveXMouseEventToPeer (peer, hwnd, message, lParam);

                                if (! ax->areMouseEventsAllowed())
                                    return 0;
                            }
                        }
                        break;

                    default:
                        break;
                }

                return CallWindowProc (ax->control->originalWndProc, hwnd, message, wParam, lParam);
            }
        }

        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    ActiveXControlComponent& owner;
    HWND controlHWND = {};
    IStorage* storage = nullptr;
    ActiveXHelpers::JuceIOleClientSite* clientSite = nullptr;
    IOleObject* control = nullptr;
    WNDPROC originalWndProc = 0;
};

//==============================================================================
ActiveXControlComponent::ActiveXControlComponent()
{
    ActiveXHelpers::activeXComps.add (this);
}

ActiveXControlComponent::~ActiveXControlComponent()
{
    deleteControl();
    ActiveXHelpers::activeXComps.removeFirstMatchingValue (this);
}

void ActiveXControlComponent::paint (Graphics& g)
{
    if (control == nullptr)
        g.fillAll (Colours::lightgrey);
}

bool ActiveXControlComponent::createControl (const void* controlIID)
{
    deleteControl();

    if (auto* peer = getPeer())
    {
        auto controlBounds = peer->getAreaCoveredBy (*this);
        auto hwnd = (HWND) peer->getNativeHandle();

        std::unique_ptr<Pimpl> newControl (new Pimpl (hwnd, *this));

        HRESULT hr = OleCreate (*(const IID*) controlIID, __uuidof (IOleObject), 1 /*OLERENDER_DRAW*/, 0,
                                newControl->clientSite, newControl->storage,
                                (void**) &(newControl->control));

        if (hr == S_OK)
        {
            newControl->control->SetHostNames (L"H", 0);

            if (OleSetContainedObject (newControl->control, TRUE) == S_OK)
            {
                RECT rect;
                rect.left   = controlBounds.getX();
                rect.top    = controlBounds.getY();
                rect.right  = controlBounds.getRight();
                rect.bottom = controlBounds.getBottom();

                if (newControl->control->DoVerb (OLEIVERB_SHOW, 0, newControl->clientSite, 0, hwnd, &rect) == S_OK)
                {
                    control.reset (newControl.release());
                    control->controlHWND = ActiveXHelpers::getHWND (this);

                    if (control->controlHWND != 0)
                    {
                        control->setControlBounds (controlBounds);

                        control->originalWndProc = (WNDPROC) GetWindowLongPtr ((HWND) control->controlHWND, GWLP_WNDPROC);
                        SetWindowLongPtr ((HWND) control->controlHWND, GWLP_WNDPROC, (LONG_PTR) Pimpl::activeXHookWndProc);
                    }

                    return true;
                }
            }
        }
    }
    else
    {
        // the component must have already been added to a real window when you call this!
        HAssertfalse;
    }

    return false;
}

void ActiveXControlComponent::deleteControl()
{
    control = nullptr;
}

void* ActiveXControlComponent::queryInterface (const void* iid) const
{
    void* result = nullptr;

    if (control != nullptr && control->control != nullptr
         && SUCCEEDED (control->control->QueryInterface (*(const IID*) iid, &result)))
        return result;

    return nullptr;
}

void ActiveXControlComponent::setMouseEventsAllowed (const bool eventsCanReachControl)
{
    mouseEventsAllowed = eventsCanReachControl;
}

intptr_t ActiveXControlComponent::offerEventToActiveXControl (void* ptr)
{
    if (control != nullptr && control->clientSite != nullptr)
        return (intptr_t) control->clientSite->offerEventToActiveXControl (*reinterpret_cast<::MSG*> (ptr));

    return S_FALSE;
}

intptr_t ActiveXControlComponent::offerEventToActiveXControlStatic (void* ptr)
{
    for (auto* ax : ActiveXHelpers::activeXComps)
    {
        auto result = ax->offerEventToActiveXControl (ptr);

        if (result != S_FALSE)
            return result;
    }

    return S_FALSE;
}

LRESULT offerEventToActiveXControl (::MSG& msg)
{
    if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
        return ActiveXControlComponent::offerEventToActiveXControlStatic (&msg);

    return S_FALSE;
}


