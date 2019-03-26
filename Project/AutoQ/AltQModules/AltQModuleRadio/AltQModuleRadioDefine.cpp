#include "AltQModuleRadioDefine.h"

bool AltQDataUserToken::FromJson(const var& v)
{
    userID = v.getProperty("user_id", String::empty);
    userToken = v.getProperty("token", String::empty);
    userExpire = v.getProperty("expire", String::empty);
    userName = v.getProperty("user_name", String::empty);
    userEmail = v.getProperty("email", String::empty);
    return true;
}

bool AltQDataChannel::FromJson(const var& v)
{
    channelID = v.getProperty("channel_id", 0);
    channelName = v.getProperty("name", String::empty);
    channelNameEn = v.getProperty("name_en", String::empty);
    channelAddr = v.getProperty("abbr_en", String::empty);
    channelSep = v.getProperty("seq_id", 0);
    return true;
}

var AltQDataChannel::ToJson()
{
    DynamicObject* o = new DynamicObject;
    o->setProperty("channel_id", channelID);
    o->setProperty("name", channelName);
    o->setProperty("name_en", channelNameEn);
    o->setProperty("abbr_en", channelAddr);
    o->setProperty("seq_id", channelSep);
    return var(o);
}

bool AltQDataSong::FromJson(const var& v)
{
    songAlbum = v.getProperty("album", String::empty);
    songPicture = v.getProperty("picture", String::empty);
    songSsid = v.getProperty("ssid", String::empty);
    songArtist = v.getProperty("artist", String::empty);
    songUrl = v.getProperty("url", String::empty);
    songCompany = v.getProperty("company", String::empty);
    songTitle = v.getProperty("title", String::empty);
    songRatingAvg = v.getProperty("rating_avg", 0.0f);
    songLength = v.getProperty("length", 0);
    songSubType = v.getProperty("subtype", String::empty);
    songPublicTime = v.getProperty("public_time", String::empty);
    songSid = v.getProperty("sid", String::empty);
    songAid = v.getProperty("aid", String::empty);
    songSHA256 = v.getProperty("sha256", String::empty);
    songKbps = v.getProperty("kbps", String::empty);
    songAlbumtitle = v.getProperty("albumtitle", String::empty);
    songLike = v.getProperty("like", 0);
    return true;
}
