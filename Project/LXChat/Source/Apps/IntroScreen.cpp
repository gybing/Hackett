#include "JuceHeader.h"
#include "JuceDemoHeader.h"
#include <map>

//==============================================================================
class IntroScreen   : public Component
{
public:
    IntroScreen()
        : linkButton ("www.juce.com", URL ("http://www.juce.com"))
    {
        setOpaque (true);

        addAndMakeVisible (versionLabel);
        addAndMakeVisible (linkButton);
        addAndMakeVisible (logo);

        versionLabel.setColour (Label::textColourId, Colours::white);
        versionLabel.setText (String ("{version}  built on {date}")
                                  .replace ("{version}", SystemStats::getJUCEVersion())
                                  .replace ("{date}",    String (__DATE__).replace ("  ", " ")),
                              dontSendNotification);

        linkButton.setColour (HyperlinkButton::textColourId, Colours::lightblue);
    }

    void paint (Graphics& g) override
    {
        g.fillAll(Colour(0xFFF2F2F2));
    }

    void resized() override
    {
        Rectangle<int> area (getLocalBounds().reduced (10));
        logo.setBounds (area);
        area = area.removeFromBottom (24);
        linkButton.setBounds (area.removeFromRight (getWidth() / 4));
        versionLabel.setBounds (area);
    }

private:
    Label versionLabel;
    HyperlinkButton linkButton;

    //==============================================================================
    struct LogoDrawComponent  : public Component,
                                private Timer
    {
        LogoDrawComponent()   : logoPath (getJUCELogoPath()), elapsed (0.0f)
        {
            startTimerHz (30); // repaint at 30 fps
        }

        void paint (Graphics& g) override
        {

            const float waveStep = 10.0f;
            const float waveY = getHeight() * 0.44f;
            int i = 0;

            static std::map<int, Colour> mapTestColors;

            for (float x = waveStep * 0.5f; x < getWidth(); x += waveStep)
            {
                const float y1 = waveY + getHeight() * 0.05f * std::sin (i * 0.38f + elapsed);
                const float y2 = waveY + getHeight() * 0.10f * std::sin (i * 0.20f + elapsed * 2.0f);

                Path wavePath;
                wavePath.addLineSegment(Line<float>(x, y1, x, y2), 2.0f);
                wavePath.addEllipse (x - waveStep * 0.3f, y1 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);
                wavePath.addEllipse (x - waveStep * 0.3f, y2 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);

                ++i;

                if (mapTestColors.find(i) == mapTestColors.end())
                    mapTestColors[i] = ProjectInfo::getRandomBrightColour();

                g.setColour(mapTestColors[i]);
                g.fillPath(wavePath);
            }

            g.fillPath (logoPath, RectanglePlacement (RectanglePlacement::centred)
                                    .getTransformToFit (logoPath.getBounds(),
                                                        getLocalBounds().reduced (20, getHeight() / 4).toFloat()));
        }

    private:
        void timerCallback() override
        {
            repaint();
            elapsed += 0.02f;
        }

        Path logoPath;
        float elapsed;
    };

    LogoDrawComponent logo;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IntroScreen)
};


// This static object will register this demo type in a global list of demos..
static LXAppType<IntroScreen> demo ("00 Welcome!");
