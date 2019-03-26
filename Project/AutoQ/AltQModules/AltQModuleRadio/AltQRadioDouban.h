#pragma once
#include "AltQDefine.h"
#include "AltQBase.h"
#include "AltQModuleRadioDefine.h"
#include "AltQRadioPlayer.h"
using namespace juce;
#include <list>

//==============================================================================
class AltQRadioDouban : public ChangeListener
{
public:
    AltQRadioDouban();
    ~AltQRadioDouban();

    bool login(const String& email, const String& password);
    bool getChannels(std::vector<AltQDataChannel::Ptr>& chs);
    
    void play(int ch = 0);
    void next();

    void pause() { player.pause(); }
    void resume() { player.resume(); }
    void stop() { player.stop(); }

protected:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

    bool requestSongs(int ch, const char* rpt);
    AltQDataSong::Ptr getsong(int ch = 0, const char* rpt = "");

private:
    int currentChannelID;
    AltQRadioPlayer player;
    AltQDataUserToken usertoken;
    ScopedPointer<AltQDataSong::Ptr> lastsong;
    std::list<AltQDataSong::Ptr> playlist;

    class AltQNetwork
    {
    public:
        AltQNetwork(AltQRadioDouban& o, const char* addr, bool ispost, bool withtoken) 
            : douban(o), url(addr), isPost(ispost)
        {
            if (withtoken)
            {
                arguments.set("user_id", douban.usertoken.userID);
                arguments.set("expire", douban.usertoken.userExpire);
                arguments.set("token", douban.usertoken.userToken);
            }
            arguments.set("app_name", "radio_ios");
            arguments.set("version", String(100));
        }
        ~AltQNetwork(){}

        var RequestData()
        {
            stream = url.withParameters(arguments).createInputStream(isPost);
            return JSON::parse(stream->readEntireStreamAsString());
        }

        StringPairArray& Argument() { return arguments; }
    private:
        AltQRadioDouban& douban;
        StringPairArray arguments;
        URL url;
        ScopedPointer<InputStream> stream;
        bool isPost;
    };
    friend class AltQNetwork;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQRadioDouban);
};
