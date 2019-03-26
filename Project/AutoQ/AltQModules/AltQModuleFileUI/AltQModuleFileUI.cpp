#include "AltQModuleFileUI.h"
#include "native/AltQNativeHelper.h"

AltQModuleFileUI::Ptr g_module;
bool CreateAltQModuleUI(IAltQModuleUIPtr* module)
{
    if (g_module)
    {
        *module = g_module;
        return true;
    }

    g_module = std::make_shared<AltQModuleFileUI>();
    *module = g_module;
    return true;
}

Image updateIcon(const File& file)
{
    const int hashCode = (file.getFullPathName() + "_iconCacheSalt").hashCode();
    Image im(ImageCache::getFromHashCode(hashCode));

    if (im.isNull())
    {
        im = juce_createIconForFile(file);

        if (im.isValid())
            ImageCache::addImageToCache(im, hashCode);
    }

    return im;
}

AltQModuleFileUI::AltQModuleFileUI()
{
}

AltQModuleFileUI::~AltQModuleFileUI()
{
}

bool AltQModuleFileUI::Initialize(const IAltQModulePtr& module)
{
    modulePtr = module;
    return true;
}

void AltQModuleFileUI::Finalize()
{

}

AltQWindow* AltQModuleFileUI::RefreshResultItemComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate)
{
    AltQFileResultItem* window = static_cast<AltQFileResultItem*>(existingWindowToUpdate);
    if (!window)
    {
        if (existingWindowToUpdate)
            deleteAndZero(existingWindowToUpdate);

        window = new AltQFileResultItem;
    }

    if (!window->setContent(result))
        deleteAndZero(window);

    return window;
}

bool AltQModuleFileUI::ShowPopupMenu(const IAltQModuleResultPtr& result)
{
    juce::PopupMenu m;
    m.addItem(1, L"打开");
    m.addItem(2, L"打开文件位置");

    switch (m.show())
    {
    case 1:
        OnResultItemExcute(result);
        break;
    case 2:
    {
        openAndSelectDirectoryFile(File(result->GetIcon()));
    }
    break;
    default:
        return false;
    }
    return true;
}

bool AltQModuleFileUI::OnResultItemExcute(const IAltQModuleResultPtr& result)
{
    bool ret = openDirectoryFile(File(result->GetIcon()));
    modulePtr->OnResultSelected(result->GetDocID());
    return ret;
}

bool AltQModuleFileUI::openAndSelectDirectoryFile(const File& file)
{
    if (file.existsAsFile())
    {
        String par("/n,/select,");
        par << file.getFullPathName();
        Process::openDocument("explorer.exe", par);
        return true;
    }
    return false;
}

bool AltQModuleFileUI::openDirectoryFile(const File& file)
{
    if (file.existsAsFile())
    {
        Process::openDocument(file.getFullPathName(), "");
        return true;
    }
    return false;
}

AltQWindow* AltQModuleFileUI::RefreshResultDetailComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate)
{
    AltQFileDetailWindow* window = static_cast<AltQFileDetailWindow*>(existingWindowToUpdate);
    if (!window)
    {
        if (existingWindowToUpdate)
            deleteAndZero(existingWindowToUpdate);

        window = new AltQFileDetailWindow;
    }

    if (!window->setContent(result))
        deleteAndZero(window);

    return window;
}

//////////////////////////////////////////////////////////////////////////
AltQFileResultItem::AltQFileResultItem()
{

}

AltQFileResultItem::~AltQFileResultItem()
{

}

bool AltQFileResultItem::setContent(const IAltQModuleResultPtr& result)
{
    resultTitle = CharPointer_UTF8(result->GetTitle().c_str());
    resultDescription = CharPointer_UTF8(result->GetDescription().c_str());
    resultIconImage = updateIcon(File(CharPointer_UTF8(result->GetIcon().c_str())));
    repaint();
    return true;
}

void AltQFileResultItem::paint(Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int iconsize = 20;

    AttributedString s;
    s.setWordWrap(AttributedString::none);
    s.setJustification(Justification::centredLeft);
    s.append(resultTitle, Colour(58, 58, 58));
    s.append(L"    — ", Colour(58, 58, 58));
    s.append(resultDescription, Colour(58, 58, 58).withAlpha(0.5f));

    const int x = iconsize + (margin * 2);
    s.draw(g, Rectangle<float>(x, 0, width - x, height));

    if (!resultIconImage.isNull())
    {
        g.drawImage(resultIconImage, margin, 4, iconsize, iconsize, 0, 0, resultIconImage.getWidth(), resultIconImage.getHeight());
    }
}

//////////////////////////////////////////////////////////////////////////
AltQFileDetailWindow::AltQFileDetailWindow()
{
}

AltQFileDetailWindow::~AltQFileDetailWindow()
{

}

bool AltQFileDetailWindow::setContent(const IAltQModuleResultPtr& result)
{
    resultTitle = CharPointer_UTF8(result->GetTitle().c_str());
    resultDescription = CharPointer_UTF8(result->GetDescription().c_str());
    targetFile = File(CharPointer_UTF8(result->GetIcon().c_str()));
    resultIconImage = updateIcon(targetFile);
    repaint();
    return true;
}

void AltQFileDetailWindow::paint(Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    int margin = 260;
    int iconsize = 105;

    if (!resultIconImage.isNull())
    {
        g.drawImage(resultIconImage, (width-iconsize)/2, 52, iconsize, iconsize, 0, 0, resultIconImage.getWidth(), resultIconImage.getHeight());
    }

    g.setColour(Colour(58, 58, 58).withAlpha(0.8f));

    String text;
    text << L"版本: " << targetFile.getVersion();
    g.drawText(text, 0, 210, width, 15, Justification::centred, true);

    int top = 280, padding = 16;
    g.drawText(L"类型 ", 0, top, width / 2, 15, Justification::centredRight, true);
    text.clear();
    text << L" " << targetFile.getFileExtension().removeCharacters(".");
    g.drawText(text, width / 2, top, width / 2, 15, Justification::centredLeft, true);

    g.drawText(L"大小 ", 0, top + (1 * padding), width / 2, 15, Justification::centredRight, true);
    text.clear();
    text << L" " << targetFile.getSize()/1024 << L" MB";
    g.drawText(text, width / 2, top + (1 * padding), width / 2, 15, Justification::centredLeft, true);

    g.drawText(L"创建时间 ", 0, top + (2 * padding), width / 2, 15, Justification::centredRight, true);
    text.clear();
    text << L" " << targetFile.getCreationTime().formatted("%Y-%m-%d %H:%M");
    g.drawText(text, width / 2, top + (2 * padding), width / 2, 15, Justification::centredLeft, true);

    g.drawText(L"修改时间 ", 0, top + (3 * padding), width / 2, 15, Justification::centredRight, true);
    text.clear();
    text << L" " << targetFile.getLastModificationTime().formatted("%Y-%m-%d %H:%M");
    g.drawText(text, width / 2, top + (3 * padding), width / 2, 15, Justification::centredLeft, true);

    g.drawText(L"上次打开时间 ", 0, top + (4 * padding), width / 2, 15, Justification::centredRight, true);
    text.clear();
    text << L" " << targetFile.getLastAccessTime().formatted("%Y-%m-%d %H:%M");
    g.drawText(text, width / 2, top + (4 * padding), width / 2, 15, Justification::centredLeft, true);

    g.setColour(Colour(212, 218, 222));
    g.drawLine(20, margin, width - 20, margin);

    g.setColour(Colour(58, 58, 58));
    g.setFont(Font(23, Font::bold));
    g.drawText(resultTitle, 0, 185, width, 25, Justification::centred, false);
}
