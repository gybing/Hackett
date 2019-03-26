#ifndef HACK_DOBAN_H_INCLUDED
#define HACK_DOBAN_H_INCLUDED
#include "../AXBase/AXNetwork.h"
#include "AXDBStructs.h"

//==============================================================================
// 豆瓣登录接口
class REQLogin : public HRequest
{
public:
    REQLogin(String name, String password, HResponse* target = nullptr);

protected:
    virtual MethodType getMethod() { return POST; }
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_Login; }
    virtual String getURL() override { return L"http://www.douban.com/j/app/login"; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQLogin)
};

//==============================================================================
// 获取频道列表
class REQChannels : public HRequest
{
public:
    REQChannels(HResponse* target = nullptr);
    
protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_Channels; }
    virtual String getURL() override { return L"http://www.douban.com/j/app/radio/channels"; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQChannels)
};

class REQSongBase : public HRequest
{
public:
    REQSongBase(HResponse* target);

protected:
    virtual String getURL() override { return L"http://www.douban.com/j/app/radio/people"; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQSongBase)
};

//==============================================================================
// 获取歌曲列表,没有歌曲播放，歌曲队列也没有任何歌曲，需要返回新播放列表
class REQNewSongs : public REQSongBase
{
public:
    REQNewSongs(int channel, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_NewSongs; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQNewSongs)
};

//==============================================================================
// 当前歌曲播放完毕，但是歌曲队列中还有歌曲
class REQEndSong : public REQSongBase
{
public:
    REQEndSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_EndSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQEndSong)
};

//==============================================================================
// 歌曲正在播放，队列中还有歌曲，需要返回新的播放列表
class REQNextSongs : public REQSongBase
{
public:
    REQNextSongs(int channel, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_NextSongs; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQNextSongs)
};

//==============================================================================
// 不再播放，并放回一个新的歌曲列表
class REQByeSong : public REQSongBase
{
public:
    REQByeSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_ByeSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQByeSong)
};

//==============================================================================
// 歌曲正在播放，队列中还有歌曲，适用于用户点击下一首
class REQSkipSong : public REQSongBase
{
public:
    REQSkipSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_SkipSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQSkipSong)
};

//==============================================================================
// 歌曲正在播放，标记喜欢当前歌曲
class REQRateSong : public REQSongBase
{
public:
    REQRateSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_RateSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQRateSong)
};

//==============================================================================
// 歌曲正在播放，标记取消喜欢当前歌曲
class REQUnRateSong : public REQSongBase
{
public:
    REQUnRateSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_UnRateSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQUnRateSong)
};

//==============================================================================
class HDouBanMusic : public HNetBase
{
public:
    ~HDouBanMusic();
    juce_DeclareSingleton(HDouBanMusic, false);
    bool initialize();
    void finalize();

    bool isLogin() const;
    const TDUserToken* getUserToken() const;
    const OwnedArray<TDChannel>* getChannels() const;
    const TDChannel* getChanelByIdx(int n) const;

    TDSong* getCurrentSong();
    bool removeLastSong();

protected:
    HDouBanMusic();
    virtual bool ProcessData(DouBanRequestID id, const var& v) override;
    virtual bool ChechValidData(DouBanRequestID id, const var& v) override;

private:
    ScopedPointer<TDUserToken> loginToken;
    OwnedArray<TDChannel> channels;
    OwnedArray<TDSong> songs;
    struct TDSong* currentSong;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HDouBanMusic)
};

#endif   // HACK_DOBAN_H_INCLUDED
