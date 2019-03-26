#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include <list>

class CEFClientHandlerDelegate
{
public:
    // CefContextMenuHandler
    virtual void OnBeforeContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {} 
    virtual bool RunContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) { return false; } 
    virtual bool OnContextMenuCommand(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags) { return false; } 
    virtual void OnContextMenuDismissed(CefRefPtr<CefFrame> frame) {} 

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated() {}
    virtual void OnBeforeClose() {}

    // CefLoadHandler methods:
    virtual void OnLoadError(CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) {}

    // DisplayHandler
    virtual void OnAddressChange(CefRefPtr<CefFrame> frame, const CefString& url) {} 
    virtual void OnTitleChange(const CefString& title) {} 
    virtual void OnFaviconURLChange(const std::vector<CefString>& icon_urls) {} 
    virtual void OnFullscreenModeChange(bool fullscreen) {} 
    virtual bool OnTooltip(CefString& text) { return false; } 
    virtual void OnStatusMessage(const CefString& value) {}
    virtual bool OnConsoleMessage(const CefString& message, const CefString& source, int line) { return false; }

    // RenderHandler
    virtual bool GetRootScreenRect(CefRect& rect) { return false; }
    virtual bool GetViewRect(CefRect& rect) =0;
    virtual bool GetScreenPoint(int viewX, int viewY, int& screenX, int& screenY) { return false; }
    virtual bool GetScreenInfo(CefScreenInfo& screen_info) { return false; }
    virtual void OnPopupShow(bool show) {}
    virtual void OnPopupSize(const CefRect& rect) {}
    virtual void OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) =0;
    virtual void OnCursorChange(CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info) {} 
    virtual bool StartDragging(CefRefPtr<CefDragData> drag_data, CefRenderHandler::DragOperationsMask allowed_ops, int x, int y) { return false; }
    virtual void UpdateDragCursor(CefRenderHandler::DragOperation operation) {}
    virtual void OnScrollOffsetChanged(double x, double y) {}
};

class CEFClientHandler 
    : public CefClient
    , public CefLifeSpanHandler
    , public CefDisplayHandler
    , public CefLoadHandler
    , public CefRenderHandler
    , public CefContextMenuHandler
{
public:
    CEFClientHandler(CEFClientHandlerDelegate*);
    ~CEFClientHandler();

    void closeBrowser(bool force);
    bool createBrowser(HWND hWnd, const CefString& url);
    CefRefPtr<CefBrowser> GetBrowser();
    static void SetCookie(const wchar_t* domain, const wchar_t* key, const wchar_t* value);

protected:
    // CefClient methods:
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE { return this; }

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

    // CefContextMenuHandler
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
    virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) OVERRIDE;
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;

    // DisplayHandler
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
    virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) OVERRIDE;
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
    virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) OVERRIDE;
    virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;

    // RenderHandler
    virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
    virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) OVERRIDE;
    virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) OVERRIDE;
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) OVERRIDE;
    virtual void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) OVERRIDE;
    virtual bool StartDragging(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> drag_data, DragOperationsMask allowed_ops, int x, int y) OVERRIDE;
    virtual void UpdateDragCursor(CefRefPtr<CefBrowser> browser, DragOperation operation) OVERRIDE;
    virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) OVERRIDE;

private:
    CefRefPtr<CefBrowser> browser_;
    CEFClientHandlerDelegate* clientHandlerDelegate;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(CEFClientHandler);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CEFClientHandler)
};

#endif
