#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../AXMusic/AXMusic.h"
#include "../AXDouban/AXDouban.h"
#include "ThumbComponent.h"
#include "../AXFFT/AXFFTComponent.h"

//==============================================================================
class MainContentComponent 
    : public Component
    , public HResponse
    , public juce::ComboBox::Listener
    , public Button::Listener
    , public ChangeListener
    , public Timer
    , public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

protected:
    // Component
    virtual void paint(Graphics& g) override;
    virtual void resized() override;

    // HResponse
    virtual void OnResponse(DouBanRequestID id, int err = 0, String message = String::empty) override;

    //
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    virtual void buttonClicked(Button*) override;
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;
    virtual void timerCallback() override;

    // slider
    virtual void sliderValueChanged(Slider* slider) override;
    virtual void sliderDragStarted(Slider*) override;
    virtual void sliderDragEnded(Slider*) override;

    bool prepareNextAudio();
    void reloadChannelsList();
    void updateSongInfo(const TDSong* song);
    void setEnableAction(bool enable);

private:
    //==============================================================================
    ScopedPointer<ComboBox> comboBox;
    ScopedPointer<ThumbComponent> thumbComponent;
    ScopedPointer<ImageButton> actionRemove;
    ScopedPointer<ImageButton> actionFavorite;
    ScopedPointer<ImageButton> actionNext;
    ScopedPointer<Slider> positionSlider;
    AXMusicPlayer musicPlayer;
    bool isPause;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
