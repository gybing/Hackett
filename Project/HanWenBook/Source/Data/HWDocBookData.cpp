#include "JuceHeader.h"
#include "HWDocBookData.h"
#include "HWLog.h"

juce_ImplementSingleton(HWDocBookData)

HWDocBookData::HWDocBookData()
{

}

HWDocBookData::~HWDocBookData()
{
}

void HWDocBookData::OnInit()
{
    m_pBookInfo = NULL;
    m_pBitmap = NULL;
    m_pCatalog = NULL;
    m_pReadBase = NULL;
    m_pstrBrief = NULL;
    m_pstrTextGet = NULL;
    m_pTextSearchData = NULL;
    m_pBookScore = NULL;
    m_nSearchPage = -1;
}

void HWDocBookData::OnFinal()
{
    SAFE_DELETE(m_pBitmap);
    SAFE_DELETE(m_pBookInfo);
    SAFE_DELETE(m_pCatalog);
    SAFE_DELETE(m_pReadBase);
    SAFE_DELETE(m_pstrBrief);
    SAFE_DELETE(m_pstrTextGet);
    SAFE_DELETE(m_pTextSearchData);
    SAFE_DELETE(m_pBookScore);
}

void* HWDocBookData::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_Book_Info:
    {
        if (dict)
        {
            HWTBookInfo* pBookInfo = new HWTBookInfo;
            pBookInfo->FromHWDict(HWDictElemDict(dict, "bookinfo"));
            result = pBookInfo;
        }
        break;
    }
    case HWReq_Pic_Download:
    {
        HWImg* pImage = HWDictElemImage(dict, "img");
		if (pImage->Length() > 0)
		{
			result = new juce::Image(juce::ImageFileFormat::loadFrom(pImage->Value(), pImage->Length()));
		}

        break;
    }
    case HWReq_Book_Res:
    {
        if (dict)
        {
            std::map<juce::String, void*>* mapResult = new std::map<juce::String, void*>;
            const HWDict* res = HWDictElemDict(dict, "res");

            HWTCatalog* pCatalog = new HWTCatalog;
            HWTReadBase* pReadBase = new HWTReadBase;
            juce::String* pstrBrief = new juce::String;

            Json::Value jv;
            Json::Reader jr;
            if (jr.parse(HWDictElemCString(res, "catalog"), jv))
                pCatalog->FromJson(jv);
            if (jr.parse(HWDictElemCString(res, "readbase"), jv))
                pReadBase->FromJson(jv);
            *pstrBrief = HWDictElemString(res, "brief");

            (*mapResult)[L"catalog"] = pCatalog;
            (*mapResult)[L"readbase"] = pReadBase;
            (*mapResult)[L"brief"] = pstrBrief;
            result = mapResult;
        }
        break;
    }
    case HWReq_Text_Get:
        {
            juce::String* pStrText = new juce::String;
            juce::String strText = HWDictElemString(dict, "text");
            if (!strText.isEmpty())
                *pStrText = strText;
            result = pStrText;
            break;
        }
    case HWReq_Search_Text:
        {
            if (dict)
            {
                HWTTextSearchData* pTextSearchData = new HWTTextSearchData;
                pTextSearchData->FromHWDict(dict);
                result = pTextSearchData;
            }
            break;
        }
    case HWReq_BookScore_Get:
    case HWReq_BookScore_Set:
        {
            HWTBookScore* pBookScore = new HWTBookScore;
            pBookScore->FromHWDict(HWDictElemDict(dict, "score"));
            result = pBookScore;
        }
    default:
        break;
    }
    return result;
}

void HWDocBookData::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Book_Info:
    {
        SAFE_DELETE(m_pBookInfo);
        m_pBookInfo = (HWTBookInfo*)data;
        break;
    }
    case HWReq_Pic_Download:
    {
		SAFE_DELETE(m_pBitmap);
		if (data)
		{
			m_pBitmap = (juce::Image*)data;
		}

        break;
    }
    case HWReq_Book_Res:
    {
        SAFE_DELETE(m_pCatalog);
        SAFE_DELETE(m_pReadBase);
        SAFE_DELETE(m_pstrBrief);

        if (data)
        {
            std::map<juce::String, void*>* mapResult = (std::map<juce::String, void*>*)data;
            m_pCatalog = (HWTCatalog*)(*mapResult)[L"catalog"];
            m_pReadBase = (HWTReadBase*)(*mapResult)[L"readbase"];
            m_pstrBrief = (juce::String*)(*mapResult)[L"brief"];
            delete mapResult;
        }
        break;
    }
    case HWReq_Text_Get:
        {
            SAFE_DELETE(m_pstrTextGet);
            if (data) m_pstrTextGet = (juce::String*)data;
            break;
        }
    case HWReq_Search_Text:
        {
            SAFE_DELETE(m_pTextSearchData);
            if (data) m_pTextSearchData = (HWTTextSearchData*)data;
            break;
        }
    case HWReq_BookScore_Get:
    case HWReq_BookScore_Set:
        {
            SAFE_DELETE(m_pBookScore);
            if (data) m_pBookScore = (HWTBookScore*)data;
            break;
        }
    default:
        break;
    }
}

bool HWDocBookData::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    switch (reqID)
    {
    case HWReq_Book_Info:
        {
            HWParaBookInfo* par = (HWParaBookInfo*)para;
            if (m_pBookInfo && par->m_strBook == m_pBookInfo->strBook)
            {
                return true;
            }
            break;
        }
    case HWReq_Pic_Download:
    case HWReq_Book_Res:
    case HWReq_Text_Get:
    case HWReq_Search_Text:
    case HWReq_BookScore_Get:
    case HWReq_BookScore_Set:
    default:
        break;
    }
    return false;
}

HWParaUI* HWDocBookData::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Book_Info:
        {
            HWParaBookInfo* paraUI = new HWParaBookInfo();
            paraUI->FromHWPara(para);
            return paraUI;
        }
        break;

    case HWReq_Book_Res:
        {
            HWParaBookRes* paraUI = new HWParaBookRes();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Pic_Download:
        {
            HWParaBookPic* paraUI = new HWParaBookPic;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Text_Get:
        {
            HWParaBookTextGet* paraUI = new HWParaBookTextGet;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Search_Text:
        {
            HWParaTextSearch* paraUI = new HWParaTextSearch;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_BookScore_Set:
        {
            HWParaBookScoreSet* paraUI = new HWParaBookScoreSet;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_BookScore_Get:
        {
            HWParaBookTextGet* paraUI = new HWParaBookTextGet;
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

void HWDocBookData::OnReqFinish( HWEnum_ReqID id, HWEnum_ErrorID err, HWEnum_BusiID busi, const HWParas* paras, HWRequestor* pRequestor, HWContext ctx )
{
    HWDocBase::OnReqFinish(id, err, busi, paras, pRequestor, ctx);
    if (id == HWReq_Pic_Download)
    {
        SAFE_DELETE(m_pBitmap);
    }
}

const HWTBookInfo* HWDocBookData::GetBookInfo()
{
    return m_pBookInfo;
}

juce::Image* HWDocBookData::GetBookImage()
{
    return m_pBitmap;
}

juce::String* HWDocBookData::GetBookBrief()
{
    return m_pstrBrief;
}

juce::String* HWDocBookData::GetTextSrc()
{
    return m_pstrTextGet;
}

HWTTextSearchData* HWDocBookData::GetTextSearchData()
{
    return m_pTextSearchData;
}

bool HWDocBookData::GetTextSearchData(int nPageNo, std::vector<int>& vecOffsets)
{
    if (m_pTextSearchData && m_nSearchPage==nPageNo)
    {
        std::map<int, std::pair<std::map<juce::String,juce::String>,std::vector<int>>>::iterator it = m_pTextSearchData->mapPageInfo.find(nPageNo);
        if (it != m_pTextSearchData->mapPageInfo.end())
        {
            for (unsigned int i=0; i<it->second.second.size(); ++i)
            {
                vecOffsets.push_back(it->second.second[i]);
            }
            return true;
        }
    }
    return false;
}

HWTBookScore* HWDocBookData::GetBookScore()
{
    return m_pBookScore;
}

void HWDocBookData::SetSearchPageNo(int nPageNo)
{
    m_nSearchPage = nPageNo;
}

