#include "JuceHeader.h"
#include "HWDocDynamic.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocDynamic)
HWDocDynamic::HWDocDynamic()
{

}

HWDocDynamic::~HWDocDynamic()
{

}

void HWDocDynamic::OnInit()
{
    m_nBookNoteListCount = 0;
    m_nUserNoteListCount = 0;
    m_pVecBookNoteList = NULL;
    m_pVecUserNoteList = NULL;
    m_pDynamicInfo = NULL;
    m_pReplayInfo = NULL;
    m_nDynamicReplyCount = 0;
    m_pVecDynamicReplyList = NULL;
    m_pDynamicReply = NULL;
    m_pDynamicReplyReply = NULL;
    m_pFavourResult = NULL;
    m_pFlowerResult = NULL;
}

void HWDocDynamic::OnFinal()
{
    m_nBookNoteListCount = 0;
    m_nUserNoteListCount = 0;
    SAFE_DELETE(m_pVecBookNoteList);
    SAFE_DELETE(m_pVecUserNoteList);
    SAFE_DELETE(m_pDynamicInfo);
    SAFE_DELETE(m_pReplayInfo);
    m_nDynamicReplyCount = 0;
    SAFE_DELETE(m_pVecDynamicReplyList);
    SAFE_DELETE(m_pDynamicReply);
    SAFE_DELETE(m_pDynamicReplyReply);
    SAFE_DELETE(m_pFavourResult);
    SAFE_DELETE(m_pFlowerResult);
}

void* HWDocDynamic::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;
    switch (id)
    {
    case HWReq_Dynamic_UserNote:
    case HWReq_Dynamic_BookNote:
        {
            std::vector<std::pair<int,int>>* pVecIds = new std::vector<std::pair<int,int>>;
            int nCount = HWDictElemInt(dict, "count");
            HWArray* pArrDycs = HWDictElemArray(dict, "dynamics");
            for (unsigned int i=0; i<HWArraySize(pArrDycs); ++i)
            {
                pVecIds->push_back(std::pair<int,int>(HWDictElemInt(HWArrayElemDict(pArrDycs, i), "id"),HWDictElemInt(HWArrayElemDict(pArrDycs, i), "type")));
            }
            result = new std::pair<int, std::vector<std::pair<int,int>>*>(nCount, pVecIds);
            break;
        }
    case HWReq_Dynamic_Info:
        {
            HWTDynamicInfo* pInfo = new HWTDynamicInfo;
            pInfo->FromHWDict(HWDictElemDict(dict, "dynamicinfo"));
            result =  pInfo;
        }
        break;
    case HWReq_Dynamic_ReplyInfo:
        {
            HWTReplyInfo* pInfo = new HWTReplyInfo;
            pInfo->FromHWDict(HWDictElemDict(dict, "replyinfo"));
            result = pInfo;
        }
        break;
    case HWReq_Dynamic_ReplyList:
        {
            int nCount = HWDictElemInt(dict, "count");
            std::vector<int>* pVecReplyList = new std::vector<int>;
            HWArray* pArray = HWDictElemArray(dict, "replys");
            for (unsigned int i=0; i<HWArraySize(pArray); ++i)
            {
                pVecReplyList->push_back(HWArrayElemInt(pArray, i));
            }
            result = new std::pair<int, std::vector<int>*>(nCount, pVecReplyList);
        }
        break;
    case HWReq_Dynamic_Reply:
        {
            HWDynamicReply* pReply = new HWDynamicReply;
            pReply->nID = HWDictElemInt(dict, "id");
            pReply->nReplyCount = HWDictElemInt(dict, "replycount");
            pReply->nReviewCount = HWDictElemInt(dict, "reviewcount");
            pReply->replyInfo.FromHWDict(HWDictElemDict(dict, "replyinfo"));
            result = pReply;
        }
        break;
    case HWReq_Dynamic_ReplyReply:
        {
            HWDinamicReplyReply* pReplyReply = new HWDinamicReplyReply;
            pReplyReply->nID = HWDictElemInt(dict, "id");
            pReplyReply->nReplyCount = HWDictElemInt(dict, "replycount");
            pReplyReply->replyInfo.FromHWDict(HWDictElemDict(dict, "replyinfo"));
            result = pReplyReply;
        }
        break;
    case HWReq_Dynamic_Favour:
        {
            std::pair<int,int>* pFavour = new std::pair<int,int>;
            pFavour->first = HWDictElemInt(dict, "id");
            pFavour->second = HWDictElemInt(dict, "favourcount");
            result = pFavour;
        }
        break;
    case HWReq_Dynamic_Flower:
        {
            std::pair<int,int>* pFlower = new std::pair<int,int>;
            pFlower->first = HWDictElemInt(dict, "id");
            pFlower->second = HWDictElemInt(dict, "flowercount");
            result = pFlower;
        }
        break;
    case HWReq_Dynamic_Del:
        {
        }
        break;
    default:
        break;
    }
    return result;
}

void HWDocDynamic::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Dynamic_UserNote:
        {
            SAFE_DELETE(m_pVecUserNoteList);
            if (data)
            {
                std::pair<int, std::vector<std::pair<int,int>>*>* pRst = (std::pair<int, std::vector<std::pair<int,int>>*>*)data;
                m_nUserNoteListCount = pRst->first;
                m_pVecUserNoteList = pRst->second;
            }
        }
        break;
    case HWReq_Dynamic_BookNote:
        {
            SAFE_DELETE(m_pVecBookNoteList);
            if (data)
            {
                std::pair<int, std::vector<std::pair<int,int>>*>* pRst = (std::pair<int, std::vector<std::pair<int,int>>*>*)data;
                m_nBookNoteListCount = pRst->first;
                m_pVecBookNoteList = pRst->second;
            }
        }
        break;
    case HWReq_Dynamic_Info:
        {
            SAFE_DELETE(m_pDynamicInfo);
            if (data) m_pDynamicInfo = (HWTDynamicInfo*)data;
        }
        break;
    case HWReq_Dynamic_ReplyInfo:
        {
            SAFE_DELETE(m_pReplayInfo);
            if (data) m_pReplayInfo = (HWTReplyInfo*)data;
        }
        break;
    case HWReq_Dynamic_ReplyList:
        {
            SAFE_DELETE(m_pVecDynamicReplyList);
            if (data)
            {
                std::pair<int, std::vector<int>*>* pair = (std::pair<int, std::vector<int>*>*)data;
                m_nDynamicReplyCount = pair->first;
                m_pVecDynamicReplyList = pair->second;
                delete pair;
            }
        }
        break;
    case HWReq_Dynamic_Reply:
        {
            SAFE_DELETE(m_pDynamicReply);
            if (data) m_pDynamicReply = (HWDynamicReply*)data;
        }
        break;
    case HWReq_Dynamic_ReplyReply:
        {
            SAFE_DELETE(m_pDynamicReplyReply);
            if (data) m_pDynamicReplyReply = (HWDinamicReplyReply*)data;
        }
        break;
    case HWReq_Dynamic_Favour:
        {
            SAFE_DELETE(m_pFavourResult);
            if (data) m_pFavourResult = (std::pair<int,int>*)data;
        }
        break;
    case HWReq_Dynamic_Flower:
        {
            SAFE_DELETE(m_pFlowerResult);
            if (data) m_pFlowerResult = (std::pair<int,int>*)data;
        }
        break;
    case HWReq_Dynamic_Del:
        break;
    default:
        break;
    }
}

bool HWDocDynamic::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocDynamic::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Dynamic_BookNote:
        {
            HWParaBookNoteList* paraUI = new HWParaBookNoteList();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_UserNote:
        {
            HWParaUserNoteList* paraUI = new HWParaUserNoteList();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_Info:
        {
            HWParaDynamicInfo* paraUI = new HWParaDynamicInfo();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_ReplyInfo:
        {
            HWParaDynamicReplyInfo* paraUI = new HWParaDynamicReplyInfo();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_ReplyList:
        {
            HWParaDynamicReplyList* paraUI = new HWParaDynamicReplyList();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_Reply:
        {
            HWParaDynamicReply* paraUI = new HWParaDynamicReply();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_ReplyReply:
        {
            HWParaDynamicReplyReply* paraUI = new HWParaDynamicReplyReply();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_Favour:
        {
            HWParaDynamicZan* paraUI = new HWParaDynamicZan();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_Flower:
        {
            HWParaDynamicFlower* paraUI = new HWParaDynamicFlower();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    case HWReq_Dynamic_Del:
        {
            HWParaDynamicDel* paraUI = new HWParaDynamicDel();
            paraUI->FromHWPara(para);
            return paraUI;	
        }
    default:
        return NULL;
    }
}

int HWDocDynamic::GetBookNoteCount() const
{
    return m_nBookNoteListCount;
}

void HWDocDynamic::GetBookNoteList( std::vector<std::pair<int,int>>& vec )
{
    if (!vec.empty()) vec.clear();

    if (m_pVecBookNoteList)
    {
        vec = *m_pVecBookNoteList;
    }
}

int HWDocDynamic::GetUserNoteCount() const
{
    return m_nUserNoteListCount;
}

void HWDocDynamic::GetUserNoteList( std::vector<std::pair<int,int>>& vec )
{
    if (!vec.empty()) vec.clear();

    if (m_pVecUserNoteList)
    {
        vec = *m_pVecUserNoteList;
    }
}


HWTDynamicInfo* HWDocDynamic::GetDynamicInfo()
{
    return m_pDynamicInfo;
}

int HWDocDynamic::GetDynamicReplyCount()
{
    return m_nDynamicReplyCount;
}

std::vector<int>* HWDocDynamic::GetDynamicReplyList()
{
    return m_pVecDynamicReplyList;
}

HWTReplyInfo* HWDocDynamic::GetDynamicReplyInfo()
{
    return m_pReplayInfo;
}

std::pair<int,int>* HWDocDynamic::GetFavourResult()
{
    return m_pFavourResult;
}

std::pair<int,int>* HWDocDynamic::GetFlowerResult()
{
    return m_pFlowerResult;
}
