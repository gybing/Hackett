#include "AXDouban.h"
#include "AXDown.h"

//==============================================================================
// ��������б����
namespace ReportType
{
    const wchar_t* type_n = L"n"; // new��û�и������ţ���������Ҳû���κθ�������Ҫ�����²����б�	������
    const wchar_t* type_e = L"e"; // sid	end����ǰ����������ϣ����Ǹ��������л��и���	�̱���
    const wchar_t* type_p = L"p"; // playing���������ڲ��ţ������л��и�������Ҫ�����µĲ����б�	������
    const wchar_t* type_b = L"b"; // sid	bye�����ٲ��ţ����Ż�һ���µĸ����б�	������
    const wchar_t* type_s = L"s"; // sid	skip���������ڲ��ţ������л��и������������û������һ��	�̱���
    const wchar_t* type_r = L"r"; // sid	rate���������ڲ��ţ����ϲ����ǰ����	�̱���
    const wchar_t* type_u = L"u"; // sid	unrate���������ڲ��ţ����ȡ��ϲ����ǰ����	�̱���
}

//==============================================================================
REQLogin::REQLogin(String name, String password, HResponse* target /*= nullptr*/)
    : HRequest(target)
{
    arguments.set("app_name", "radio_ios");
    arguments.set("version", String(100));
    arguments.set("email", name);
    arguments.set("password", password);
}

//==============================================================================
REQChannels::REQChannels(HResponse* target /*= nullptr*/)
    : HRequest(target)
{
}

//==============================================================================
REQSongBase::REQSongBase(HResponse* target)
    : HRequest(target)
{
    if (HDouBanMusic::getInstance()->isLogin())
    {
        arguments.set("user_id", HDouBanMusic::getInstance()->getUserToken()->userID);
        arguments.set("expire", HDouBanMusic::getInstance()->getUserToken()->userExpire);
        arguments.set("token", HDouBanMusic::getInstance()->getUserToken()->userToken);
    }
    arguments.set("app_name", "radio_ios");
    arguments.set("version", String(100));
}

//==============================================================================
REQNewSongs::REQNewSongs(int channel, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_n);
    arguments.set("channel", String(channel));
}

//==============================================================================
REQEndSong::REQEndSong(int channel, const String& sid, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_e);
    arguments.set("channel", String(channel));
    arguments.set("sid", sid);
}



//==============================================================================
REQNextSongs::REQNextSongs(int channel, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_p);
    arguments.set("channel", String(channel));
}


//==============================================================================
REQByeSong::REQByeSong(int channel, const String& sid, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_b);
    arguments.set("channel", String(channel));
    arguments.set("sid", sid);
}


//==============================================================================
REQSkipSong::REQSkipSong(int channel, const String& sid, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_s);
    arguments.set("channel", String(channel));
    arguments.set("sid", sid);
}


//==============================================================================
REQRateSong::REQRateSong(int channel, const String& sid, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_r);
    arguments.set("channel", String(channel));
    arguments.set("sid", String(sid));
}

//==============================================================================
REQUnRateSong::REQUnRateSong(int channel, const String& sid, HResponse* target /*= nullptr*/)
    : REQSongBase(target)
{
    arguments.set("type", ReportType::type_u);
    arguments.set("channel", String(channel));
    arguments.set("sid", String(sid));
}

//==============================================================================
juce_ImplementSingleton(HDouBanMusic);

HDouBanMusic::HDouBanMusic()
{
    currentSong = nullptr;
    currentChannel = nullptr;
}

HDouBanMusic::~HDouBanMusic()
{

}

bool HDouBanMusic::initialize()
{
    currentSong = nullptr;
    return true;
}

void HDouBanMusic::finalize()
{
}

bool HDouBanMusic::isLogin() const
{
    return nullptr != loginToken.get();
}

bool HDouBanMusic::ProcessData(DouBanRequestID id, const var& v)
{
    switch (id)
    {
    case IDRequest_Douban_Login:
    {
        loginToken = new TDUserToken;
        loginToken->FromJson(v);
        return true;
    }
    case IDRequest_Douban_Channels:
    {
        channels.clear(true);
        Array<var>* chans = v.getProperty("channels", var()).getArray();
        for (int i = 0; i < chans->size(); ++i)
        {
            auto c = chans->getUnchecked(i);
            TDChannel* ch = new TDChannel;
            ch->FromJson(c);
            channels.add(ch);
        }

        return true;
    }
    
    case IDRequest_Douban_NewSongs:
    case IDRequest_Douban_EndSong:
    case IDRequest_Douban_NextSongs:
    case IDRequest_Douban_ByeSong:
    case IDRequest_Douban_SkipSong:
    case IDRequest_Douban_RateSong:
    case IDRequest_Douban_UnRateSong:
    {
        Array<var>* sns = v.getProperty("song", var()).getArray();
        for (int i = 0; i < sns->size(); ++i)
        {
            auto c = sns->getUnchecked(i);
            TDSong* s = new TDSong;
            s->FromJson(c);
            songs.add(s);

            AXDown* d = new AXDown(s);
            arrDowns.add(d);
            d->startThread();
        }
        return true;
    }
    default:
        break;
    }

    return false;
}

bool HDouBanMusic::ChechValidData(DouBanRequestID id, const var& v)
{
    int r = v.getProperty("r", 0);
    return r == 0;
}

const TDUserToken* HDouBanMusic::getUserToken() const
{
    return loginToken;
}

const OwnedArray<TDChannel>* HDouBanMusic::getChannels() const
{
    return &channels;
}

TDChannel* HDouBanMusic::getChanelByIdx(int n)
{
    if (n >= 0 && n < channels.size())
        return channels.getUnchecked(n);
    return nullptr;
}

TDSong* HDouBanMusic::getCurrentSong()
{
    ScopedLock l(cs);

    if (!currentSong && !songs.isEmpty())
    {
        currentSong = songs.getUnchecked(0);
    }
    return currentSong;
}

bool HDouBanMusic::removeLastSong()
{
    ScopedLock l(cs);

    currentSong = nullptr;
    if (!songs.isEmpty())
        songs.removeRange(0, 1);
    return true;
}

int HDouBanMusic::getSongCount()
{
    return songs.size();
}

const TDChannel* HDouBanMusic::getCurrentChannel()
{
    return currentChannel;
}

void HDouBanMusic::setChannelIdx(int idx)
{
    currentChannel = getChanelByIdx(idx);
}

void HDouBanMusic::removeSong(const String& ssid)
{
    ScopedLock l(cs);

    for (int i = 0; i < songs.size(); ++i)
    {
        if (songs[i]->songSid == ssid)
        {
            songs.remove(i);
            break;
        }
    }
}

TDSong* HDouBanMusic::getLastSong()
{
    ScopedLock l(cs);
    if (songs.isEmpty())
        return nullptr;
    
    return songs.getLast();
}
