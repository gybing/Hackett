#include "AltQRadioDouban.h"

const char* ALTQURL_DOUBAN_LOGIN = "http://www.douban.com/j/app/login";
const char* ALTQURL_DOUBAN_CHANNEL = "http://www.douban.com/j/app/radio/channels";
const char* ALTQURL_DOUBAN_PEOPLE = "http://www.douban.com/j/app/radio/people";

// ��������б����
namespace ReportType
{
    const char* type_n = "n"; // new��û�и������ţ���������Ҳû���κθ�������Ҫ�����²����б�	������
    const char* type_e = "e"; // sid	end����ǰ����������ϣ����Ǹ��������л��и���	�̱���
    const char* type_p = "p"; // playing���������ڲ��ţ������л��и�������Ҫ�����µĲ����б�	������
    const char* type_b = "b"; // sid	bye�����ٲ��ţ����Ż�һ���µĸ����б�	������
    const char* type_s = "s"; // sid	skip���������ڲ��ţ������л��и������������û������һ��	�̱���
    const char* type_r = "r"; // sid	rate���������ڲ��ţ����ϲ����ǰ����	�̱���
    const char* type_u = "u"; // sid	unrate���������ڲ��ţ����ȡ��ϲ����ǰ����	�̱���
}

AltQRadioDouban::AltQRadioDouban()
{
    currentChannelID = 0;
    player.audioTransportSource.addChangeListener(this);

}

AltQRadioDouban::~AltQRadioDouban()
{
}

bool AltQRadioDouban::login(const String& email, const String& password)
{
    ScopedPointer<AltQNetwork> net(new AltQNetwork(*this, ALTQURL_DOUBAN_LOGIN, true, false));

    net->Argument().set("email", email);
    net->Argument().set("password", password);

    usertoken.FromJson(net->RequestData());
    return true;
}

bool AltQRadioDouban::getChannels(std::vector<AltQDataChannel::Ptr>& chs)
{
    ScopedPointer<AltQNetwork> net(new AltQNetwork(*this, ALTQURL_DOUBAN_CHANNEL, false, true));

    var v = net->RequestData();
    Array<var>* chans = v.getProperty("channels", var()).getArray();
    for (int i = 0; i < chans->size(); ++i)
    {
        auto c = chans->getUnchecked(i);
        AltQDataChannel* ch = new AltQDataChannel;
        ch->FromJson(c);
        chs.push_back(ch);
    }

    return true;
}

void AltQRadioDouban::play(int ch /*= 0*/)
{
    AltQDataSong::Ptr s = getsong(ch, ReportType::type_n);

    InputStream* stream = URL(s->songUrl).createInputStream(false);
    if (stream)
    {
        player.play(*stream);
    }
    currentChannelID = ch;
}

void AltQRadioDouban::next()
{
    if (AltQDataSong::Ptr s = getsong(currentChannelID, ReportType::type_s))
    {
        InputStream* stream = URL(s->songUrl).createInputStream(false);
        if (stream)
        {
            player.play(*stream);
        }
    }  
}

bool AltQRadioDouban::requestSongs(int ch, const char* rpt)
{
    ScopedPointer<AltQNetwork> net(new AltQNetwork(*this, ALTQURL_DOUBAN_PEOPLE, false, true));

    net->Argument().set("type", rpt);
    net->Argument().set("channel", String(ch));

    var v = net->RequestData();

    if (Array<var>* ss = v.getProperty("song", var()).getArray())
    {
        for (int i = 0; i < ss->size(); ++i)
        {
            AltQDataSong::Ptr s(new AltQDataSong);
            s->FromJson(ss->getUnchecked(i));
            playlist.push_back(s);
        }
    }

    return true;
}

AltQDataSong::Ptr AltQRadioDouban::getsong(int ch /*= 0*/, const char* rpt /*= ""*/)
{
    if (playlist.empty())
    {
        requestSongs(ch, rpt);
        if (playlist.empty())
            return nullptr;
    }

    AltQDataSong::Ptr s = playlist.front();
    playlist.pop_front();
    return s;
}

void AltQRadioDouban::changeListenerCallback(ChangeBroadcaster* source)
{
    if (AudioTransportSource* transport = (AudioTransportSource*)source)
    {
        if (!transport->isPlaying())
            next();
    }
}
