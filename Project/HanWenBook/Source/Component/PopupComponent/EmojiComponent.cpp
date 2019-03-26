#include "JuceHeader.h"
#include "EmojiComponent.h"

//==============================================================================
namespace PopupEmojiSettings
{
    const int dismissCommandId = 0x6287345f;
    static bool menuWasHiddenBecauseOfAppChange = false;

    static const wchar_t* faceNames [] = {
        L"0000",L"0100",L"0200",L"0300",L"0400",L"0500",L"0600",L"0700",L"0800",L"0900",L"1000",L"1100",L"1200",L"1300",L"1400",
        L"0001",L"0101",L"0201",L"0301",L"0401",L"0501",L"0601",L"0701",L"0801",L"0901",L"1001",L"1101",L"1201",L"1301",L"1401",
        L"0002",L"0102",L"0202",L"0302",L"0402",L"0502",L"0602",L"0702",L"0802",L"0902",L"1002",L"1102",L"1202",L"1302",L"1402",
        L"0003",L"0103",L"0203",L"0303",L"0403",L"0503",L"0603",L"0703",L"0803",L"0903",L"1003",L"1103",L"1203",L"1303",L"1403",
        L"0004",L"0104",L"0204",L"0304",L"0404",L"0504",L"0604",L"0704",L"0804",L"0904",L"1004",L"1104",L"1204",L"1304",L"1404",
        L"0005",L"0105",L"0205",L"0305",L"0405",L"0505",L"0605",L"0705",L"0805",L"0905",L"1005",L"1105",L"1205",L"1305",L"1405",
        L"0006",L"0106",L"0206",L"0306",L"0406",L"0506",L"0606",L"0706",L"0806",L"0906",L"1006",L"1106",L"1206",L"1306",L"1406"
    };

    static const wchar_t* emojiText[] = {
        L"微笑",L"撇嘴",L"色",L"发呆",L"得意",L"流泪",L"害羞",L"闭嘴",L"睡",L"大哭",L"尴尬",L"发怒",L"调皮",L"呲牙",L"惊讶",
        L"难过",L"酷",L"冷汗",L"抓狂",L"吐",L"偷笑",L"可爱",L"白眼",L"傲慢",L"饥饿",L"困",L"惊恐",L"流汗",L"憨笑",L"大兵",
        L"奋斗",L"咒骂",L"疑问",L"嘘",L"晕",L"折磨",L"衰",L"骷髅",L"敲打",L"再见",L"擦汗",L"抠鼻",L"鼓掌",L"糗大了",L"坏笑",
        L"左哼哼",L"右哼哼",L"哈欠",L"鄙视",L"委屈",L"快哭了",L"阴险",L"亲亲",L"吓",L"可怜",L"菜刀",L"西瓜",L"啤酒",L"篮球",L"乒乓",
        L"咖啡",L"饭",L"猪头",L"玫瑰",L"凋谢",L"示爱",L"爱心",L"心碎",L"蛋糕",L"闪电",L"炸弹",L"刀",L"足球",L"瓢虫",L"便便",
        L"月亮",L"太阳",L"礼物",L"拥抱",L"强",L"弱",L"握手",L"胜利",L"抱拳",L"勾引",L"拳头",L"差劲",L"给力",L"NO",L"OK",
        L"干杯",L"飞吻",L"跳跳",L"发抖",L"怄火",L"转圈",L"磕头",L"回头",L"跳绳",L"挥手",L"激动",L"街舞",L"献吻",L"左太极",L"右太极"
    };
}

//==============================================================================
struct EmojiComponent::HelperClasses
{

class MouseSourceState;
class EmojiWindow;

//==============================================================================
class EmojiWindow  : public juce::Component
{
public:
    EmojiWindow (const EmojiComponent& emoji,
                const Options& opts,
                const int w, const int h,
                const int row, const int col,
                const bool alignToRectangle)
       : juce::Component ("emoji"),
         options (opts),
         componentAttachedTo (options.targetComponent),
         itemWidth(w),
         itemHeight(h),
         numColumns (col),
         numRows(row),
         windowCreationTime (juce::Time::getMillisecondCounter()),
         lastFocusedTime (windowCreationTime),
         timeEnteredCurrentChildComp (windowCreationTime)
    {
        setWantsKeyboardFocus (false);
        setMouseClickGrabsKeyboardFocus (false);
        setAlwaysOnTop (true);

        setOpaque (getLookAndFeel().findColour (EmojiComponent::backgroundColourId).isOpaque()
            || ! juce::Desktop::canUseSemiTransparentWindows());

        calculateWindowPos (options.targetArea, alignToRectangle);
        setTopLeftPosition (windowPos.getPosition());

        setBounds (windowPos);
        addToDesktop (juce::ComponentPeer::windowIsTemporary
                       | juce::ComponentPeer::windowIgnoresKeyPresses
                       | getLookAndFeel().getMenuWindowFlags());

        getActiveWindows().add (this);
        juce::Desktop::getInstance().addGlobalMouseListener (this);
    }

    ~EmojiWindow()
    {
        getActiveWindows().removeFirstMatchingValue (this);
        juce::Desktop::getInstance().removeGlobalMouseListener (this);
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        const int width = getWidth();
        const int height = getHeight();

        if (isOpaque())
            g.fillAll (juce::Colours::white);

        const juce::Colour background (findColour (EmojiComponent::backgroundColourId));

        g.fillAll (background);
        g.setColour (background.overlaidWith (juce::Colour (0x1badd8e6)));

        for (int i = 0; i < width; i += 3)
            g.fillRect (0, i, height, 1);

        g.setColour (findColour (EmojiComponent::lineColourId));
        // g.drawRect (0, 0, width, height);

        for (int x=0; x<numColumns; ++x)
        {
            const int pos = x * itemWidth;
            g.drawLine(pos, 0, pos, height, 1);
        }

        for (int y=0; y<numRows; ++y)
        {
            const int pos = y * itemHeight;
            g.drawLine(0, pos, width, pos, 1);
        }

        if (!options.imageEmojiBackground.isNull())
        {
            g.drawImage(options.imageEmojiBackground, 0, 0, width, height, 0, 0, options.imageEmojiBackground.getWidth(), options.imageEmojiBackground.getHeight());
        }

        if (currentEmoji >=0 && currentEmoji < (numColumns*numRows))
        {
            const juce::Rectangle<int> area(itemWidth * (currentEmoji % numColumns), itemHeight * (currentEmoji / numColumns), itemWidth, itemHeight);

            g.setColour(background);
            g.fillRect(area);
            g.setColour(findColour (EmojiComponent::highlineColourId));
            g.drawRect(area);
        }
    }

    //==============================================================================
    // hide this and all sub-comps
    void hide (const int item, const bool makeInvisible)
    {
        if (isVisible())
        {
            juce::WeakReference<juce::Component> deletionChecker (this);
            exitModalState (item);

            if (makeInvisible && (deletionChecker != nullptr))
                setVisible (false);
        }
    }

    void dismissEmoji (const int item)
    {
        hide (item, false);
    }

    //==============================================================================
    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key.isKeyCode (juce::KeyPress::downKey))
        {
            selectNextItem (1);
        }
        else if (key.isKeyCode (juce::KeyPress::upKey))
        {
            selectNextItem (-1);
        }
        else if (key.isKeyCode (juce::KeyPress::leftKey))
        {
            if (componentAttachedTo != nullptr)
            {
                componentAttachedTo->keyPressed (key);
            }
        }
        else if (key.isKeyCode (juce::KeyPress::rightKey))
        {
            if (componentAttachedTo != nullptr)
            {
                componentAttachedTo->keyPressed (key);
            }
        }
        else if (key.isKeyCode (juce::KeyPress::returnKey))
        {
            triggerCurrentlyHighlightedItem();
        }
        else if (key.isKeyCode (juce::KeyPress::escapeKey))
        {
            dismissEmoji (-1);
        }
        else
        {
            return false;
        }

        return true;
    }

    void inputAttemptWhenModal() override
    {
        juce::WeakReference<juce::Component> deletionChecker (this);

        const juce::Point<int> globalMousePos (juce::Desktop::getMousePosition());
        const juce::Point<int> localMousePos (getLocalPoint (nullptr, globalMousePos));

        juce::Component* c = getComponentAt (localMousePos);
        if (c == this)
            return;

        if (deletionChecker == nullptr)
            return;

        if (! isOverAnyEmoji())
        {
            if (componentAttachedTo != nullptr)
            {
                // we want to dismiss the menu, but if we do it synchronously, then
                // the mouse-click will be allowed to pass through. That's good, except
                // when the user clicks on the button that orginally popped the menu up,
                // as they'll expect the menu to go away, and in fact it'll just
                // come back. So only dismiss synchronously if they're not on the original
                // comp that we're attached to.
                const juce::Point<int> mousePos (componentAttachedTo->getMouseXYRelative());

                if (componentAttachedTo->reallyContains (mousePos, true))
                {
                    postCommandMessage (PopupEmojiSettings::dismissCommandId); // dismiss asynchrounously
                    return;
                }
            }

            dismissEmoji (-1);
        }
    }

    //==============================================================================
    void mouseDrag  (const juce::MouseEvent& e) override
    {
        mouseMove(e);
    }

    void mouseMove  (const juce::MouseEvent& e) override
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        juce::Point<int> pos = e2.getPosition();

        if (pos.x >= 0 && pos.x < getWidth() &&
            pos.y >= 0 && pos.y < getHeight())
        {
            const int x = pos.x / itemWidth;
            const int y = pos.y / itemHeight;

            if (x >= 0 && x < numColumns && y >=0 && y < numRows)
            {
                setCurrentlyHighlightedChild((y * numColumns) + x);
            }
        }
    }

    void mouseUp    (const juce::MouseEvent& e) override
    {
        if (currentEmoji != -1)
        {
            dismissEmoji (currentEmoji);
        }
    }

    void mouseExit(const juce::MouseEvent& event) override
    {
        setCurrentlyHighlightedChild(-1);
    }

    static juce::Array<EmojiWindow*>& getActiveWindows()
    {
        static juce::Array<EmojiWindow*> activeMenuWindows;
        return activeMenuWindows;
    }

    //==============================================================================
    bool isOverAnyEmoji() const
    {
        return isVisible() && isAnyMouseOver();
    }

    bool isAnyMouseOver() const
    {
        return (currentEmoji != -1) && (currentEmoji < (numColumns*numRows));
    }

    //==============================================================================
    void calculateWindowPos (const juce::Rectangle<int>& target, const bool alignToRectangle)
    {
        const juce::Rectangle<int> mon (juce::Desktop::getInstance().getDisplays()
                                     .getDisplayContaining (target.getCentre())
                                                           #if JUCE_MAC
                                                            .userArea);
                                                           #else
                                                            .totalArea); // on windows, don't stop the menu overlapping the taskbar
                                                           #endif

        const int widthToUse = itemWidth*numColumns;
        const int heightToUse = itemHeight*numRows;

        int x, y;

        if (alignToRectangle)
        {
            x = target.getX();

            const int spaceUnder = mon.getHeight() - (target.getBottom() - mon.getY());
            const int spaceOver = target.getY() - mon.getY();

            if (heightToUse < spaceUnder - 30 || spaceUnder >= spaceOver)
                y = target.getBottom();
            else
                y = target.getY() - heightToUse;
        }
        else
        {
            bool tendTowardsRight = target.getX() + widthToUse < mon.getWidth();
            const int biggestSpace = juce::jmax (mon.getRight() - target.getRight(),
                                           target.getX() - mon.getX()) - 32;

            if (biggestSpace < widthToUse)
            {
                tendTowardsRight = (mon.getRight() - target.getRight()) >= (target.getX() - mon.getX());
            }

            if (tendTowardsRight)
                x = juce::jmin (mon.getRight() - widthToUse - 4, target.getRight());
            else
                x = juce::jmax (mon.getX() + 4, target.getX() - widthToUse);

            y = target.getY();
            if (target.getY() + heightToUse > mon.getHeight())
                y = juce::jmax (mon.getY(), target.getBottom() - heightToUse);
        }

        x = juce::jmax (mon.getX() + 1, juce::jmin (mon.getRight() - (widthToUse + 6), x));
        y = juce::jmax (mon.getY() + 1, juce::jmin (mon.getBottom() - (heightToUse + 6), y));

        windowPos.setBounds (x, y, widthToUse, heightToUse);
    }

    void setCurrentlyHighlightedChild (int item)
    {
        if (currentEmoji != item)
        {
            currentEmoji = item;
            timeEnteredCurrentChildComp = juce::Time::getApproximateMillisecondCounter();

            if (currentEmoji >= 0 && currentEmoji < numColumns*numRows)
            {
                juce::String emoji = juce::File::getCurrentWorkingDirectory().getFullPathName();
                emoji << L"\\res\\face\\" << PopupEmojiSettings::faceNames[currentEmoji] << L".gif";
                componentCurrentEmoji = new juce::GIFComponent(emoji);

                const int x = itemWidth * (currentEmoji % numColumns);
                const int y = itemHeight * (currentEmoji / numColumns);
                componentCurrentEmoji->setBounds(x + 2, y + 2, 24, 24);
                componentCurrentEmoji->setWantsKeyboardFocus (false);
                componentCurrentEmoji->setMouseClickGrabsKeyboardFocus (false);
                componentCurrentEmoji->setInterceptsMouseClicks(false, false);
                addAndMakeVisible(componentCurrentEmoji);
            }
            else if (componentCurrentEmoji)
            {
                componentCurrentEmoji = nullptr;
            }
            repaint();
        }
    }

    void triggerCurrentlyHighlightedItem()
    {
        if (currentEmoji != -1)
        {
            dismissEmoji (currentEmoji);
        }
    }

    void selectNextItem (const int delta)
    {
        const int c = juce::jlimit(0, (numRows * numColumns - 1), currentEmoji + delta);
        setCurrentlyHighlightedChild(c);
    }

    //==============================================================================
    const Options options;
    juce::WeakReference<juce::Component> componentAttachedTo;
    juce::ScopedPointer<juce::GIFComponent> componentCurrentEmoji;
    juce::Rectangle<int> windowPos;
    int itemWidth, itemHeight, numColumns, numRows, currentEmoji;
    juce::uint32 windowCreationTime, lastFocusedTime, timeEnteredCurrentChildComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmojiWindow)
};

};

//==============================================================================
EmojiComponent::EmojiComponent()
{
}

EmojiComponent::~EmojiComponent()
{
}

//==============================================================================
EmojiComponent::Options::Options()
    : targetComponent (nullptr)
{
    targetArea.setPosition (juce::Desktop::getMousePosition());
    imageEmojiBackground = CXIMAGE_CACHE_LOAD(L"main_common_face_view_back.png");
}

EmojiComponent::Options EmojiComponent::Options::withTargetComponent (juce::Component* comp) const noexcept
{
    Options o (*this);
    o.targetComponent = comp;

    if (comp != nullptr)
        o.targetArea = comp->getScreenBounds();

    return o;
}

EmojiComponent::Options EmojiComponent::Options::withTargetScreenArea (const juce::Rectangle<int>& area) const noexcept
{
    Options o (*this);
    o.targetArea = area;
    return o;
}

juce::Component* EmojiComponent::createWindow (const Options& options) const
{
    return new HelperClasses::EmojiWindow (*this, options, 29, 29, 7, 15, ! options.targetArea.isEmpty());
}

//==============================================================================
// This invokes any command manager commands and deletes the menu window when it is dismissed
class PopupEmojiCompletionCallback  : public juce::ModalComponentManager::Callback
{
public:
    PopupEmojiCompletionCallback()
        : managerOfChosenCommand (nullptr),
          prevFocused (juce::Component::getCurrentlyFocusedComponent()),
          prevTopLevel (prevFocused != nullptr ? prevFocused->getTopLevelComponent() : nullptr)
    {
        PopupEmojiSettings::menuWasHiddenBecauseOfAppChange = false;
    }

    void modalStateFinished (int result)
    {
        if (managerOfChosenCommand != nullptr && result != 0)
        {
            juce::ApplicationCommandTarget::InvocationInfo info (result);
            info.invocationMethod = juce::ApplicationCommandTarget::InvocationInfo::fromMenu;

            managerOfChosenCommand->invoke (info, true);
        }

        // (this would be the place to fade out the component, if that's what's required)
        component = nullptr;

        if (! PopupEmojiSettings::menuWasHiddenBecauseOfAppChange)
        {
            if (prevTopLevel != nullptr)
                prevTopLevel->toFront (true);

            if (prevFocused != nullptr)
                prevFocused->grabKeyboardFocus();
        }
    }

    juce::ApplicationCommandManager* managerOfChosenCommand;
    juce::ScopedPointer<juce::Component> component;
    juce::WeakReference<juce::Component> prevFocused, prevTopLevel;

private:
    JUCE_DECLARE_NON_COPYABLE (PopupEmojiCompletionCallback)
};

int EmojiComponent::showWithOptionalCallback (const Options& options, juce::ModalComponentManager::Callback* const userCallback,
                                         const bool canBeModal)
{
    juce::ScopedPointer<juce::ModalComponentManager::Callback> userCallbackDeleter (userCallback);
    juce::ScopedPointer<PopupEmojiCompletionCallback> callback (new PopupEmojiCompletionCallback());

    if (juce::Component* window = createWindow (options))
    {
        callback->component = window;

        window->setVisible (true); // (must be called before enterModalState on Windows to avoid DropShadower confusion)
        window->enterModalState (false, userCallbackDeleter.release());
        juce::ModalComponentManager::getInstance()->attachCallback (window, callback.release());

        window->toFront (false);  // need to do this after making it modal, or it could
                                  // be stuck behind other comps that are already modal..

       #if JUCE_MODAL_LOOPS_PERMITTED
        if (userCallback == nullptr && canBeModal)
            return window->runModalLoop();
       #else
        (void) canBeModal;
        jassert (! (userCallback == nullptr && canBeModal));
       #endif
    }

    return 0;
}

bool JUCE_CALLTYPE EmojiComponent::dismissAllActiveEmojis()
{
    const juce::Array<HelperClasses::EmojiWindow*>& windows = HelperClasses::EmojiWindow::getActiveWindows();
    const int numWindows = windows.size();

    for (int i = numWindows; --i >= 0;)
        if (HelperClasses::EmojiWindow* const pmw = windows[i])
            pmw->dismissEmoji (-1);

    return numWindows > 0;
}

void EmojiComponent::showEmojiAsync(const Options& options, juce::ModalComponentManager::Callback* callback)
{
#if ! JUCE_MODAL_LOOPS_PERMITTED
    jassert (userCallback != nullptr);
#endif

    showWithOptionalCallback (options, callback, false);
}

juce::String EmojiComponent::getEmojiFormatName(int returnValue)
{
    if (returnValue > 0 && returnValue < 105)
    {
        return PopupEmojiSettings::emojiText[returnValue];
    }
}

int EmojiComponent::getEmojiIndex(const juce::String& e)
{
    for (unsigned int i=0; i<105; ++i)
    {
        if (e == PopupEmojiSettings::emojiText[i])
            return i;
    }
    return -1;
}
