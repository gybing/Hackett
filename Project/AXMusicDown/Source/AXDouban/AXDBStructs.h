#ifndef HACK_DSTRUCT_H_INCLUDED
#define HACK_DSTRUCT_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

struct TDBase
{
    virtual ~TDBase(){}
    virtual bool FromJson(const var& v) = 0;
};

struct TDUserToken : public TDBase
{
    String userID;
    String userToken;
    String userExpire;
    String userName;
    String userEmail;

    virtual bool FromJson(const var& v) override;
};

struct TDChannel : public TDBase
{
    int channelID;
    String channelName;
    String channelNameEn;
    String channelAddr;
    int channelSep;

    virtual bool FromJson(const var& v) override;
};

struct TDSong : public TDBase
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

    virtual bool FromJson(const var& v) override;
};
#endif   // HACK_DSTRUCT_H_INCLUDED
