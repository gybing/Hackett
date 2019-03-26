#include "JuceHeader.h"
#include "CEFClient.h"
#include <sstream>
#include <string>
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_runnable.h"

CEFClientHandler::CEFClientHandler(CEFClientHandlerDelegate* de)
    : clientHandlerDelegate(de)
{
}

CEFClientHandler::~CEFClientHandler()
{
    clientHandlerDelegate = nullptr;
    browser_ = nullptr;
}

bool CEFClientHandler::createBrowser(HWND hWnd, const CefString& url)
{
    jassert(clientHandlerDelegate);

    CefWindowInfo info;
    info.SetAsWindowless(hWnd, false);
    CefBrowserSettings browserSettings;
    CefString(&browserSettings.default_encoding) = CefString("GBK");
    return CefBrowserHost::CreateBrowser(info, static_cast<CefRefPtr<CefClient> >(this), url, browserSettings, NULL);
}

void CEFClientHandler::closeBrowser(bool force)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&CEFClientHandler::closeBrowser, this, force));
        return;
    }

    if (browser_)
        browser_->GetHost()->CloseBrowser(force);
}

void CEFClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
    CEF_REQUIRE_UI_THREAD();

    browser_ = browser;

    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnAfterCreated();
}

void CEFClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (browser_)
        browser_ = nullptr;
}

void CEFClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   ErrorCode errorCode,
                                   const CefString& errorText,
                                   const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnLoadError(frame, errorCode, errorText, failedUrl);
}

void CEFClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnBeforeContextMenu(frame, params, model);
}


bool CEFClientHandler::RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->RunContextMenu(frame, params, model, callback);
    return false;
}

bool CEFClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
    CEF_REQUIRE_UI_THREAD();
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->OnContextMenuCommand(frame, params, command_id, event_flags);
    return false;
}

void CEFClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->OnContextMenuDismissed(frame);
}


//////////////////////////////////////////////////////////////////////////

// DisplayHandler
void CEFClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnAddressChange(frame, url);
}

void CEFClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnTitleChange(title);
}

void CEFClientHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnFaviconURLChange(icon_urls);
}

void CEFClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnFullscreenModeChange(fullscreen);
}

bool CEFClientHandler::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->OnTooltip(text);
    return false;
}

void CEFClientHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnStatusMessage(value);
}

bool CEFClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->OnConsoleMessage(message, source, line);
    return false;
}


// RenderHandler
bool CEFClientHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->GetRootScreenRect(rect);
    return false;
}

bool CEFClientHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->GetViewRect(rect);
    return false;
}

bool CEFClientHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->GetScreenPoint(viewX, viewY, screenX, screenY);
    return false;
}

bool CEFClientHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->GetScreenInfo(screen_info);
    return false;
}

void CEFClientHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnPopupShow(show);
}

void CEFClientHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnPopupSize(rect);
}

void CEFClientHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnPaint(type, dirtyRects, buffer, width, height);
}

void CEFClientHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnCursorChange(cursor, type, custom_cursor_info);
}

bool CEFClientHandler::StartDragging(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> drag_data, DragOperationsMask allowed_ops, int x, int y)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        return clientHandlerDelegate->StartDragging(drag_data, allowed_ops, x, y);
    return false;
}

void CEFClientHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser, DragOperation operation)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->UpdateDragCursor(operation);
}

void CEFClientHandler::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
    if (browser->IsSame(browser_) && clientHandlerDelegate)
        clientHandlerDelegate->OnScrollOffsetChanged(x, y);
}

CefRefPtr<CefBrowser> CEFClientHandler::GetBrowser()
{
    return browser_;
}

void CEFClientHandler::SetCookie(const wchar_t* domain, const wchar_t* key, const wchar_t* value)
{
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);  
    CefCookie cookie;  
    CefString(&cookie.name).FromWString(key);  
    CefString(&cookie.value).FromWString(value);  
    CefString(&cookie.domain).FromWString(domain);//www.magpcss.net//kagula-work-pc  
    CefString(&cookie.path).FromASCII("/");  
    cookie.has_expires = false;  

    std::wstring httpDomain = L"http://";  
    httpDomain.append(domain);

    CefPostTask(TID_IO, NewCefRunnableMethod(manager.get(), &CefCookieManager::SetCookie,  
        CefString(httpDomain.c_str()), cookie, nullptr));
}
