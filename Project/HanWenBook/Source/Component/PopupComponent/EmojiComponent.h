#ifndef JUCE_POPUPEMOJI_H_INCLUDED
#define JUCE_POPUPEMOJI_H_INCLUDED

//==============================================================================
/** Creates and displays a popup-emoji.

    To show a popup-emoji, call its show() method, which returns the id of the item the user selects.

    E.g. @code
    class EmojiCallback : public juce::ModalComponentManager::Callback
    {
    public:
        EmojiCallback(ReadInfoPostil& o) : owner(o) { }
        virtual void modalStateFinished (int returnValue) override
        {
            int a = returnValue;
        }
    private:
        ReadInfoPostil& owner;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmojiCallback)
    };
    EmojiComponent emoji;
    emoji.showEmojiAsync(EmojiComponent::Options(), new EmojiCallback(*this));
    @endcode
*/

class EmojiComponent
{
public:
    EmojiComponent();
    ~EmojiComponent();

    //==============================================================================
    class Options
    {
    public:
        Options();

        Options withTargetComponent (juce::Component* targetComponent) const noexcept;
        Options withTargetScreenArea (const juce::Rectangle<int>& targetArea) const noexcept;

    private:
        friend class EmojiComponent;
        juce::Rectangle<int> targetArea;
        juce::Component* targetComponent;
        juce::Image imageEmojiBackground;
        JUCE_LEAK_DETECTOR (Options)
    };

    void showEmojiAsync (const Options& options, juce::ModalComponentManager::Callback* callback);

    static bool JUCE_CALLTYPE dismissAllActiveEmojis();

    enum ColourIds
    {
        backgroundColourId             = 0x1110700,  /**< The colour to fill the menu's background with. */
        lineColourId                   = 0x1110600,
        highlineColourId               = 0x1110601,
    };

    static juce::String getEmojiFormatName(int returnValue);
    static int getEmojiIndex(const juce::String& e);

private:
    //==============================================================================
    JUCE_PUBLIC_IN_DLL_BUILD (struct HelperClasses)
    friend struct HelperClasses;

    juce::Component* createWindow (const Options&) const;
    int showWithOptionalCallback (const Options&, juce::ModalComponentManager::Callback*, bool);

    JUCE_LEAK_DETECTOR (EmojiComponent)
};

#endif   // JUCE_POPUPEMOJI_H_INCLUDED
