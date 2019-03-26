#ifndef HACK_NETBASE_H_INCLUDED
#define HACK_NETBASE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

// 请求方式
enum MethodType {  GET, POST };

// 豆瓣请求ID
enum DouBanRequestID
{
    IDRequest_Douban_Login,
    IDRequest_Douban_Channels,

    IDRequest_Douban_NewSongs, // 没有歌曲播放，歌曲队列也没有任何歌曲，需要返回新播放列表
    IDRequest_Douban_EndSong, // 当前歌曲播放完毕，但是歌曲队列中还有歌曲
    IDRequest_Douban_NextSongs, // 歌曲正在播放，队列中还有歌曲，需要返回新的播放列表
    IDRequest_Douban_ByeSong, // 不再播放，并放回一个新的歌曲列表
    IDRequest_Douban_SkipSong, // 歌曲正在播放，队列中还有歌曲，适用于用户点击下一首
    IDRequest_Douban_RateSong, // 歌曲正在播放，标记喜欢当前歌曲
    IDRequest_Douban_UnRateSong, // 歌曲正在播放，标记取消喜欢当前歌曲
};

class HResponse
{
public:
    virtual void OnResponse(DouBanRequestID id, int err = 0, String message = String::empty) = 0;
};

// 网路请求接口
class HRequest : private Thread
{
protected:
    virtual String getURL() = 0;
    virtual DouBanRequestID getRequestID() = 0;
    virtual String getName() { return String::empty; }
    virtual MethodType getMethod() { return GET; }

    friend class HNetManager;
    HRequest(HResponse* resp);
    var& getData();
    HResponse* getResponseTarget(); 

public:
    virtual ~HRequest();

protected:
    virtual void run() override;
    static bool ProcessCallback(void* context, int bytesSent, int totalBytes);

protected:
    StringPairArray arguments;
    var data;
    HResponse* response;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HRequest)
};

class HNetBase
{
public:
    friend class HNetManager;
    virtual ~HNetBase() {}

    // 发送请求
    bool SubmiteRequest(HRequest* req);

protected:
    HNetBase() {}
    virtual bool ProcessData(DouBanRequestID id, const var& v) = 0;
    virtual bool ChechValidData(DouBanRequestID id, const var& v) = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HNetBase)
};

class HNetManager
{
public:
    virtual ~HNetManager();
    juce_DeclareSingleton(HNetManager, false);

    bool SubmiteRequest(HRequest* req, HNetBase* target);

protected:
    friend class HRequest;
    HNetManager();
    void ProcessResponse(HRequest* req);

private:
    juce::HashMap<HRequest*, HNetBase*> mapRequests;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HNetManager)
};

#endif   // HACK_NETBASE_H_INCLUDED
