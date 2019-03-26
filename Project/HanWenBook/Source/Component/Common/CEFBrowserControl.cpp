#include "JuceHeader.h"
#include "CEFBrowserControl.h"

enum EnumClientMenuID {
    CLIENT_ID_SHOW_DEVTOOLS  = MENU_ID_USER_FIRST
};

class CEFBrowserControl::AsyncUpdatePaint : public juce::AsyncUpdater, public juce::DeletedAtShutdown
{
public:
    AsyncUpdatePaint(CEFBrowserControl& bc, juce::Image& im, const CefRenderHandler::RectList& rl) 
        : control(bc) , imageToDraw(im) , renderRectList(rl) {}

private:
    CEFBrowserControl& control;
    juce::Image imageToDraw;
    CefRenderHandler::RectList renderRectList;

    virtual void handleAsyncUpdate() override
    {
        control.imageToDraw = imageToDraw;

        juce::Rectangle<int> rectInvalied;
        if (!renderRectList.empty())
        {
            const CefRect& rc = renderRectList[0];
            rectInvalied.setBounds(rc.x, rc.y, rc.width, rc.height);

            for (unsigned int i=1; i<renderRectList.size(); ++i)
            {
                const CefRect& rc = renderRectList[0];

                rectInvalied.setBounds(
                    juce::jmin(rectInvalied.getX(),rc.x), 
                    juce::jmin(rectInvalied.getX(),rc.x), 
                    juce::jmax(rectInvalied.getWidth(),rc.width), 
                    juce::jmax(rectInvalied.getHeight(),rc.height));
            }
        }
        
        if (!rectInvalied.isEmpty())
        {
            control.repaint(rectInvalied);
        }
        else
        {
            control.repaint();
        }
        delete this;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AsyncUpdatePaint)
};

class CEFBrowserControl::AsyncSetCursor : public juce::AsyncUpdater, public juce::DeletedAtShutdown
{
public:
    AsyncSetCursor(CEFBrowserControl& bc, juce::MouseCursor::StandardCursorType cur) 
        : control(bc) , cursor(cur) {}

private:
    CEFBrowserControl& control;
    juce::MouseCursor::StandardCursorType cursor;

    virtual void handleAsyncUpdate() override
    {
        control.setMouseCursor(cursor);
        delete this;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AsyncSetCursor)

};

//==============================================================================
CEFBrowserControl::CEFBrowserControl()
    : browserHwnd(0)
{
    setWantsKeyboardFocus(true);
    checkWindowAssociation();
}

CEFBrowserControl::~CEFBrowserControl()
{
    if (clientHandler)
        clientHandler->closeBrowser(true);
}

void CEFBrowserControl::resized()
{
    if (browserHost)
    {
        browserHost->WasResized();
    }
}

void CEFBrowserControl::paint (juce::Graphics& g)
{
    if (imageToDraw.isNull())
    {
        g.fillAll(ProjectInfo::colorLightWhite);
    }
    else
    {
        const juce::Rectangle<int> rcClip = g.getClipBounds();
        g.drawImage(imageToDraw, rcClip.getX(), rcClip.getY(), rcClip.getWidth(), rcClip.getHeight(), rcClip.getX(), rcClip.getY(), rcClip.getWidth(), rcClip.getHeight(), false);
    }
}

void CEFBrowserControl::SetCookie(const juce::String& domain, const juce::String& key, const juce::String& value)
{
    CEFClientHandler::SetCookie(domain.toWideCharPointer(), key.toWideCharPointer(), value.toWideCharPointer());
}

void CEFBrowserControl::goToURL(const juce::String& url, const juce::StringArray* headers /*= nullptr*/, const juce::MemoryBlock* postData /*= nullptr*/)
{
    if (browserHost)
    {
        if (CefRefPtr<CefFrame> frame = clientHandler->GetBrowser()->GetMainFrame())
        {
            frame->LoadURL(url.isEmpty() ? L"" : url.toWideCharPointer());
            return;
        }
    }

    strGotoUrl = url;
}

void CEFBrowserControl::visibilityChanged()
{
    checkWindowAssociation();
}

void CEFBrowserControl::checkWindowAssociation()
{
    const bool isShow = isShowing();
    if (isShow && !clientHandler && getPeer())
    {
        if (juce::ComponentPeer* const peer = getPeer())
        {
            clientHandler = new CEFClientHandler(this);
            if (!clientHandler->createBrowser((HWND)peer->getNativeHandle(), getDefaultUrl()))
                jassert(false);
        }
    }

    if (browserHost)
    {
        browserHost->SetWindowVisibility(isShow);
    }
}

void CEFBrowserControl::parentHierarchyChanged()
{
    checkWindowAssociation();
}

// MouseEvent
int CEFBrowserControl::getEventModifiers(const juce::ModifierKeys& mods)
{
    int modifiers = 0;
    if (mods.isShiftDown())
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (mods.isCtrlDown())
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (mods.isAltDown())
        modifiers |= EVENTFLAG_ALT_DOWN;

    if (mods.isLeftButtonDown())
        modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (mods.isMiddleButtonDown())
        modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    if (mods.isRightButtonDown())
        modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;

    return modifiers;
}


void CEFBrowserControl::mouseDrag (const juce::MouseEvent& e)
{
    mouseMove(e);
}

void CEFBrowserControl::mouseMove (const juce::MouseEvent& e)
{
    if (browserHost)
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        CefMouseEvent mouse_event;
        mouse_event.x = e2.getPosition().getX();
        mouse_event.y = e2.getPosition().getY();
        mouse_event.modifiers = getEventModifiers(e.mods);
        browserHost->SendMouseMoveEvent(mouse_event, false);
    }
}

void CEFBrowserControl::mouseEnter (const juce::MouseEvent& e)
{
}

void CEFBrowserControl::mouseExit (const juce::MouseEvent& e)
{
}

void CEFBrowserControl::mouseDown (const juce::MouseEvent& e)
{
    if (browserHost)
    {
        // 设置焦点
        browserHost->SendFocusEvent(true);

        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        CefBrowserHost::MouseButtonType btnType = (e.mods.isLeftButtonDown() ? MBT_LEFT : (e2.mods.isRightButtonDown() ? MBT_RIGHT : MBT_MIDDLE));
        CefMouseEvent mouse_event;
        mouse_event.x = e2.getPosition().getX();
        mouse_event.y = e2.getPosition().getY();
        mouse_event.modifiers = getEventModifiers(e2.mods);
        browserHost->SendMouseClickEvent(mouse_event, btnType, false, e2.getNumberOfClicks());
    }
}

void CEFBrowserControl::mouseUp (const juce::MouseEvent& e)
{
    if (browserHost)
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        CefBrowserHost::MouseButtonType btnType = (e.mods.isLeftButtonDown() ? MBT_LEFT : (e2.mods.isRightButtonDown() ? MBT_RIGHT : MBT_MIDDLE));
        CefMouseEvent mouse_event;
        mouse_event.x = e2.getPosition().getX();
        mouse_event.y = e2.getPosition().getY();
        mouse_event.modifiers = getEventModifiers(e2.mods);
        browserHost->SendMouseClickEvent(mouse_event, btnType, true, e2.getNumberOfClicks());
    }
}

void CEFBrowserControl::mouseDoubleClick (const juce::MouseEvent& e)
{
    if (browserHost)
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        CefBrowserHost::MouseButtonType btnType = (e2.mods.isLeftButtonDown() ? MBT_LEFT : (e2.mods.isRightButtonDown() ? MBT_RIGHT : MBT_MIDDLE));
        CefMouseEvent mouse_event;
        mouse_event.x = e2.getPosition().getX();
        mouse_event.y = e2.getPosition().getY();
        mouse_event.modifiers = getEventModifiers(e2.mods);
        browserHost->SendMouseClickEvent(mouse_event, btnType, true, e2.getNumberOfClicks());
    }
}

void CEFBrowserControl::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (browserHost)
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        CefMouseEvent mouse_event;
        mouse_event.x = e2.getPosition().getX();
        mouse_event.y = e2.getPosition().getY();
        mouse_event.modifiers = getEventModifiers(e2.mods);
        browserHost->SendMouseWheelEvent(mouse_event, e2.mods.isShiftDown() ? (wheel.deltaX*500.0f) : 0, !e2.mods.isShiftDown() ? (wheel.deltaY*500.0f) : 0);
    }
}

bool CEFBrowserControl::keyPressed(const juce::KeyPress& key)
{
    if (browserHost)
    {
        if (key.isKeyCode(juce::KeyPress::F5Key))
        {
            clientHandler->GetBrowser()->Reload();
            return true;
        }

        CefKeyEvent event;
        event.is_system_key = false;
        event.modifiers = getEventModifiers(key.getModifiers());
        event.native_key_code = key.getKeyCode() & ~0x10000;

        const juce::juce_wchar ch = key.getTextCharacter();
        if (ch > 0 && 
            !key.isKeyCode(juce::KeyPress::backspaceKey) &&
            !key.isKeyCode(juce::KeyPress::tabKey))
        {
            event.type = KEYEVENT_CHAR;
            event.windows_key_code = ch;
        }
        else
        {
            event.type = KEYEVENT_KEYDOWN;
            event.windows_key_code = key.getKeyCode() & ~0x10000;
        }

        browserHost->SendKeyEvent(event);
        return true;
    }
    return false;
}

bool CEFBrowserControl::keyStateChanged(bool isKeyDown)
{
    if (!isKeyDown && browserHost)
    {
        CefKeyEvent event;
        event.is_system_key = false;
        event.modifiers = getEventModifiers(juce::ModifierKeys::getCurrentModifiers());
        event.native_key_code = 0;
        event.windows_key_code = 0;
        event.type = KEYEVENT_KEYUP;
        browserHost->SendKeyEvent(event);
        return true;
    }
    return false;
}

void CEFBrowserControl::modifierKeysChanged(const juce::ModifierKeys& modifiers)
{
}

void CEFBrowserControl::focusGained(FocusChangeType cause)
{
}

void CEFBrowserControl::focusLost(FocusChangeType cause)
{
    if (browserHost)
    {
        browserHost->SendFocusEvent(false);
    }
}

void CEFBrowserControl::focusOfChildComponentChanged(FocusChangeType cause)
{
}

// Input
bool CEFBrowserControl::isTextInputActive() const
{
    return browserHost != nullptr;
}

juce::Range<int> CEFBrowserControl::getHighlightedRegion() const
{
    return juce::Range<int>();
}

void CEFBrowserControl::setHighlightedRegion (const juce::Range<int>& newRange)
{

}

void CEFBrowserControl::setTemporaryUnderlining (const juce::Array <juce::Range<int> >& underlinedRegions)
{

}

juce::String CEFBrowserControl::getTextInRange (const juce::Range<int>& range) const
{
    return juce::String();
}

void CEFBrowserControl::insertTextAtCaret (const juce::String& textToInsert)
{
    if (browserHost && textToInsert.isNotEmpty() && textToInsert != L" ")
    {
        for (int i=0; i<textToInsert.length(); ++i)
        {
            CefKeyEvent keyEvent;
            keyEvent.windows_key_code = textToInsert[i];
            keyEvent.is_system_key = false;
            keyEvent.type = KEYEVENT_CHAR;
            browserHost->SendKeyEvent(keyEvent);
        }
    }
}

juce::Rectangle<int> CEFBrowserControl::getCaretRectangle()
{
    void* conext = browserHost->GetNSTextInputContext();
    if (conext)
    {
        jassert(false);
    }
    return juce::Rectangle<int>();
}

//CefContextMenuHandler
void CEFBrowserControl::OnBeforeContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
    
}

class CEFBrowserControl::AsyncRunContextMenu : public juce::AsyncUpdater, public juce::DeletedAtShutdown
{
public:
    AsyncRunContextMenu(CefRefPtr<CefBrowser> b, juce::PopupMenu* m)
        : browser(b), menu(m)
    {}

protected:

    virtual void handleAsyncUpdate() override
    {
        menu->showMenuAsync(juce::PopupMenu::Options().withStandardItemHeight(21), new CEFBrowserPopupMenuCallBack(browser));
    }

    class CEFBrowserPopupMenuCallBack : public juce::ModalComponentManager::Callback
    {
    public:
        explicit CEFBrowserPopupMenuCallBack(CefRefPtr<CefBrowser> br) : browser(br) { }

    private:
        virtual void modalStateFinished (int returnValue) override
        {
            switch (returnValue)
            {
            case MENU_ID_BACK: browser->GoBack(); break;
            case MENU_ID_FORWARD: browser->GoForward(); break;
            case MENU_ID_RELOAD: browser->Reload(); break;
            case MENU_ID_RELOAD_NOCACHE: browser->ReloadIgnoreCache(); break;
            case MENU_ID_STOPLOAD: browser->StopLoad(); break;

                // Editing.
            case MENU_ID_UNDO: browser->GetFocusedFrame()->Undo(); break;
            case MENU_ID_REDO: browser->GetFocusedFrame()->Redo(); break;
            case MENU_ID_CUT: browser->GetFocusedFrame()->Cut(); break;
            case MENU_ID_COPY: browser->GetFocusedFrame()->Copy(); break;
            case MENU_ID_PASTE: browser->GetFocusedFrame()->Paste(); break;
            case MENU_ID_DELETE: browser->GetFocusedFrame()->Delete(); break;
            case MENU_ID_SELECT_ALL: browser->GetFocusedFrame()->SelectAll(); break;

                // Miscellaneous.
            case MENU_ID_FIND: browser->GetHost()->Find(browser->GetFocusedFrame()->GetIdentifier(), CefString(), false, false, false); break;
            case MENU_ID_PRINT: browser->GetHost()->Print(); break;
            case MENU_ID_VIEW_SOURCE: browser->GetFocusedFrame()->ViewSource(); break;
            case CLIENT_ID_SHOW_DEVTOOLS:
                {
                    CefWindowInfo info;
                    info.SetAsPopup(NULL, L"开发者工具");
                    CefBrowserSettings setting;
                    CefString(&setting.default_encoding) = CefString("GBK");
                    browser->GetHost()->ShowDevTools(info, new CEFClientHandler(nullptr), setting, CefPoint());
                    break;
                }
            default:
                break;
            }
        }

        CefRefPtr<CefBrowser> browser;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CEFBrowserPopupMenuCallBack)
    };
private:
    CefRefPtr<CefBrowser> browser;
    juce::ScopedPointer<juce::PopupMenu> menu;
};

bool CEFBrowserControl::RunContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback)
{
    // 添加 重新加载 审查元素
    if (!(params->GetTypeFlags() & CM_TYPEFLAG_EDITABLE))
    {
        if (model->GetCount() > 0) 
            model->AddSeparator();

        for (int i=0; i<model->GetCount(); ++i)
        {
            if (model->GetCommandIdAt(i) == -1)
            {
                model->InsertItemAt(i, MENU_ID_RELOAD, L"重新加载");
                break;
            }
        }

        model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, L"审查元素");
    }

    juce::PopupMenu* m = new juce::PopupMenu;

    for (int i=0; i<model->GetCount(); ++i)
    {
        const int id = model->GetCommandIdAt(i);
        switch (id)
        {
        case -1: m->addSeparator(); break;
        case MENU_ID_BACK: m->addItem(model->GetCommandIdAt(i), L"返回", frame->GetBrowser()->CanGoBack()); break;
        case MENU_ID_FORWARD: m->addItem(model->GetCommandIdAt(i), L"前进", frame->GetBrowser()->CanGoForward()); break;
        case MENU_ID_RELOAD: m->addItem(model->GetCommandIdAt(i), L"重新加载"); break;
        case MENU_ID_RELOAD_NOCACHE: m->addItem(model->GetCommandIdAt(i), L"全部重新加载"); break;
        case MENU_ID_STOPLOAD: m->addItem(model->GetCommandIdAt(i), L"停止加载"); break;

            // Editing.
        case MENU_ID_UNDO: m->addItem(model->GetCommandIdAt(i), L"撤销"); break;
        case MENU_ID_REDO: m->addItem(model->GetCommandIdAt(i), L"重做"); break;
        case MENU_ID_CUT: m->addItem(model->GetCommandIdAt(i), L"剪切"); break;
        case MENU_ID_COPY: m->addItem(model->GetCommandIdAt(i), L"复制"); break;
        case MENU_ID_PASTE: m->addItem(model->GetCommandIdAt(i), L"粘贴"); break;
        case MENU_ID_DELETE: m->addItem(model->GetCommandIdAt(i), L"删除"); break;
        case MENU_ID_SELECT_ALL: m->addItem(model->GetCommandIdAt(i), L"全选"); break;

            // Miscellaneous.
        case MENU_ID_FIND: m->addItem(model->GetCommandIdAt(i), L"查找"); break;
        default:
            break;
        }

        if (CXOption::getInstance()->getApplicationDebugMode())
        {
            switch (id)
            {
            case MENU_ID_FIND: m->addItem(model->GetCommandIdAt(i), L"查找"); break;
            case MENU_ID_PRINT: m->addItem(model->GetCommandIdAt(i), L"打印"); break;
            case MENU_ID_VIEW_SOURCE: m->addItem(model->GetCommandIdAt(i), L"查看源代码"); break;
            case CLIENT_ID_SHOW_DEVTOOLS: m->addItem(model->GetCommandIdAt(i), L"审查元素"); break;
            default:
                break;
            }
        }
    }

    (new AsyncRunContextMenu(frame->GetBrowser(), m))->triggerAsyncUpdate();
    return true;
}

bool CEFBrowserControl::OnContextMenuCommand(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
    return false;
}

void CEFBrowserControl::OnContextMenuDismissed(CefRefPtr<CefFrame> frame)
{

}

// CefLifeSpanHandler methods:
void CEFBrowserControl::OnAfterCreated()
{
    if (CefRefPtr<CefBrowser> br = clientHandler->GetBrowser())
    {
        browserHost = br->GetHost();
        browserHwnd = browserHost->GetWindowHandle();

        if (strGotoUrl.isNotEmpty())
        {
            juce::String url = strGotoUrl;
            strGotoUrl.clear();

            goToURL(url);
        }        
    }
}

void CEFBrowserControl::OnBeforeClose()
{
    browserHwnd = nullptr;
    browserHost = nullptr;
}

// CefLoadHandler methods:
void CEFBrowserControl::OnLoadError(CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
        "<h2>Server error.</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

void CEFBrowserControl::OnAddressChange(CefRefPtr<CefFrame> frame, const CefString& url)
{
    pageFinishedLoading(juce::String(url.c_str()));
} 

// CEFClient RenderHandler
bool CEFBrowserControl::GetRootScreenRect(CefRect& rect)
{
    if (juce::ComponentPeer* const peer = getPeer())
    {
        const juce::Rectangle<int> bounds (peer->getBounds());
        rect.Set(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
        return true;
    }

    return false;
}

bool CEFBrowserControl::GetViewRect(CefRect& rect)
{
    if (juce::ComponentPeer* const peer = getPeer())
    {
        const juce::Rectangle<int> bounds (peer->getAreaCoveredBy (*this));
        rect.Set(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
        return true;
    }

    return false;
}

bool CEFBrowserControl::GetScreenPoint(int viewX, int viewY, int& screenX, int& screenY)
{
    if (juce::ComponentPeer* const peer = getPeer())
    {
        const juce::Rectangle<int> peerBounds (peer->getBounds());
        const juce::Rectangle<int> bounds (peer->getAreaCoveredBy (*this));
        screenX = viewX+bounds.getX()+peerBounds.getX();
        screenY = viewY+bounds.getY()+peerBounds.getY();
        return true;
    }

    return false;
}

bool CEFBrowserControl::GetScreenInfo(CefScreenInfo& screen_info)
{
    if (!::IsWindow(browserHwnd))
        return false;

    CefRect rect;
    GetViewRect(rect);

    screen_info.device_scale_factor = juce::Desktop::getInstance().getGlobalScaleFactor();
    // The screen info rectangles are used by the renderer to create and position
    // popups. Keep popups inside the view rectangle.
    screen_info.rect = rect;
    screen_info.available_rect = rect;
    return true;
}

void CEFBrowserControl::OnPopupShow(bool show)
{
    
}

void CEFBrowserControl::OnPopupSize(const CefRect& rect)
{

}

void CEFBrowserControl::OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    juce::Image image = juce::Image (juce::Image::ARGB, (int) width, (int) height, true);
    const juce::Image::BitmapData destData (image, juce::Image::BitmapData::writeOnly);
    unsigned int sz = width*height*4;
    juce::uint8* dest = destData.getLinePointer (0);
    memcpy_s(dest, sz, buffer, sz);

    AsyncUpdatePaint* rp = new AsyncUpdatePaint(*this, image, dirtyRects);
    rp->triggerAsyncUpdate();
} 

void CEFBrowserControl::OnCursorChange(CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info)
{
    if (browserHost)
    {
        juce::MouseCursor::StandardCursorType cur(juce::MouseCursor::NormalCursor);
        switch (type)
        {
        case CT_HAND: cur = juce::MouseCursor::PointingHandCursor; break;
        case CT_IBEAM: cur = juce::MouseCursor::IBeamCursor; break;
        default:
            break;
        }

        (new AsyncSetCursor(*this, cur))->triggerAsyncUpdate();
    }
} 
