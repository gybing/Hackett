#include "JuceHeader.h"
#include "HWDocTag.h"

HWParaBookOrder::HWParaBookOrder(int nSwapIdx1, int nSwapIdx2)
{
    HWDocTag::HWBookList* arrBooks = HWDocTag::getInstance()->GetBookList();
    if (arrBooks && !arrBooks->empty())
    {
        if (nSwapIdx1!=-1 && nSwapIdx2!=-1 && nSwapIdx1<arrBooks->size() && nSwapIdx2<arrBooks->size())
        {
            for (unsigned int i=0; i<arrBooks->size(); ++i)
            {
                m_vecIDs.push_back((*arrBooks)[i]->nId);
            }
            int nTmp = m_vecIDs[nSwapIdx1];
            m_vecIDs[nSwapIdx1] = m_vecIDs[nSwapIdx2];
            m_vecIDs[nSwapIdx2] = nTmp;
        }
    }
}

juce_ImplementSingleton(HWDocTag)

HWDocTag::HWDocTag( void )
{
}

HWDocTag::~HWDocTag( void )
{
}

void HWDocTag::OnInit()
{
    m_pTagList = NULL;
    m_pBookList = NULL;
}

void HWDocTag::OnFinal()
{
    SAFE_DELETE_VECTOR_PTR(m_pTagList);
    SAFE_DELETE_VECTOR_PTR(m_pBookList);
}

void* HWDocTag::OnRespPreProcess( HWEnum_ReqID id, const HWParas* paras, const HWDict* dict )
{
    void* result = NULL;
    
    switch (id)
    {
        case HWReq_Tag_List:
        {
            if (dict)
            {
                HWTagList* pList = new HWTagList;
                HWArray* pArrTag = HWDictElemArray(dict, "tags");
                for (unsigned int i=0; i<HWArraySize(pArrTag); ++i)
                {
                    HWTUserTag* pUserTag = new HWTUserTag;
                    pUserTag->FromHWDict(HWArrayElemDict(pArrTag, i));
                    pList->push_back(pUserTag);
                }
                
                // 增加 未分类标签
                HWTUserTag* pUnClassify = new HWTUserTag;
                pUnClassify->nId = 0;
                pUnClassify->strName = L"未分类";
                pList->insert(pList->begin(), pUnClassify);

                result = pList;
            }
            
            break;
        }
        case HWReq_Book_List:
        {
            if (dict)
            {
                HWBookList* pBookList = new HWBookList;
                HWTUserBook* pUserBook;
                
                HWArray* pArrBooks = HWDictElemArray(dict, "books");
                for (unsigned int i=0; i<HWArraySize(pArrBooks); ++i)
                {
                    pUserBook = new HWTUserBook;
                    pUserBook->FromHWDict(HWArrayElemDict(pArrBooks, i));
                    pBookList->push_back(pUserBook);
                }
                
                result = pBookList;
            }
            
            break;
        }
        case HWReq_Book_Collect:
        {
            HWTUserBook* pBook = new HWTUserBook;
            pBook->FromHWDict(HWDictElemDict(dict, "userbook"));
            result = pBook;
            
            break;
        }
            
        case HWReq_Tag_Add:
        {
            HWTUserTag* pTag = new HWTUserTag;
            pTag->FromHWDict(HWDictElemDict(dict, "tag"));
            result = new std::pair<HWTUserTag*,bool>(pTag, paras->BoolVal("bfirst"));
            break;
        }
            
        default:
            break;
    }
    
    return result;
}

void HWDocTag::OnRespProcess( HWEnum_ReqID id, const HWParas* paras, void* data )
{
    switch (id)
    {
        case HWReq_Tag_List:
        {
            SAFE_DELETE_VECTOR_PTR(m_pTagList);
            m_pTagList = (HWTagList*)data;
            break;
        }
        case HWReq_Book_List:
        {
            SAFE_DELETE_VECTOR_PTR(m_pBookList);
            m_pBookList = (HWBookList*)data;
            FOR_EACH_VECTOR(i, (*m_pBookList))
            {
                if (GetUserTagByID((*m_pBookList)[i]->nTag) == NULL)
                    (*m_pBookList)[i]->nTag = 0;
            }
            break;
        }
        case HWReq_Book_Collect:
        {
            if (data)
            {
                HWTUserBook* pBook = (HWTUserBook*)data;
                if (pBook->nTag != 0) pBook->nTag = 0;
                m_pBookList->insert(m_pBookList->begin(), pBook);
                HWNOTIFY(OnBookTagListChange())
                HWNOTIFY(OnBookListChange())
            }
            break;
        }
        case HWReq_Tag_Add:
        {
            jassert(m_pTagList);
            if (data)
            {
                std::pair<HWTUserTag*,bool>* pResult = (std::pair<HWTUserTag*,bool>*)data;
                if (pResult->second)
                    m_pTagList->insert(m_pTagList->begin()+1, pResult->first);
                else
                    m_pTagList->push_back(pResult->first);
                delete pResult;
                HWNOTIFY(OnBookTagListChange())
            }
            break;
        }
        case HWReq_Tag_Del:
        {
            break;
        }
        default:
            break;
    }
}

HWParaUI* HWDocTag::MakeHWParaUI( HWEnum_ReqID reqID, const HWParas* para )
{
    switch (reqID)
    {
        case HWReq_Tag_List:
        {
            HWParaTagList* paraUI = new HWParaTagList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            
        case HWReq_Tag_Add:
        {
            HWParaAddTag* paraUI = new HWParaAddTag();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            
        case HWReq_Tag_Del:
        {
            HWParaDelTag* paraUI = new HWParaDelTag();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        case HWReq_Tag_ModName:
        {
            HWParaTagName* paraUI = new HWParaTagName();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        case HWReq_Tag_ModOrder:
        {
            HWParaTagOrder* paraUI = new HWParaTagOrder();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        case HWReq_Book_List:
        {
            HWParaBookList* paraUI = new HWParaBookList();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            
        case HWReq_Book_ModOrder:
        {
            HWParaBookOrder* paraUI = new HWParaBookOrder();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        case HWReq_Book_Del:
        {
            HWParaBookDel* paraUI = new HWParaBookDel();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        case HWReq_Book_Collect:
        {
            HWParaBookCollect* paraUI = new HWParaBookCollect();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
        case HWReq_Book_MakeTag:
        {
            HWParaBookMakeTag* paraUI = new HWParaBookMakeTag();
            paraUI->FromHWPara(para);
            return paraUI;
        }
            break;
            
        default:
            return NULL;
    }
}

HWDocTag::HWTagList* HWDocTag::GetUserTagList()
{
    return m_pTagList;
}


int HWDocTag::GetUserTagCount()
{
    return m_pTagList ? m_pTagList->size() : 0;
}

void HWDocTag::GetUserBookListFromTag( int nTag, HWDocTag::HWBookList& arrBookList)
{
    if (m_pTagList && m_pBookList)
    {
        for (unsigned int i=0; i<m_pBookList->size(); ++i)
        {
            HWTUserBook* pUserBook = (*m_pBookList)[i];
            if (pUserBook && pUserBook->nTag == nTag)
            {
                arrBookList.push_back(pUserBook);
            }
        }
    }
}

void HWDocTag::GetUserBookListFromTag(int nBegin, int nCount, int nTag, HWDocTag::HWBookList& arrBookList)
{
    if (nBegin < 0 || nCount <= 0) return;
    
    int nIdx=0, nCnt = 0;
    if (m_pTagList && m_pBookList)
    {
        for (unsigned int i=0; i<m_pBookList->size(); ++i)
        {
            HWTUserBook* pUserBook = (*m_pBookList)[i];
            if (pUserBook->nTag == nTag)
            {
                if (nIdx >= nBegin)
                {
                    arrBookList.push_back(pUserBook); ++nCnt;
                    if (nCnt == nCount) return;
                }
                ++nIdx;
            }
        }
    }
}


HWTUserTag* HWDocTag::GetUserTagByID( int nTagID )
{
    if (m_pTagList)
    {
        for (int i=0; i<m_pTagList->size(); ++i)
        {
            if ((*m_pTagList)[i]->nId == nTagID)
            {
                return (*m_pTagList)[i];
            }
        }
    }
    return NULL;
}

HWTUserTag* HWDocTag::GetUserTag(int nIdx)
{
    return ( m_pTagList && nIdx >= 0 && nIdx < m_pTagList->size() ) ? (*m_pTagList)[nIdx] : NULL;
}

HWDocTag::HWBookList* HWDocTag::GetBookList()
{
    return m_pBookList;
}

void HWDocTag::ChangeBookPos(std::vector<int>& vecBookIds, int nDestBookID, int nDestPosInfo)
{
    if (m_pBookList == NULL || vecBookIds.empty())
        return;
    
    std::vector<HWTUserBook*> vecMoveBooks;
    
    for (unsigned int i=0; i<vecBookIds.size(); ++i)
    {
        for (unsigned int c=0; c<m_pBookList->size(); ++c)
        {
            if ((*m_pBookList)[c]->nId == vecBookIds[i])
            {
                vecMoveBooks.push_back((*m_pBookList)[c]);
                m_pBookList->erase(m_pBookList->begin()+c);
                break;
            }
        }
    }
    
    
    for (unsigned int i=0; i<m_pBookList->size(); ++i)
    {
        if ((*m_pBookList)[i]->nId == nDestBookID)
        {
            m_pBookList->insert(m_pBookList->begin() + i + ((nDestPosInfo==0) ? 0 : 1), vecMoveBooks.begin(), vecMoveBooks.end());
            break;
        }
    }
    
    HWNOTIFY(OnBookPosChange());
}

void HWDocTag::SubmitChangeBookPos(HWRequestor* requestor)
{
    if (!m_pBookList) return;
    
    std::vector<int> vecOrder;
    for (unsigned int i=0; i<m_pBookList->size(); ++i)
    {
        vecOrder.push_back((*m_pBookList)[i]->nId);
    }
    
    HWParaBookOrder bookOrderUI(vecOrder);
    SubmitRequest(requestor, HWReq_Book_ModOrder, &bookOrderUI, 0);
}

int HWDocTag::GetTagBookCount(int nTag)
{
    int nCount =0;
    if (m_pBookList)
    {
        for (unsigned int i=0; i<m_pBookList->size(); ++i)
        {
            if ((*m_pBookList)[i]->nTag == nTag)
            {
                ++nCount;
            }
        }
    }
    return nCount;
}

HWTUserBook* HWDocTag::GetUserBook( int nID )
{
    if (m_pBookList)
    {
        for (unsigned int i=0; i<m_pBookList->size(); ++i)
        {
            if ((*m_pBookList)[i]->nId == nID)
            {
                return (*m_pBookList)[i];
            }
        }
    }
    return NULL;
}

HWTUserBook* HWDocTag::GetUserBook(juce::String hBook)
{
    if (m_pBookList)
    {
        for (unsigned int i=0; i<m_pBookList->size(); ++i)
        {
            if ((*m_pBookList)[i]->strBook == hBook)
            {
                return (*m_pBookList)[i];
            }
        }
    }
    return NULL;
}

int HWDocTag::GetUserBookCount()
{
    return m_pBookList ? m_pBookList->size() : 0;
}

bool HWDocTag::LocalGet(HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx)
{
    switch (reqID)
    {
        case HWReq_Tag_List:
        {
            return m_pTagList != NULL;
        }
        case HWReq_Book_List:
        {
            return m_pBookList != NULL;
        }
        case HWReq_Tag_ModOrder:
        case HWReq_Book_Del:
        case HWReq_Tag_Del:
        case HWReq_Book_MakeTag:
        case HWReq_Tag_ModName:
            break;
        default:
            break;
    }
    
    return false;
}

bool HWDocTag::LocalSet( HWRequestor* pRequestor, HWEnum_ReqID reqID, HWParaUI* para, HWContext ctx )
{
    switch (reqID)
    {
        case HWReq_Tag_ModOrder:
        {
            if (m_pTagList)
            {
                HWParaTagOrder* pTagOrder = (HWParaTagOrder*)para;
                
                HWTagList* pNewList = new HWTagList;
                pNewList->push_back((*m_pTagList)[0]);
                
                for (unsigned int i=0; i<pTagOrder->m_arrIDs.size(); ++i)
                {
                    bool bFound = false;
                    for (unsigned int u=0; u<m_pTagList->size(); ++u)
                    {
                        if ((*m_pTagList)[u]->nId == pTagOrder->m_arrIDs[i])
                        {
                            pNewList->push_back((*m_pTagList)[u]);
                            bFound = true;
                            break;
                        }
                    }
                    jassert(bFound == true);
                }
                
                HWTagList* pOldList = m_pTagList;
                m_pTagList = pNewList;
                
                pOldList->clear();
                delete pOldList;
            }
            break;
        }
            
        case HWReq_Book_Del:
        {
            HWParaBookDel* bookDel = (HWParaBookDel*)para;
            
            HWBookList::iterator iter = m_pBookList->begin();
            while (iter != m_pBookList->end())
            {
                bool bNotFound = true;
                for (unsigned int i=0; i<bookDel->m_vecIDs.size(); ++i)
                {
                    if (bookDel->m_vecIDs[i] == (*iter)->nId)
                    {
                        iter = m_pBookList->erase(iter);
                        bNotFound = false;
                        break;
                    }
                }
                if (bNotFound) ++iter;
            }
            HWNOTIFY(OnBookTagListChange())
            HWNOTIFY(OnBookDelete(bookDel->m_vecIDs));
            break;
        }
            
        case HWReq_Tag_Del:
        {
            if (m_pTagList)
            {
                HWParaDelTag* pDelTag = (HWParaDelTag*)para;
                for (unsigned int i=0; i<m_pTagList->size(); ++i)
                {
                    if ((*m_pTagList)[i]->nId == pDelTag->m_nID)
                    {
                        delete (*m_pTagList)[i];
                        m_pTagList->erase(m_pTagList->begin()+i);
                        
                        if (m_pBookList)
                        {
                            for (unsigned int i=0; i<m_pBookList->size(); ++i)
                            {
                                if ((*m_pBookList)[i]->nTag == pDelTag->m_nID)
                                {
                                    (*m_pBookList)[i]->nTag = 0;
                                }
                            }
                        }
                        HWNOTIFY(OnBookTagListChange())
                        HWNOTIFY(OnBookListChange())
                        break;
                    }
                }
            }
            break;
        }
        case HWReq_Book_MakeTag:
        {
            if (m_pBookList)
            {
                HWParaBookMakeTag* bookTag = (HWParaBookMakeTag*)para;
                
                for (unsigned int i=0; i<m_pBookList->size(); ++i)
                {
                    if ((*m_pBookList)[i]->nId == bookTag->m_nID)
                    {
                        (*m_pBookList)[i]->nTag = bookTag->m_nTagID;
                        HWNOTIFY(OnBookTagListChange())
                        HWNOTIFY(OnBookListChange())
                        break;
                    }
                }
            }
            
            break;
        }
        case HWReq_Tag_ModName:
        {
            if (m_pTagList)
            {
                HWParaTagName* tagName = (HWParaTagName*)para;
                for (unsigned int i=0; i<m_pTagList->size(); ++i)
                {
                    if ((*m_pTagList)[i]->nId == tagName->m_nID)
                    {
                        (*m_pTagList)[i]->strName = tagName->m_strName;
                        break;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    
    return true;
}

bool HWDocTag::IsTagNameExist( juce::String& strName )
{
    if (m_pTagList)
    {
        for (unsigned int i=0; i<m_pTagList->size(); ++i)
        {
            if ((*m_pTagList)[i]->strName == strName)
            {
                return true;
            }
        }
    }
    return false;
}

