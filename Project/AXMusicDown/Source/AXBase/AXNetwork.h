#ifndef HACK_NETBASE_H_INCLUDED
#define HACK_NETBASE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

// ����ʽ
enum MethodType {  GET, POST };

// ��������ID
enum DouBanRequestID
{
    IDRequest_Douban_Login,
    IDRequest_Douban_Channels,

    IDRequest_Douban_NewSongs, // û�и������ţ���������Ҳû���κθ�������Ҫ�����²����б�
    IDRequest_Douban_EndSong, // ��ǰ����������ϣ����Ǹ��������л��и���
    IDRequest_Douban_NextSongs, // �������ڲ��ţ������л��и�������Ҫ�����µĲ����б�
    IDRequest_Douban_ByeSong, // ���ٲ��ţ����Ż�һ���µĸ����б�
    IDRequest_Douban_SkipSong, // �������ڲ��ţ������л��и������������û������һ��
    IDRequest_Douban_RateSong, // �������ڲ��ţ����ϲ����ǰ����
    IDRequest_Douban_UnRateSong, // �������ڲ��ţ����ȡ��ϲ����ǰ����
};

class HResponse
{
public:
    virtual void OnResponse(DouBanRequestID id, int err = 0, String message = String::empty) = 0;
};

// ��·����ӿ�
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

    // ��������
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
