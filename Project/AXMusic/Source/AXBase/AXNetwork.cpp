#include "AXNetwork.h"

//==============================================================================
HRequest::HRequest(HResponse* resp)
    : Thread(getName())
    , response(resp)
{

}
HRequest::~HRequest()
{
    stopThread(-1);
}

void HRequest::run()
{
    int statusCode = 0;
    StringPairArray responseHeaders;

    juce::URL url(getURL());
    ScopedPointer<InputStream> stream(url.withParameters(arguments).createInputStream(POST == getMethod(),
        (juce::URL::OpenStreamProgressCallback*)HRequest::ProcessCallback,
        this,
        String::empty,
        10000, // timeout in millisecs
        &responseHeaders, &statusCode));

    if (stream == nullptr/* || statusCode != 0*/)
    {
        String message;
        message << L"[ERROR] [REQUEST " << getRequestID() << L"] Status code:" << statusCode << L" Stream:" << (long)stream.get() << L"";
        Logger::writeToLog(message);
        return;
    }

    data = JSON::parse(stream->readEntireStreamAsString());
    juce::MessageManager::getInstance()->callAsync([=](){
        HNetManager::getInstance()->ProcessResponse(this);
    });
}

bool HRequest::ProcessCallback(void* context, int bytesSent, int totalBytes)
{
    ignoreUnused(context);
    ignoreUnused(bytesSent);
    ignoreUnused(totalBytes);
    return true;
}

HResponse* HRequest::getResponseTarget()
{
    return response;
}

var& HRequest::getData()
{
    return data;
}

//==============================================================================
juce_ImplementSingleton(HNetManager);

HNetManager::HNetManager()
{
}

HNetManager::~HNetManager()
{
}

bool HNetManager::SubmiteRequest(HRequest* req, HNetBase* target)
{
    mapRequests.set(req, target);
    req->startThread();
    return true;
}

void HNetManager::ProcessResponse(HRequest* req)
{
    ScopedPointer<HRequest> deleter(req);

    do 
    {
        if (!mapRequests.contains(req))
            return;

        HNetBase* base = mapRequests[req];
        if (!base)
            break;

        if (!base->ChechValidData(req->getRequestID(), req->getData()))
        {
            int r = req->getData().getProperty("r", 0);
            String msg = req->getData().getProperty("err", String::empty);

            // 错误响应
            if (HResponse* resp = req->getResponseTarget())
                resp->OnResponse(req->getRequestID(), r, msg);

            break;
        }

        if (!base->ProcessData(req->getRequestID(), req->getData()))
            break;

        // 成功响应
        if (HResponse* resp = req->getResponseTarget())
            resp->OnResponse(req->getRequestID());

    } while (false);

    mapRequests.remove(req);
}

bool HNetBase::SubmiteRequest(HRequest* req)
{
    return HNetManager::getInstance()->SubmiteRequest(req, this);
}
