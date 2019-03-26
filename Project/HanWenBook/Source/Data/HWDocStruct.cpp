#include "JuceHeader.h"
#include "HWDocStruct.h"

///////////////////////////////////////////////////////////////////////////////////////////////

void HWTCheckVersion::FromHWDict( const HWDict* dict )
{
    strVersion = HWDictElemString(dict, "version");
    strNewVersion = HWDictElemString(dict, "newversion");
    bForce = HWDictElemInt(dict, "enforce");
    strDescription = HWDictElemString(dict, "description");

    HWDict* pDict = HWDictElemDict(dict, "devicerelevance");
    strDownURL = HWDictElemString(pDict, "downloadurl");
    HWArray* pArr = HWDictElemArray(pDict, "diffFiles");
    for (unsigned int i=0; i<HWArraySize(pArr); ++i)
    {
        HWArray* pFile = HWArrayElemArray(pArr, i);
        vecDefFiles.push_back(std::pair<juce::String, int>(HWArrayElemCString(pFile, 0), HWArrayElemInt(pFile, 1)));
    }
}



// <HWTUserUataVer>:�û����ݰ汾



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookDataVer>:ͼ�����ݰ汾



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserBookVer>:�û�ͼ�����ݰ汾



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserBookStat>:�û�ͼ��ͳ����Ϣ



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTClassInfo>:�༶��Ϣ

void HWTClassInfo::FromHWDict( const HWDict* dict )
{
    nClassid = (HWEnum_Gender)HWDictElemInt(dict, "classid");
    nGrade = (HWEnum_Gender)HWDictElemInt(dict, "grade");
    strName = HWDictElemString(dict, "name");
    strSchool = HWDictElemString(dict, "school");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserInfo>:�û�����

void HWTUserInfo::FromHWDict( const HWDict* dict )
{
    enUserType = (HWEnum_UserType)HWDictElemInt(dict, "usertype");
    strShelfNo = HWDictElemString(dict, "shelfno");
    strName = juce::CharPointer_UTF8(HWDictElemString(dict, "name"));
    enSex = (HWEnum_Gender)HWDictElemInt(dict, "sex");
    nCredits = HWDictElemInt(dict, "credits");

    HWDict* extend = HWDictElemDict(dict, "extend");

    switch (enUserType)
    {
    case HWUserType_Student:
        {
            nExtStudentNo = HWDictElemInt(extend, "studentno");
            nExtClass = HWDictElemInt(extend, "classid");
        }
        break;
    case HWUserType_Teacher:
        {
            HWArray* pArrIds = HWDictElemArray(extend, "classes");
            for (unsigned int idx=0; idx<HWArraySize(pArrIds); ++idx)
                arrExtClasses.push_back(HWArrayElemInt(pArrIds, idx));
        }
        break;
    case HWUserType_Family:
        {
            HWArray* pArrIds = HWDictElemArray(extend, "children");
            for (unsigned int idx=0; idx<HWArraySize(pArrIds); ++idx)
                arrExtChildren.push_back(HWArrayElemCString(pArrIds, idx));
        }
        break;
    case HWUserType_TeacherFamily:
        {
            HWArray* pArrIds = HWDictElemArray(extend, "children");
            for (unsigned int idx=0; idx<HWArraySize(pArrIds); ++idx)
                arrExtChildren.push_back(HWArrayElemCString(pArrIds, idx));
            pArrIds = HWDictElemArray(extend, "classes");
            for (unsigned int idx=0; idx<HWArraySize(pArrIds); ++idx)
                arrExtClasses.push_back(HWArrayElemInt(pArrIds, idx));
        }
        break;
    default:
        jassert(false);
        break;
    }
    
}
int HWTUserInfo::ExtClass() const
{
    jassert(enUserType == HWUserType_Student);
    return nExtClass;
}

int HWTUserInfo::ExtStudentNo() const
{
    jassert(enUserType == HWUserType_Student);
    return nExtStudentNo;
}

std::vector<int>& HWTUserInfo::ExtClasses()
{
    jassert(enUserType == HWUserType_Teacher || enUserType == HWUserType_TeacherFamily);
    return arrExtClasses;
}

std::vector<juce::String> HWTUserInfo::ExtChildren()
{
    jassert(enUserType == HWUserType_Family || enUserType == HWUserType_TeacherFamily);
    return arrExtChildren;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserTag>:�û���ǩ��Ϣ

void HWTUserTag::FromHWDict( const HWDict* dict )
{
    nId = HWDictElemInt(dict, "id"); // @id- ��ǩID
    strName = HWDictElemString(dict, "name"); // @name- ��ǩ����

}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserBook>:�û�ͼ����Ϣ

void HWTUserBook::FromHWDict( const HWDict* dict )
{
    nId = HWDictElemInt(dict, "id");
    strBook = HWDictElemString(dict, "guid");
    nTag = HWDictElemInt(dict, "tag");
    tTime = HWDictElemInt(dict, "time");
    strName = HWDictElemString(dict, "name");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTReadData>:�Ķ�����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTHistoryData>:�Ķ���ʷ����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTReadBase>:�Ķ���������

void HWTReadBase::FromJson( const Json::Value& jv )
{
    const Json::Value& jvAuthor = jv["A"];
    arrAuthor.clear();
    for (unsigned int i=0; i<jvAuthor.size(); ++i)
        arrAuthor.push_back(juce::CharPointer_UTF8(jvAuthor[i].asCString()));
    nWordCount = jv["WC"].asInt();
    nPDFPageCount = jv["PDFPC"].asInt();
    nPDFWidth = jv["PDFW"].asInt();
    nPDFHeight = jv["PDFH"].asInt();
    jvPDFNoInfo = jv["PDFNoInfo"];
    nBodyPDFPageNo = jv["BodyPDFPageNo"].asInt();
    arrNSDMode.clear();
    const Json::Value& jvNdsMode = jv["NSDM"];
    for (unsigned int i=0; i<jvNdsMode.size(); ++i)
        arrNSDMode.push_back(juce::CharPointer_UTF8(jvNdsMode[i].asCString()));
    bExistText = jv["ExistText"].asInt();
    fHVScale = (float)jv["HVScale"].asDouble();
    bIsSupportPactMode = jv["IsSupportPactMode"].asInt();
    nFontWidth = jv["FW"].asInt();
}


///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookSign>:�û���������

void HWTBookSign::FromHWDict( const HWDict* dict )
{
    nId = HWDictElemInt(dict, "id"); // @id- ����ID
    nPageNo = HWDictElemInt(dict, "pageno"); // @pageno- ҳ��
    dwClr = HWDictElemInt(dict, "clr"); // @clr- ������ɫ
    tTime = HWDictElemInt(dict, "time"); // @time- ����ʱ��

    Json::Value jv;
    Json::Reader jr;
    if (jr.parse(HWDictElemCString(dict, "be"), jv))
    {
        nBe[0] = jv[0u].asInt();
        nBe[1] = jv[1].asInt();
    }
    strText = HWDictElemString(dict, "text");
}


//////////////////////////////////////////////////////////////////////////
// <HWTBookPostil>:�û���ע����
void HWTBookPostil::FromHWDict( const HWDict* dict )
{
    nID = HWDictElemInt(dict, "id");
    nDynamicID = HWDictElemInt(dict, "dynamicid");
    tTime = HWDictElemInt(dict, "time");
    enDevice = (HWEnum_Device)HWDictElemInt(dict, "devicetype");
    strBook = HWDictElemString(dict, "guid");
    nPageNo = HWDictElemInt(dict, "page");
    strShowPage = HWDictElemString(dict, "showpage");
    nZone = HWDictElemInt(dict, "zone");
    strText = HWDictElemString(dict, "text");
    strBookText = HWDictElemString(dict, "booktext");
    Json::Value jv;
    Json::Reader jr;
    if (jr.parse(HWDictElemCString(dict, "be"), jv))
    {
        nBex = jv[0u].asInt();
        nBec = jv[1].asInt();
    }
}

//////////////////////////////////////////////////////////////////////////
// <HWTBookComment>:�û�����

void HWTBookComment::FromHWDict( const HWDict* dict )
{
    nID = HWDictElemInt(dict, "id"); // @id- (int),                    // ����ID
    nDynamicID = HWDictElemInt(dict, "dynamicid"); // @dynamicid- (int),             // ��̬ID�����ڻ�ȡ�����ܱ�������Ϣ��
    tTime = HWDictElemInt(dict, "time"); // @time- (int),
    enDeviceType = (HWEnum_Device)HWDictElemInt(dict, "devicetype"); // @devicetype-  (int),			// �豸����
    strBook = HWDictElemString(dict, "guid"); // @guid- (string),
    strText = HWDictElemString(dict, "text"); // @text-(string),				// �����ı�
}

///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookMark>:�û���ǩ����

void HWTBookMark::FromHWDict( const HWDict* dict )
{
    nPageNo = HWDictElemInt(dict, "pageno"); // @pageno- ҳ��
    strTitle = HWDictElemString(dict, "title"); // @title- ��ǩ����
    dwClr = HWDictElemInt(dict, "clr"); // @clr- ��ǩ��ɫ
    tTime = HWDictElemInt(dict, "time"); // @time- ����ʱ��
}

///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTDynamicData>:��̬����
void HWTDynamicData::FromHWDict( const HWDict* dict )
{
    nId = HWDictElemInt(dict, "id");
    nType = HWDictElemInt(dict, "type");
    tTime = HWDictElemInt(dict, "time");
    nReplyCount = HWDictElemInt(dict, "replycount");
    nReviewCount = HWDictElemInt(dict, "reviewcount");
    nFlowerCount = HWDictElemInt(dict, "flowercount");
    nFavourCount = HWDictElemInt(dict, "favourcount");
    dictContent = *HWDictElemDict(dict, "content");
    enDeviceType = (HWEnum_Device)HWDictElemInt(dict, "devicetype");
    uIPaddr = HWDictElemInt(dict, "ipaddr");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTDynamicOP>:��̬����״̬
void HWTDynamicOP::FromHWDict( const HWDict* dict )
{
    nFlower = HWDictElemInt(dict, "flower"); // @flower- "1"
    nFavour = HWDictElemInt(dict, "favour"); // @favour- "0"
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTDynamicInfo>:��̬��Ϣ
void HWTDynamicInfo::FromHWDict( const HWDict* dict )
{
    user.FromHWDict(HWDictElemDict(dict, "user"));
    data.FromHWDict(HWDictElemDict(dict, "data"));
    op.FromHWDict(HWDictElemDict(dict, "op"));
}


///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTNoteData>:�ʼ�����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTCommInfo>:��̬������Ϣ




///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTReplyData>:�ظ�����

void HWTReplyData::FromHWDict( const HWDict* dict )
{
    destUser.FromHWDict(HWDictElemDict(dict, "destuser")); // - �ظ�Ŀ���û�
    nId = HWDictElemInt(dict, "id"); // - �ظ�ID
    nType = HWDictElemInt(dict, "type"); // - �ظ�����
    tTime = HWDictElemInt(dict, "time"); // - ����ʱ��
    strText = HWDictElemString(dict, "text"); // - �ظ��ı�
    enDeviceType = (HWEnum_Device)HWDictElemInt(dict, "devicetype"); // - �豸����
    ipAddr = HWDictElemInt(dict, "ipaddr"); // - IP��ַ
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTReplyInfo>:�ظ���Ϣ
void HWTReplyInfo::FromHWDict( const HWDict* dict )
{
    user.FromHWDict(HWDictElemDict(dict, "user")); //     @user  >,
    data.FromHWDict(HWDictElemDict(dict, "data")); //     @data  >
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTScoreData>:��������



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTScoreInfo>:������Ϣ



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookScore>:ͼ������

void HWTBookScore::FromHWDict( const HWDict* dict )
{
    nCount = HWDictElemInt(dict, "count");
    fStars = HWDictElemFloat(dict, "stars");
    nMyStar = HWDictElemInt(dict, "mystar");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookStat>:ͼ��ͳ����Ϣ

void HWTBookStat::FromHWDict( const HWDict* dict )
{
    nCollect = HWDictElemInt(dict, "collect");
    nPostil = HWDictElemInt(dict, "postil");
    nComment = HWDictElemInt(dict, "comment");
    nScore = HWDictElemInt(dict, "score");
    fStars = HWDictElemFloat(dict, "stars");
    nRead = HWDictElemInt(dict, "read");
    nCommend = HWDictElemInt(dict, "commend");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTUserStat>:�û�ͳ����Ϣ

void HWTUserStat::FromHWDict( const HWDict* dict )
{
    nCollect = HWDictElemInt(dict, "collect");
    nAttention = HWDictElemInt(dict, "attention");
    nFans = HWDictElemInt(dict, "fans");
    nPostil = HWDictElemInt(dict, "postil");
    nComment = HWDictElemInt(dict, "comment");
    // nScore = HWDictElemInt(dict, "score");
    nFlower = HWDictElemInt(dict, "flower");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTClassStat>:�༶ͳ��

void HWTClassStat::FromHWDict( const HWDict* dict )
{
    nDuration = HWDictElemInt(dict, "duration");
    nPage = HWDictElemInt(dict, "page");
    nPostil = HWDictElemInt(dict, "postil");
    nComment = HWDictElemInt(dict, "comment");
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTNotifyData>:֪ͨ����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTLetterData>:˽������



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTLetterUserInfo>:˽���û���Ϣ



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTMessageData>:��Ϣ����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTCommendData>:�༶�Ƽ�����

void HWTCommendData::FromHWDict( const HWDict* dict )
{
    tTime = HWDictElemInt(dict, "time");
    strText = HWDictElemString(dict, "text");
    // nCount = HWDictElemInt(dict, "count");
    HWArray* pArr = HWDictElemArray(dict, "books");
    nCount = HWArraySize(pArr);
    for (unsigned int i=0; i<nCount; ++i)
    {
        arrBooks.push_back(HWArrayElemCString(pArr, i));
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTCommendInfo>:�༶�Ƽ���Ϣ



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTCommendBook>:�Ƽ�ͼ��



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTTypeTree>:�����



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTBookInfo>:ͼ����Ϣ

void HWTBookInfo::FromJson( const Json::Value& jv )
{
    strBook = juce::CharPointer_UTF8(jv["guid"].asCString());
    nTypeId = jv["typeid"].asInt();
    nPageCount = jv["pagecount"].asInt();
    nWordCount = jv["wordcount"].asInt();
    nDataSize = jv["datasize"].asInt();
    tPressTime = jv["presstime"].asInt();
    tOnlineTime = jv["onlinetime"].asInt();
    bOnline = jv["bonline"].asInt() == 1;
    strName = juce::CharPointer_UTF8(jv["name"].asCString());
    for (unsigned int i=0; i<jv["authors"].size(); ++i)
    {
        arrAuthors.push_back(juce::CharPointer_UTF8(jv["authors"][i].asCString()));
    }
    strPress = juce::CharPointer_UTF8(jv["press"].asCString());
}

void HWTBookInfo::FromHWDict( const HWDict* dict )
{
    strBook = HWDictElemString(dict, "guid");
    nTypeId = HWDictElemInt(dict, "typeid");
    nPageCount = HWDictElemInt(dict, "pagecount");
    nWordCount = HWDictElemInt(dict, "wordcount");
    nDataSize = HWDictElemInt(dict, "datasize");
    tPressTime = HWDictElemInt(dict, "presstime");
    tOnlineTime = HWDictElemInt(dict, "onlinetime");
    bOnline = HWDictElemBool(dict, "bonline");
    strName = HWDictElemString(dict, "name");

    Json::Value jv;
    Json::Reader jr;
    if (jr.parse(HWDictElemCString(dict, "authors"), jv))
    {
        for (unsigned int i=0; i<jv.size(); ++i)
        {
            arrAuthors.push_back(juce::CharPointer_UTF8(jv[i].asCString()));
        }
    }
    strPress = HWDictElemString(dict, "press");
}

HWTBookInfo& HWTBookInfo::operator=( const HWTBookInfo& info )
{
    strBook = info.strBook;
    nTypeId = info.nTypeId;
    nPageCount = info.nPageCount;
    nWordCount = info.nWordCount;
    nDataSize = info.nDataSize;
    tPressTime = info.tPressTime;
    tOnlineTime = info.tOnlineTime;
    bOnline = info.bOnline;
    strName = info.strName;
    strPress = info.strPress;
    for (unsigned int i=0; i<info.arrAuthors.size(); ++i)
        arrAuthors.push_back(info.arrAuthors[i]);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTCatalog>:ͼ��Ŀ¼
void HWTCatalog::FromJson( const Json::Value& jv )
{
    strTitle = juce::CharPointer_UTF8(jv[0u].asCString());
    nPageNo = jv[1].asInt();
    strShowPage = juce::CharPointer_UTF8(jv[2].asCString());
    const Json::Value& jvArr = jv[3];
    for (unsigned int i=0; i<jvArr.size(); ++i)
    {
        HWTCatalog* pInfo = new HWTCatalog;
        pInfo->FromJson(jvArr[i]);
        vecCatalog.push_back(pInfo);
    }
}

HWTCatalog::~HWTCatalog()
{
    for (unsigned int i=0; i<vecCatalog.size(); ++i)
        delete vecCatalog[i];
    vecCatalog.clear();
}



///////////////////////////////////////////////////////////////////////////////////////////////
// <HWTPageOffSets>:ȫ�ķ�ҳ�ַ�����

//////////////////////////////////////////////////////////////////////////
// <historydata>:�Ķ���ʷ����
void HWTReadHistoryData::FromJson( const Json::Value& jv )
{
    strBook = juce::CharPointer_UTF8(jv["guid"].asCString());
    nPageNo = jv["pageno"].asInt();
    strShowPageNo = juce::CharPointer_UTF8(jv["showpage"].asCString());
    tLastTime = jv["time"].asInt();
}

void HWTReadHistoryData::FromHWDict(const HWDict* dict)
{
	strBook = HWDictElemString(dict, "guid");
	nPageNo = HWDictElemInt(dict, "pageno");
	strShowPageNo = HWDictElemString(dict, "showpage");
	tLastTime = HWDictElemInt(dict, "time");
}

void HWTTextSearchData::FromHWDict( const HWDict* dict )
{
    vecPDFNoInfo.clear();
    vecHiPage.clear();
    mapPageInfo.clear();

    HWArray* pArr;
    HWDict* pDict;

    pArr = HWDictElemArray(dict, "pdfnoinfo");
    for (unsigned int i=0; i<HWArraySize(pArr); ++i)
    {
        vecPDFNoInfo.push_back(HWArrayElemInt(pArr, i));
    }

    pDict = HWDictElemDict(dict, "hlpage");
    if (!pDict->empty())
    {
        HWDict::iterator it = pDict->begin();
        HWDict::iterator itEnd = pDict->end();
        for (; it != itEnd; ++it)
        {
            vecHiPage.push_back(std::pair<juce::String,std::vector<int>>());
            vecHiPage.back().first = juce::CharPointer_UTF8(it->first.c_str());
            std::vector<int>& vec = vecHiPage.back().second;
            pArr = (HWArray*)it->second;
            for (unsigned int i=0; i<HWArraySize(pArr); ++i)
                vec.push_back(HWArrayElemInt(pArr, i));
        }
    }
    
    pDict = HWDictElemDict(dict, "pageinfo");
    if (!pDict->empty())
    {
        HWDict::iterator it = pDict->begin();
        HWDict::iterator itEnd = pDict->end();
        for (; it != itEnd; ++it)
        {
            std::pair<std::map<juce::String,juce::String>,std::vector<int>>& par = mapPageInfo[atoi(it->first.c_str())];
            {
                HWDict* dtContent = HWDictElemDict(it->second, "contexts");
                HWDict::iterator itCnt = dtContent->begin();
                HWDict::iterator itCntEnd = dtContent->end();
                std::map<juce::String,juce::String>& mapContent = par.first;
                for (; itCnt != itCntEnd; ++itCnt)
                {
                    juce::String page = juce::CharPointer_UTF8(itCnt->first.c_str());
                    mapContent[page] = juce::CharPointer_UTF8(HWDictElemString(dtContent, itCnt->first.c_str()));
                }
            }

            pArr = HWDictElemArray(it->second, "offsets");
            for (unsigned int i=0; i<HWArraySize(pArr); ++i)
            {
                par.second.push_back(HWArrayElemInt(pArr, i));
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void HWTIemInfo::FromHWDict( const HWDict* dict )
{
    strCode = HWDictElemString(dict, "code"); // ��code��: ��129621�� //��Ʒ����
    strName = HWDictElemString(dict, "name"); // ��name��:��һ���ڻ�Ա��,									//��Ʒ����
    strDesc = HWDictElemString(dict, "desc"); // ��desc��:�������ѧ������ؼҳ��Ļ�Ա�ʸ��ӳ�һ�ꡱ,		//��Ʒ����
    nPrice = HWDictElemInt(dict, "price"); // ��price��:6800,											//��Ʒ�۸�(��λ����)
    nDiscount = HWDictElemInt(dict, "discount"); // ��discount��:5800										//��Ʒ�ۿۼ�(��λ����)
}
