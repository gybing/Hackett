#include "JuceHeader.h"
#include "HWDocRead.h"

#include "KnlBlock.h"

juce_ImplementSingleton(HWDocRead)

HWDocRead::HWDocRead()
{
}

HWDocRead::~HWDocRead()
{
}

void HWDocRead::OnInit()
{
    m_strBook = "";
    m_nHistoryPageNo = 0;
    m_pReadHistoryList = NULL;
    m_pCatalog = NULL;
    m_pReadBase = NULL;
    m_pstrBrief = NULL;
    m_bIsBookOpened = false;
    m_fAspectRatio = 1.0f;
}

void HWDocRead::OnFinal()
{
    m_strBook.clear();
    m_nHistoryPageNo = 0;
    SAFE_DELETE_VECTOR_PTR(m_pReadHistoryList);
    SAFE_DELETE(m_pCatalog);
    SAFE_DELETE(m_pReadBase);
    SAFE_DELETE(m_pstrBrief);
    m_bIsBookOpened = false;
}

const juce::String& HWDocRead::GetBook()
{
    return m_strBook;
}

int HWDocRead::GetHistoryPageNo()
{
    return m_nHistoryPageNo;
}

std::vector<HWTReadHistoryData*>* HWDocRead::GetHistoryList()
{
    return m_pReadHistoryList;
}

void* HWDocRead::OnRespPreProcess(HWEnum_ReqID id, const HWParas* paras, const HWDict* dict)
{
    void* result = NULL;

    switch (id)
    {
    case HWReq_Read_OpenBook:
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
                (*mapResult)[L"pageno"] = (void*)HWDictElemInt(dict, "pageno");
                result = mapResult;
            }
            break;
        }
    case HWReq_Read_HistoryList:
        {
            if (dict)
            {
				HWArray* pArrHis = HWDictElemArray(dict, "historylist");
				std::vector<HWTReadHistoryData*>* history = new std::vector<HWTReadHistoryData*>;
				for (unsigned int i=0; i<HWArraySize(pArrHis); ++i)
				{
					HWTReadHistoryData* his = new HWTReadHistoryData;
					his->FromHWDict(HWArrayElemDict(pArrHis, i));
					history->push_back(his);
				}
                result = history;
            }
            break;
        }
    case HWReq_Read_OnePage:
    case HWReq_Read_BookStat:
    case HWReq_Read_DelHistory:
        break;
    default:break;
    }

    return result;
}

void HWDocRead::OnRespProcess(HWEnum_ReqID id, const HWParas* paras, void* data)
{
    switch (id)
    {
    case HWReq_Read_OpenBook:
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
                m_nHistoryPageNo = (juce::int64)(*mapResult)[L"pageno"];
                m_strBook = paras->StrVal("guid");
                m_bIsBookOpened = true;

                m_fAspectRatio = (m_pReadBase->nPDFHeight*1.0f) / (m_pReadBase->nPDFWidth*1.0f);
                delete mapResult;
            }

            if (m_pReadHistoryList)
            {
                for (unsigned int j=0; j<m_pReadHistoryList->size(); ++j)
                {
                    if ((*m_pReadHistoryList)[j]->strBook == m_strBook)
                    {
                        delete (*m_pReadHistoryList)[j];
                        m_pReadHistoryList->erase(m_pReadHistoryList->begin() + j);
                        break;
                    }
                }

                HWTReadHistoryData* pReadData = new HWTReadHistoryData();
                pReadData->nPageNo = m_nHistoryPageNo;
                pReadData->strBook = m_strBook;
                juce::String strShow;
                GetShowPageNo(m_nHistoryPageNo, strShow);
                pReadData->strShowPageNo = strShow;
                pReadData->tLastTime = time(NULL);
                m_pReadHistoryList->insert(m_pReadHistoryList->begin(), pReadData);
            }
            HWNOTIFY(OnBookOpened());
            break;
        }
    case HWReq_Read_HistoryList:
        {
            SAFE_DELETE_VECTOR_PTR(m_pReadHistoryList);
            if (data) m_pReadHistoryList = (std::vector<HWTReadHistoryData*>*)data;
            break;
        }
    case HWReq_Read_BookStat:
    case HWReq_Read_OnePage:
    case HWReq_Read_DelHistory:
    default:break;
    }
}

bool HWDocRead::LocalGet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    return false;
}

HWParaUI* HWDocRead::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
    case HWReq_Read_OpenBook:
        {
            HWParaOpenBook* paraUI = new HWParaOpenBook();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Read_HistoryList:
        {
            HWParaReadHistoryList* paraUI = new HWParaReadHistoryList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Read_BookStat:
        {
            HWParaReadBookStat* paraUI = new HWParaReadBookStat();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Read_OnePage:
        {
            HWParaReadOnePage* paraUI = new HWParaReadOnePage();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    case HWReq_Read_DelHistory:
        {
            HWParaReadDelHistory* paraUI = new HWParaReadDelHistory();
            paraUI->FromHWPara(para);
            return paraUI;
        }
    default:
        return NULL;
    }
}

int HWDocRead::GetPDFPageCount()
{
    jassert(m_pReadBase);
    return m_pReadBase->nPDFPageCount;
}

int HWDocRead::GetPDFWidth()
{
    jassert(m_pReadBase);
    return m_pReadBase->nPDFWidth;
}

int HWDocRead::GetPDFHeight()
{
    jassert(m_pReadBase);
    return m_pReadBase->nPDFHeight;
}

bool HWDocRead::IsBookOpened()
{
    return m_bIsBookOpened;
}

int HWDocRead::GetHistoryCount()
{
    if (m_pReadHistoryList)
        return m_pReadHistoryList->size();
    return 0;
}

HWTReadHistoryData* HWDocRead::GetHistoryData(unsigned int nIdx )
{
    if (m_pReadHistoryList && nIdx < m_pReadHistoryList->size())
    {
        return (*m_pReadHistoryList)[nIdx];
    }
    return NULL;
}

juce::String* HWDocRead::GetBookBrief()
{
    return m_pstrBrief;
}

const HWTCatalog* HWDocRead::GetCatalog(int nIdx, bool bAll, int& nDeep)
{
    if (m_pCatalog)
    {
        if (bAll)
        {
            int nLoop = -1; nDeep = -1;
            return GetCatalog(nIdx, nLoop, nDeep, m_pCatalog);
        }
        else
        {
            if (nIdx >= 0 && nIdx < m_pCatalog->vecCatalog.size())
                return m_pCatalog->vecCatalog[nIdx];
        }
    }

    return NULL;
}

const HWTCatalog* HWDocRead::GetCatalog(int& nSrc, int& nIdx, int& nDeep, const HWTCatalog* pCatalog )
{
    if (nSrc == nIdx++) return pCatalog;
    if (pCatalog->vecCatalog.empty()) return NULL;

    ++nDeep;

    const HWTCatalog* cfg = NULL;
    FOR_EACH_VECTOR(i, pCatalog->vecCatalog)
    {
        cfg = GetCatalog(nSrc, nIdx, nDeep, pCatalog->vecCatalog[i]);
        if (cfg) return cfg;
    }

    --nDeep;
    return NULL;
}

bool HWDocRead::HasChildCatalog()
{
    int nCnt = m_pCatalog->vecCatalog.size();
    FOR_EACH_VECTOR(i, m_pCatalog->vecCatalog)
    {
        FOR_EACH_VECTOR(j, m_pCatalog->vecCatalog[i]->vecCatalog)
        {
            return true;
        }
    }
    return false;
}

int HWDocRead::GetCatalogCount(bool bAll /*= false*/)
{
    if (m_pCatalog)
    {
        if ( bAll ) return GetCatalogCount(m_pCatalog);
        else return m_pCatalog->vecCatalog.size();
    }
    return 0;
}

int HWDocRead::GetCatalogCount( HWTCatalog* pCatalog )
{
    int nCnt = pCatalog->vecCatalog.size();
    FOR_EACH_VECTOR(i, pCatalog->vecCatalog)
    {
        nCnt += GetCatalogCount(pCatalog->vecCatalog[i]);
    }
    return nCnt;
}

int HWDocRead::GetPDFPageNo( const wchar_t* strShowPage )
{
    int nShowNo = juce::String(strShowPage).getIntValue();

    if (!IsBookOpened())
        return PAGE_INVALID;

    if (!m_pReadBase || m_pReadBase->jvPDFNoInfo.empty())
        return PAGE_INVALID;

    Json::Value& jvPDFNoInfo = m_pReadBase->jvPDFNoInfo;
    jassert(jvPDFNoInfo.isArray());

    unsigned int uSize = jvPDFNoInfo.size()/5;//5Œ™“ª∏ˆ–≈œ¢∂Œ
    for (unsigned int uCnt = 0 ; uCnt < uSize; ++uCnt) 
    {
        unsigned int nFormat = (jvPDFNoInfo[uCnt*5+0]).asUInt();
        if (nFormat != 0) continue;

        double rate = (jvPDFNoInfo[uCnt*5+1]).asDouble();
        unsigned int   startPageno = (jvPDFNoInfo[uCnt*5+2]).asUInt();
        unsigned int   nPdfStart = (jvPDFNoInfo[uCnt*5+3]).asUInt();
        unsigned int   nPdfEnd = (jvPDFNoInfo[uCnt*5+4]).asUInt();

        int nOffset = (rate > 0) ? (rate * (nPdfEnd - nPdfStart + 1)) : ((nPdfEnd - nPdfStart) / abs(rate));
        if (nShowNo >= startPageno && nShowNo < startPageno + nOffset)
        {
            int no = 0;

            if (nPdfStart == nPdfEnd)
            {
                no = nPdfStart;
            }
            else 
            {
                no =  (rate > 0) ? int(nShowNo/rate) : int(nShowNo*abs(rate));
                no = (no - startPageno) + nPdfStart;
            }

            return no;
        }
    }
    
    return PAGE_INVALID;
}

bool HWDocRead::GetShowPageNo( int nPDFNo, juce::String& strShow )
{
    strShow = L"非正文";

    if (!m_pReadBase || m_pReadBase->jvPDFNoInfo.empty())
        return false;

    Json::Value& jvPDFNoInfo = m_pReadBase->jvPDFNoInfo;
    jassert(jvPDFNoInfo.isArray());

    unsigned int uSize = jvPDFNoInfo.size()/5;
    for (unsigned int uCnt = 0 ; uCnt < uSize; ++uCnt) //5Œ™“ª∏ˆ–≈œ¢∂Œ
    {
        const Json::Value& valFormat		 = jvPDFNoInfo[uCnt*5+0];
        const Json::Value& valPageRate		 = jvPDFNoInfo[uCnt*5+1];
        const Json::Value& valStartPageNo	 = jvPDFNoInfo[uCnt*5+2];
        const Json::Value& valPdfStartPageNo = jvPDFNoInfo[uCnt*5+3];
        const Json::Value& valPdfEndPageNo   = jvPDFNoInfo[uCnt*5+4];
        if (nPDFNo >= (int)valPdfStartPageNo.asUInt() && nPDFNo <= (int)valPdfEndPageNo.asUInt())
        {
            unsigned int no = nPDFNo - valPdfStartPageNo.asUInt();
            double rate = valPageRate.asDouble();
            if (rate > 0) 
                no = (no * rate) + valStartPageNo.asUInt();

            switch (valFormat.asInt())
            {
            case 0: //
                {
                    strShow.clear();
                    strShow << (int)no;
                }
                break;
            case 1://
                HWDocHelper::getInstance()->GetRomanNum(no, strShow);
                break;
            case 2://
                HWDocHelper::getInstance()->GetChineseNum(no, strShow);
                break;
            }
            
            return true;
        }
    }

    return false;
}

bool HWDocRead::LocalSet(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx)
{
    switch (reqID)
    {
    case HWReq_Read_OnePage:
        {
            HWParaReadOnePage* paraUI = (HWParaReadOnePage*)para;
            m_nHistoryPageNo = paraUI->m_nPageNo;
            if (m_pReadHistoryList)
            {
                for (unsigned int j=0; j<m_pReadHistoryList->size(); ++j)
                {
                    if ((*m_pReadHistoryList)[j]->strBook == paraUI->m_strGuid)
                    {
                        delete (*m_pReadHistoryList)[j];
                        m_pReadHistoryList->erase(m_pReadHistoryList->begin() + j);
                        break;
                    }
                }

                HWTReadHistoryData* pReadData = new HWTReadHistoryData();
                pReadData->nPageNo = paraUI->m_nPageNo;
                pReadData->strBook = paraUI->m_strGuid;
                juce::String strShow;
                GetShowPageNo(paraUI->m_nPageNo, strShow);
                pReadData->strShowPageNo = strShow;
                pReadData->tLastTime = time(NULL);
                m_pReadHistoryList->insert(m_pReadHistoryList->begin(), pReadData);
                HWNOTIFY(OnReadOnePage());
            }

            break;
        }
    case HWReq_Read_DelHistory:
        {
            if (m_pReadHistoryList)
            {
                HWParaReadDelHistory* paraUI = (HWParaReadDelHistory*)para;
                for (unsigned int i=0; i<paraUI->m_vecBooks.size(); ++i)
                {
                    for (unsigned int j=0; j<m_pReadHistoryList->size(); ++j)
                    {
                        if (paraUI->m_vecBooks[i] == (*m_pReadHistoryList)[j]->strBook)
                        {
                            delete (*m_pReadHistoryList)[j];
                            m_pReadHistoryList->erase(m_pReadHistoryList->begin() + j);
                        }
                    }
                }       
                HWNOTIFY(OnReadHistoryDel());
            }
            break;
        }
    default:
        break;
    }

    return true;
}

bool HWDocRead::IsValidBookData()
{
    return (m_pReadBase->nPDFPageCount > 0) &&
        m_pReadBase->nPDFWidth > 0 &&
        m_pReadBase->nPDFHeight > 0;
}

float HWDocRead::GetPDFAspectRatio()
{
    return m_fAspectRatio;
}
