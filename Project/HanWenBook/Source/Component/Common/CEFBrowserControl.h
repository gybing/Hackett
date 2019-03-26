#ifndef JUCE_CEFBROWSERCONTROL_H_INCLUDED
#define JUCE_CEFBROWSERCONTROL_H_INCLUDED
#include "CEFClient/CEFClient.h"

class CEFBrowserControl
    : public juce::Component
    , public juce::TextInputTarget
    , public CEFClientHandlerDelegate
{
public:
    CEFBrowserControl();
    ~CEFBrowserControl();

    void goToURL (const juce::String& url, const juce::StringArray* headers = nullptr, const juce::MemoryBlock* postData = nullptr);
    static void SetCookie(const juce::String& domain, const juce::String& key, const juce::String& value);

protected:
    virtual void pageFinishedLoading (const juce::String&) {}
    virtual const wchar_t* getDefaultUrl() { return L""; }

    void checkWindowAssociation();
    int getEventModifiers(const juce::ModifierKeys& mods);

protected:
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;
    virtual void visibilityChanged() override;
    virtual void focusGained (FocusChangeType cause) override;
    virtual void focusLost (FocusChangeType cause) override;
    virtual void focusOfChildComponentChanged (FocusChangeType cause) override;
    void parentHierarchyChanged() override;

    // Mouse
    virtual void mouseMove (const juce::MouseEvent& event) override;
    virtual void mouseEnter (const juce::MouseEvent& event) override;
    virtual void mouseExit (const juce::MouseEvent& event) override;
    virtual void mouseDown (const juce::MouseEvent& event) override;
    virtual void mouseDrag (const juce::MouseEvent& event) override;
    virtual void mouseUp (const juce::MouseEvent& event) override;
    virtual void mouseDoubleClick (const juce::MouseEvent& event) override;
    virtual void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // KeyBoard
    virtual bool keyPressed (const juce::KeyPress& key) override;
    virtual bool keyStateChanged (bool isKeyDown) override;
    virtual void modifierKeysChanged (const juce::ModifierKeys& modifiers) override;

    // Input
    virtual bool isTextInputActive() const override;
    virtual juce::Range<int> getHighlightedRegion() const override;
    virtual void setHighlightedRegion (const juce::Range<int>& newRange) override;
    virtual void setTemporaryUnderlining (const juce::Array <juce::Range<int> >& underlinedRegions) override;
    virtual juce::String getTextInRange (const juce::Range<int>& range) const override;
    virtual void insertTextAtCaret (const juce::String& textToInsert) override;
    virtual juce::Rectangle<int> getCaretRectangle() override;

    //CefContextMenuHandler
    virtual void OnBeforeContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override; 
    virtual bool RunContextMenu(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) override; 
    virtual bool OnContextMenuCommand(CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags) override; 
    virtual void OnContextMenuDismissed(CefRefPtr<CefFrame> frame) override; 

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated() override; 
    virtual void OnBeforeClose() override; 

    // CefLoadHandler methods:
    virtual void OnLoadError(CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override; 

    // CEFClient DisplayHandler
    virtual void OnAddressChange(CefRefPtr<CefFrame> frame, const CefString& url) override; 

    // CEFClient RenderHandler
    virtual bool GetRootScreenRect(CefRect& rect) override;
    virtual bool GetViewRect(CefRect& rect) override;
    virtual bool GetScreenPoint(int viewX, int viewY, int& screenX, int& screenY) override;
    virtual bool GetScreenInfo(CefScreenInfo& screen_info) override;
    virtual void OnPopupShow(bool show) override;
    virtual void OnPopupSize(const CefRect& rect) override;
    virtual void OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override; 
    virtual void OnCursorChange(CefCursorHandle cursor, CefRenderHandler::CursorType type, const CefCursorInfo& custom_cursor_info) override; 

private:
    class AsyncRunContextMenu;
    class AsyncSetCursor;
    class AsyncUpdatePaint;
    CefRefPtr<CEFClientHandler> clientHandler;
    CefRefPtr<CefBrowserHost> browserHost;
    HWND browserHwnd;
    juce::Image imageToDraw;
    juce::String strGotoUrl;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CEFBrowserControl)
};

#endif
