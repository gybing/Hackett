#include "AltQModuleRadio.h"
#include "AltQModuleRadioDefine.h"
#include <iostream>

static std::string g_module_id;
const char* g_module_name = "AltQModuleRadio";

static const String  cmd_stop = L"Í£Ö¹²¥·Å";
static const String  cmd_next = L"ÏÂÒ»Çú";

AltQModuleRadioPtr g_module;
ALTQ__API bool CreateAltQModule(IAltQModulePtr* module)
{
	if (g_module)
	{
		*module = g_module;
		return true;
	}

	g_module = std::make_shared<AltQModuleRadio>();
	*module = g_module;
	return true;
}

AltQModuleRadio::AltQModuleRadio()
{
}

bool AltQModuleRadio::Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core)
{
    g_module_id = key;
	m_StoragePath = storage_path;
	m_AltQCoreInternal = core;

    m_AltQRadioIndexer = m_AltQCoreInternal->CreateModuleIndexer(g_module_name, g_module_id);

    File storage(m_StoragePath);
    storage.createDirectory();
    if (!storage.isDirectory())
        return false;


    if (!radio.login("23925493@qq.com", "Hanruijun1"))
        return false;

    File version(storage.getChildFile("version"));
    if (!version.existsAsFile())
    {
        std::vector<AltQDataChannel::Ptr> channels;
        if (radio.getChannels(channels))
        {
            indexer = new AltQChannelIndexer(m_AltQRadioIndexer, channels);
            indexer->startThread();
        }
    }

    FileOutputStream fs(version);
    fs.write("1.0.0", 5);
    fs.flush();

	return true;
}

void AltQModuleRadio::Finalize()
{
    if (indexer) indexer->stopThread(-1);
}

IAltQModuleResultPtr AltQModuleRadio::ParseResult(const AltQResultPtr& result)
{
	if (result->m_ModuleID != g_module_id)
	{
        return nullptr;
	}
    
    if (result->m_Content == cmd_next || result->m_Content == cmd_stop)
    {
        AltQRadioResultPtr ch = std::make_shared<AltQRadioResult>(result->m_DocID);
        ch->channel.channelName = result->m_Content;
        return ch;
    }

    AltQRadioResultPtr ch = std::make_shared<AltQRadioResult>(result->m_DocID);
    if (!ch->channel.FromJson(JSON::parse(String(result->m_Content.c_str()))))
        return nullptr;

    return ch;
}

void AltQModuleRadio::OnResultSelected(const AltQDocID& id)
{
    if (id == cmd_stop)
    {
        radio.stop();
        return;
    }
    else if (id == cmd_next)
    {
        radio.next();
    }
    else
    {
        m_AltQRadioIndexer->IncreaseScore(id);
        radio.play(String(id).getIntValue());
    }
}

void AltQChannelIndexer::run()
{
    for (auto c : channels)
    {
        AltQDocument doc;
        doc.m_Content = JSON::toString(c->ToJson()).toStdString();
        doc.m_ToIndex = c->channelName.toStdString();
        doc.m_UserData = String(c->channelID).toStdString();
        indexer->AddDoc(doc);
    }

    AltQDocument doc;
    doc.m_ToIndex = cmd_next.toStdString();
    doc.m_UserData = doc.m_ToIndex;
    doc.m_Content = doc.m_ToIndex;
    indexer->AddDoc(doc);
    doc.m_ToIndex = cmd_stop.toStdString();
    doc.m_UserData = doc.m_ToIndex;
    doc.m_Content = doc.m_ToIndex;
    indexer->AddDoc(doc);
}
