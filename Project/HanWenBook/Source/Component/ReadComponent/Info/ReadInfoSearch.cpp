#include "JuceHeader.h"
#include "ReadInfoSearch.h"

class ReadInfoSearch::BookSignItem
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    BookSignItem(ReadInfoSearch& r)
        : owner(r)
        , rowNumber(-1)
    {
        addAndMakeVisible(buttonDelete = new juce::TextButton(L"删除"));
        addAndMakeVisible(labelTitle = new juce::Label);
        addAndMakeVisible(labelCreateTime = new juce::Label);
        labelTitle->setFont(juce::Font(18));
        labelTitle->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelTitle->setJustificationType(juce::Justification::centredLeft);
        labelTitle->setMinimumHorizontalScale(1.0f);
        labelTitle->setInterceptsMouseClicks(false, false);

        labelCreateTime->setFont(juce::Font(18));
        labelCreateTime->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelCreateTime->setJustificationType(juce::Justification::centredRight);
        labelCreateTime->setInterceptsMouseClicks(false, false);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightRed);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
        buttonDelete->addListener(this);
    }

    void setContext(int row, const juce::String& title, const juce::String& t, int id, int pageno)
    {
        rowNumber = row;
        labelTitle->setText(title.removeCharacters(juce::String(L"\n")), juce::dontSendNotification);
        labelCreateTime->setText(t, juce::dontSendNotification);
        signID = id;
        signPageno = pageno;
    }

    ~BookSignItem()
    {

    }

    int getSignPageno() const
    {
        return signPageno;
    }

protected:
    virtual void paint(juce::Graphics& g) override
    {
    }

    virtual void resized() override
    {
        labelTitle->setBounds(juce::Rectangle<int>(5, 0, getWidth()-160, getHeight()));
        labelCreateTime->setBounds(juce::Rectangle<int>(getRight()-165, 0, 125, getHeight()));
        buttonDelete->setBounds(juce::Rectangle<int>(getRight()-45, 8, 45, 20));
    }

    virtual void buttonClicked (juce::Button* b) override
    {
        HWParaSignDel delBookSigl(HWDocRead::getInstance()->GetBook(), signID);
        HWDocBookSign::getInstance()->SubmitRequest(this, HWReq_Sign_Del, &delBookSigl, 0);
    }

    virtual void mouseDown (const juce::MouseEvent& event) override
    {
    }

    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override
    {

    }

private:
    ReadInfoSearch& owner;
    int rowNumber, signID, signPageno;
    juce::ScopedPointer<juce::Label> labelTitle;
    juce::ScopedPointer<juce::Label> labelCreateTime;
    juce::ScopedPointer<juce::TextButton> buttonDelete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookSignItem)
};

//////////////////////////////////////////////////////////////////////////
// ReadInfoSearch

ReadInfoSearch::ReadInfoSearch()
    : hasNeedInitData(false)
{
    addAndMakeVisible(comboArea = new juce::ComboBox);
    addAndMakeVisible(editorSearch = new juce::TextEditor);
    addAndMakeVisible(buttonSearch = new juce::ImageButton);
    addAndMakeVisible(contentPanel = new juce::PacketListBox(this));

    editorSearch->setTextToShowWhenEmpty(L"全文搜索", juce::Colours::white.contrasting(0.1f));
    editorSearch->setFont(juce::Font(18));
    editorSearch->setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::white);
    editorSearch->setColour(juce::TextEditor::ColourIds::outlineColourId, ProjectInfo::colorLightGrey);
    editorSearch->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, ProjectInfo::colorLightBlue);
    editorSearch->setColour(juce::TextEditor::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    editorSearch->setColour(juce::TextEditor::ColourIds::shadowColourId, juce::Colours::transparentWhite);

    juce::Image btnImg = CXIMAGE_CACHE_LOAD(L"main_search_icon.png");
    buttonSearch->setImages(true, true, false,
        btnImg, 1.0f, juce::Colours::transparentWhite,
        btnImg, 1.0f, juce::Colours::transparentBlack,
        btnImg, 1.0f, juce::Colours::transparentBlack);
    buttonSearch->addListener(this);
}

ReadInfoSearch::~ReadInfoSearch()
{

}

void ReadInfoSearch::resized()
{
    comboArea->setBounds(15, 15, 80, 30);
    editorSearch->setBounds(95, 15, getWidth() - 140, 30);
    buttonSearch->setBounds(getWidth()-41, 23, 12, 12);
    contentPanel->setBounds(15, 55, getWidth() - 30, getHeight() - 70);
}

void ReadInfoSearch::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoSearch::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    initComboboxInfo();
}

void ReadInfoSearch::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Search_Text:
        {
            contentPanel->removeAllPanel();

            if (HWTTextSearchData* data = HWDocBookData::getInstance()->GetTextSearchData())
            {
                contentPanel->addPanel(L"包含所有关键字的结果");

                for (unsigned int i=0; i<data->vecHiPage.size(); ++i)
                {
                    juce::String str;
                    str << L"包含“" << data->vecHiPage[i].first << L"”的结果";
                    contentPanel->addPanel(str);
                }
            }
            break;
        }
    default:
        break;
    }
}

void ReadInfoSearch::initComboboxInfo()
{
    vecComboItemAreas.clear();

    int ID = 0x10000;
    comboArea->addItem(L"全文", ++ID);
    vecComboItemAreas.push_back(-1);

    const int nCatalogCount = HWDocRead::getInstance()->GetCatalogCount();
    for (int i=0; i<nCatalogCount; ++i)
    {
        int nEnd, nBegin, nDeep = -1;
        const HWTCatalog* pCatalogB = HWDocRead::getInstance()->GetCatalog(i, FALSE, nDeep);

        nBegin = pCatalogB->nPageNo;

        if (i + 1 != nCatalogCount)
        {
            const HWTCatalog* pCatalogE = HWDocRead::getInstance()->GetCatalog(i+1, FALSE, nDeep);
            nEnd = pCatalogE->nPageNo;
        }
        else
        {
            nEnd = HWDocRead::getInstance()->GetPDFPageCount();
        }

        vecComboItemAreas.push_back(MAKELONG(nBegin, nEnd));
        juce::String str;
        str << L"第" << i+1 << L"章";
        comboArea->addItem(str, ++ID);
    }

    comboArea->setSelectedItemIndex(0);
}

void ReadInfoSearch::search()
{
    juce::String text = editorSearch->getText();
    if (text.isNotEmpty())
    {
        HWParaTextSearch searchUI(text, HWDocRead::getInstance()->GetBook(), vecComboItemAreas[comboArea->getSelectedItemIndex()]);
        HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Search_Text, &searchUI, 0);
    }
}

void ReadInfoSearch::textEditorReturnKeyPressed(juce::TextEditor&)
{
    search();
}

int ReadInfoSearch::getNumRows()
{
    int row = contentPanel->getSelected();

    int num = 0;
    if (HWTTextSearchData* data = HWDocBookData::getInstance()->GetTextSearchData())
    {
        if (row == 0)
        {
            for (unsigned int i=0; i<data->vecHiPage.size(); ++i)
                num += data->vecHiPage[i].second.size();

            return num;
        }

        --row;

        if (row >= 0 && row < data->vecHiPage.size())
            num = data->vecHiPage[row].second.size();
    }

    return num;
}

void ReadInfoSearch::paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    int row = contentPanel->getSelected();

    g.setColour(ProjectInfo::colorLightGrey.withAlpha(0.5f));
    g.fillRect(0, 1, width, height-2);

    if (HWTTextSearchData* data = HWDocBookData::getInstance()->GetTextSearchData())
    {
        if (row == 0 && rowNumber >=0)
        {
            std::vector<int>* vec = nullptr;
            const juce::String& word = data->vecHiPage[row].first;
            for (unsigned int i=0; i<data->vecHiPage.size(); ++i)
            {
                const int s = data->vecHiPage[i].second.size();
                if (s > rowNumber)
                {
                    vec = &data->vecHiPage[i].second;
                    break;
                }
                rowNumber -= s;
            }

            if (!vec || rowNumber<0 ||rowNumber >= vec->size())
                return;

            const int pageno = (*vec)[rowNumber];

            juce::String strPage;
            juce::String str;
            str << L"第" << strPage << L"页";
            strPage = HWDocRead::getInstance()->GetShowPageNo(pageno, strPage) ? str : L"非正文页";

            g.setFont(juce::Font(18));
            g.setColour(ProjectInfo::colorLightGrey);
            g.drawFittedText(strPage, 10, 8, 100, 20, juce::Justification::topLeft, 1);

            std::map<int, std::pair<std::map<juce::String,juce::String>,std::vector<int>>>::iterator itInfo = data->mapPageInfo.find(pageno);
            
            if (itInfo != data->mapPageInfo.end())
            {
                std::map<juce::String,juce::String>::iterator itWord = itInfo->second.first.find(word);
                if (itWord != itInfo->second.first.end())
                {
                    juce::String text = itWord->second.removeCharacters(juce::String(L"\n"));
                    juce::AttributedString att;

                    int nStart = 0;
                    int nPos = -1;
                    while ( -1 != (nPos = text.indexOf(nStart, word)))
                    {
                        att.append(text.substring(nStart, nPos-nStart), juce::Font(18), juce::Colour(0xFF6B5147));
                        att.append(word, juce::Font(18), juce::Colour(0xFFF56863));
                        nStart = nPos + word.length();
                    }
                    att.append(text.substring(text.length() - nStart + word.length()), juce::Font(18), juce::Colour(0xFF6B5147));
                    att.draw(g, juce::Rectangle<float>(10.0f, 25.0f, (width*1.0f)-20.0f, 25.0f));
                }
            }

            return;
        }

        --row;

        if (row >= 0 && row < data->vecHiPage.size() &&
            rowNumber >=0 && rowNumber < data->vecHiPage[row].second.size())
        {
            const juce::String& word = data->vecHiPage[row].first;
            const int pageno = data->vecHiPage[row].second[rowNumber];

            juce::String strPage;
            juce::String str;
            str << L"第" << strPage << L"页";
            strPage = HWDocRead::getInstance()->GetShowPageNo(pageno, strPage) ? str : L"非正文页";

            g.setFont(juce::Font(18));
            g.setColour(ProjectInfo::colorLightGrey);
            g.drawFittedText(strPage, 10, 8, 100, 20, juce::Justification::topLeft, 1);

            std::map<int, std::pair<std::map<juce::String,juce::String>,std::vector<int>>>::iterator itInfo = data->mapPageInfo.find(pageno);
            if (itInfo != data->mapPageInfo.end())
            {
                std::map<juce::String,juce::String>::iterator itWord = itInfo->second.first.find(word);
                if (itWord != itInfo->second.first.end())
                {
                    juce::String text = itWord->second.removeCharacters(juce::String(L"\n"));
                    juce::AttributedString att;

                    int nStart = 0;
                    int nPos = -1;
                    while ( -1 != (nPos = text.indexOf(nStart, word)))
                    {
                        att.append(text.substring(nStart, nPos-nStart), juce::Font(18), juce::Colour(0xFF6B5147));
                        att.append(word, juce::Font(18), juce::Colour(0xFFF56863));
                        nStart = nPos + word.length();
                    }
                    att.append(text.substring(text.length() - nStart + word.length()), juce::Font(18), juce::Colour(0xFF6B5147));
                    att.draw(g, juce::Rectangle<float>(10.0f, 25.0f, (width*1.0f)-20.0f, 25.0f));
                }
            }
        }
    }
}

void ReadInfoSearch::expandSelectChanged()
{

}

void ReadInfoSearch::buttonClicked(juce::Button* b)
{
    if (b == buttonSearch)
    {
        search();
    }
}
