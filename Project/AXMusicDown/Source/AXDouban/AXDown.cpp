#include "AXDown.h"
#include "AXDouban.h"

AXDown::AXDown(const TDSong* u)
    : music(*u)
    , Thread("")
{
}

AXDown::~AXDown()
{
}

void AXDown::run()
{
    {
        URL url(music.songUrl);
        ScopedPointer<InputStream> stream = url.createInputStream(false);

        File out(File::getSpecialLocation(File::userMusicDirectory).getChildFile("AXMusic").getChildFile(HDouBanMusic::getInstance()->getCurrentChannel()->channelName));
        out.createDirectory();

        String path = out.getFullPathName();
        path << "\\";
        path << music.songTitle;
        path << "-";
        path << music.songArtist;
        path << File(music.songUrl).getFileExtension();

        File outFile(path);
        if (!outFile.existsAsFile())
        {
            juce::FileOutputStream fs(outFile);
            fs.writeFromInputStream(*stream, stream->getTotalLength());
        }
    }

    wait(1000);
    HDouBanMusic::getInstance()->removeSong(music.songSid);
}
