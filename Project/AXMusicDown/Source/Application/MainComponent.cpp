#include "MainComponent.h"

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }

//
MainContentComponent::MainContentComponent()
{
    setInterceptsMouseClicks(false, true);

    addAndMakeVisible(comboBox = new ComboBox);
    comboBox->setEditableText(false);
    comboBox->setJustificationType(Justification::centred);
    comboBox->addListener(this);
    addAndMakeVisible(thumbComponent = new ThumbComponent(AXLoadImage(thumb_default_png)));
    thumbComponent->addListener(this);
    thumbComponent->setActionImage(AXLoadImage(action_play_png));

    addAndMakeVisible(actionRemove = new ImageButton());
    actionRemove->addListener(this);

    actionRemove->setImages(false, true, false,
        AXLoadImage(remove_normal_png), 1.0f, Colours::transparentBlack,
        AXLoadImage(remove_press_png), 0.9f, Colours::transparentBlack,
        AXLoadImage(remove_press_png), 1.0f, Colours::transparentBlack);
    addAndMakeVisible(actionFavorite = new ImageButton());
    actionFavorite->addListener(this);
    actionFavorite->setImages(false, true, false,
        AXLoadImage(favorite_normal_png), 1.0f, Colours::transparentBlack,
        AXLoadImage(favorite_press_png), 0.9f, Colours::transparentBlack,
        AXLoadImage(favorite_press_png), 1.0f, Colours::transparentBlack);
    addAndMakeVisible(actionNext = new ImageButton());
    actionNext->addListener(this);
    actionNext->setImages(false, true, false,
        AXLoadImage(next_normal_png), 1.0f, Colours::transparentBlack,
        AXLoadImage(next_press_png), 0.9f, Colours::transparentBlack,
        AXLoadImage(next_press_png), 1.0f, Colours::transparentBlack);

    addAndMakeVisible(positionSlider = new Slider(Slider::LinearHorizontal, Slider::NoTextBox));
    positionSlider->setColour(Slider::ColourIds::textBoxOutlineColourId, Colours::transparentBlack);
    positionSlider->addListener(this);

    setSize(510, 230);

    nDownCount = 8;
    isPause = false;
    musicPlayer.audioTransportSource.addChangeListener(this);
    // Init Basic
    HDouBanMusic::getInstance()->SubmiteRequest(new REQLogin("23925493@qq.com", "Hanruijun1", this));
    startTimer(500);
}

MainContentComponent::~MainContentComponent()
{
    stopTimer();
    musicPlayer.audioTransportSource.removeChangeListener(this);
}

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xffffffff));
}

void MainContentComponent::resized()
{
    const int nWidth = getWidth();
    const int nHeight = getHeight();
    
    comboBox->setBounds(nWidth - 100, 0, 100, 30);
    thumbComponent->setBounds(0, 0, nHeight, nHeight);
    
    int nSize = nHeight / 6;
    
    actionFavorite->setBounds(nHeight + (0.25 * (nWidth - nHeight)) - (nSize / 2.0f), nHeight * 0.6f, nSize, nSize);
    actionRemove->setBounds(nHeight + (0.5 * (nWidth - nHeight)) - (nSize / 2.0f), nHeight * 0.6f, nSize, nSize);
    actionNext->setBounds(nHeight + (0.75 * (nWidth - nHeight)) - (nSize / 2.0f), nHeight * 0.6f, nSize, nSize);
    positionSlider->setBounds(actionFavorite->getX(), (nHeight-50) / 2, actionNext->getRight() - actionFavorite->getX(), 40);
}

void MainContentComponent::OnResponse(DouBanRequestID id, int err /*= 0*/, String message /*= String::empty*/)
{
    switch (id)
    {
    case IDRequest_Douban_Login:
    {
        HDouBanMusic::getInstance()->SubmiteRequest(new REQChannels(this));
    }
    break;
    case IDRequest_Douban_Channels:
    {
        if (!HDouBanMusic::getInstance()->getCurrentSong())
        {
            if (const TDChannel* channel = HDouBanMusic::getInstance()->getCurrentChannel())
                HDouBanMusic::getInstance()->SubmiteRequest(new REQNewSongs(channel->channelID, this));
        }

        reloadChannelsList();
    }
    break;
    case IDRequest_Douban_SkipSong:
    case IDRequest_Douban_NextSongs:
    case IDRequest_Douban_NewSongs:
    case IDRequest_Douban_EndSong:
    case IDRequest_Douban_ByeSong:
    {
        
        // setEnableAction(true);
    }
    break;

    case IDRequest_Douban_RateSong:
    case IDRequest_Douban_UnRateSong:
    default:
        break;
    }
}


void MainContentComponent::reloadChannelsList()
{
    comboBox->clear(dontSendNotification);
    const OwnedArray<TDChannel>* arr = HDouBanMusic::getInstance()->getChannels();
    for (int i = 0; i < arr->size(); ++i)
    {
        TDChannel* chanel = arr->getUnchecked(i);
        comboBox->addItem(chanel->channelName, i + 1);
    }

    comboBox->setSelectedId(1, sendNotification);
}

void MainContentComponent::comboBoxChanged(ComboBox*)
{
    HDouBanMusic::getInstance()->setChannelIdx(comboBox->getSelectedId() - 1);
    if (const TDChannel* channel = HDouBanMusic::getInstance()->getCurrentChannel())
        HDouBanMusic::getInstance()->SubmiteRequest(new REQNewSongs(channel->channelID, this));
}

bool MainContentComponent::prepareNextAudio()
{
    if (const TDChannel* channel = HDouBanMusic::getInstance()->getCurrentChannel())
        return HDouBanMusic::getInstance()->SubmiteRequest(new REQNewSongs(channel->channelID, this));

    return false;
}

void MainContentComponent::updateSongInfo(const TDSong* song)
{
    if (DocumentWindow* window = findParentComponentOfClass<DocumentWindow>())
    {
        window->setName(song->songTitle);
    }

    if (song && song->songPicture.isNotEmpty())
    {
        String name = ProjectInfo::getMusicPicturePatn();
        name << "\\" << song->songTitle << " - " << song->songArtist << song->songPicture.substring(song->songPicture.lastIndexOfIgnoreCase("."));
        thumbComponent->setImageUrl(song->songPicture, name);
    }
}

void MainContentComponent::buttonClicked(Button* button)
{
    const TDChannel* channel = HDouBanMusic::getInstance()->getCurrentChannel();
    TDSong* song = HDouBanMusic::getInstance()->getCurrentSong();

    if (button == actionFavorite)
    {
        if (!song->songLike)
        {
            HDouBanMusic::getInstance()->SubmiteRequest(new REQRateSong(channel->channelID, song->songSid, this));
        }
        else
        {
            HDouBanMusic::getInstance()->SubmiteRequest(new REQUnRateSong(channel->channelID, song->songSid, this));
        }
    }
    else if (button == actionRemove)
    {
        HDouBanMusic::getInstance()->SubmiteRequest(new REQSkipSong(channel->channelID, song->songSid, this));
    }
    else if (button == actionNext)
    {
        // setEnableAction(false);
        HDouBanMusic::getInstance()->SubmiteRequest(new REQSkipSong(channel->channelID, song->songSid, this));
    }
    else if (button == thumbComponent)
    {
        isPause = musicPlayer.audioTransportSource.isPlaying();
        isPause ? musicPlayer.audioTransportSource.stop() : musicPlayer.audioTransportSource.start();
    }
}

void MainContentComponent::setEnableAction(bool enable)
{
    actionFavorite->setEnabled(enable);
    actionRemove->setEnabled(enable);
    actionNext->setEnabled(enable);
}

void MainContentComponent::changeListenerCallback(ChangeBroadcaster* source)
{
//     if (AudioTransportSource* transport = (AudioTransportSource*)source)
//     {
//         if (transport->isPlaying())
//         {
//             positionSlider->setRange(0, transport->getLengthInSeconds());
//             startTimerHz(10);
//         }
//         else
//         {
//             if (!isPause)
//             {
//                 HDouBanMusic::getInstance()->removeLastSong();
//                 prepareNextAudio();
//             }
//             stopTimer();
//         }
//     }
}

void MainContentComponent::timerCallback()
{
    {
        if (HDouBanMusic::getInstance()->getSongCount() < 10)
        {
            const TDChannel* channel = HDouBanMusic::getInstance()->getCurrentChannel();
            const TDSong* song = HDouBanMusic::getInstance()->getLastSong();
            if (song)
            {
                HDouBanMusic::getInstance()->SubmiteRequest(new REQSkipSong(channel->channelID, song->songSid, this));
            }
            else
            {
                HDouBanMusic::getInstance()->SubmiteRequest(new REQNextSongs(channel->channelID, this));
            }
        }
    }

    // positionSlider->setValue(/*(int)*/musicPlayer.audioTransportSource.getCurrentPosition(), dontSendNotification);
}

void MainContentComponent::sliderValueChanged(Slider* slider)
{
    musicPlayer.audioTransportSource.setPosition(slider->getValue());
}

void MainContentComponent::sliderDragStarted(Slider*)
{
    isPause = true;
    musicPlayer.audioTransportSource.stop();
}

void MainContentComponent::sliderDragEnded(Slider*)
{
    isPause = false;
    musicPlayer.audioTransportSource.start();
}
