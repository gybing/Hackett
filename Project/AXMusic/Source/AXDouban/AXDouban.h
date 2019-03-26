#ifndef HACK_DOBAN_H_INCLUDED
#define HACK_DOBAN_H_INCLUDED
#include "../AXBase/AXNetwork.h"
#include "AXDBStructs.h"

//==============================================================================
// �����¼�ӿ�
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
// ��ȡƵ���б�
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
// ��ȡ�����б�,û�и������ţ���������Ҳû���κθ�������Ҫ�����²����б�
class REQNewSongs : public REQSongBase
{
public:
    REQNewSongs(int channel, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_NewSongs; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQNewSongs)
};

//==============================================================================
// ��ǰ����������ϣ����Ǹ��������л��и���
class REQEndSong : public REQSongBase
{
public:
    REQEndSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_EndSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQEndSong)
};

//==============================================================================
// �������ڲ��ţ������л��и�������Ҫ�����µĲ����б�
class REQNextSongs : public REQSongBase
{
public:
    REQNextSongs(int channel, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_NextSongs; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQNextSongs)
};

//==============================================================================
// ���ٲ��ţ����Ż�һ���µĸ����б�
class REQByeSong : public REQSongBase
{
public:
    REQByeSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_ByeSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQByeSong)
};

//==============================================================================
// �������ڲ��ţ������л��и������������û������һ��
class REQSkipSong : public REQSongBase
{
public:
    REQSkipSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_SkipSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQSkipSong)
};

//==============================================================================
// �������ڲ��ţ����ϲ����ǰ����
class REQRateSong : public REQSongBase
{
public:
    REQRateSong(int channel, const String& sid, HResponse* target = nullptr);

protected:
    virtual DouBanRequestID getRequestID() override { return IDRequest_Douban_RateSong; }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(REQRateSong)
};

//==============================================================================
// �������ڲ��ţ����ȡ��ϲ����ǰ����
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
