#pragma once
#include "AltQInterfaceModule.h"
#include "AltQBase.h"
using namespace juce;

struct AltQDataUserToken
{
    String userID;
    String userToken;
    String userExpire;
    String userName;
    String userEmail;

    bool FromJson(const var& v);
};


struct AltQDataChannel : public ReferenceCountedObject
{
    int channelID;
    String channelName;
    String channelNameEn;
    String channelAddr;
    int channelSep;

    bool FromJson(const var& v);
    var ToJson();
    typedef ReferenceCountedObjectPtr<AltQDataChannel> Ptr;
};

struct AltQDataSong : public ReferenceCountedObject
{
    String songAlbum;
    String songPicture;
    String songSsid;
    String songArtist;
    String songUrl;
    String songCompany;
    String songTitle;
    float songRatingAvg;
    int songLength;
    String songSubType;
    String songPublicTime;
    String songSid;
    String songAid;
    String songSHA256;
    String songKbps;
    String songAlbumtitle;
    int songLike;

    bool FromJson(const var& v);
    typedef ReferenceCountedObjectPtr<AltQDataSong> Ptr;
};


class AltQRadioResult : public IAltQModuleResult
{
public:
    AltQRadioResult(const AltQDocID& doc) : docID(doc) {}

	virtual std::string GetTitle()
	{
		return channel.channelName.toStdString();
	}

	virtual std::string GetDescription()
	{
        return String(L"“Ù¿÷µÁÃ®").toStdString();
	}

	virtual std::string GetIcon()
	{
        File path = File::getSpecialLocation(File::currentExecutableFile);
        File icon = path.getParentDirectory().getChildFile("Plugin/Radio/icon.ico");
        if (icon.existsAsFile())
        {
            return icon.getFullPathName().toStdString();
        }
		return "";
	}

    virtual AltQDocID GetDocID()
    {
        return docID;
    }

    AltQDocID docID;
    AltQDataChannel channel;
};
typedef std::shared_ptr<AltQRadioResult> AltQRadioResultPtr;