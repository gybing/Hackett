#include "JuceHeader.h"
#include "LXMainContentComponent.h"
#include "../Apps/LXAppTypeBase.h"

//==============================================================================
LXMainContentComponent::LXMainContentComponent()
{
    addAndMakeVisible(appGridView = new LXGridView(String::empty, this));
    appGridView->setItemSelectedOnMouseDown(false);
    // appGridView->setInterceptsMouseClicks(false, false);

    // ���������䱳����ɫΪ����ɫ��������ɫ����Ĭ�ϵĺ�ɫ
    lasso.setColour(LassoComponent<Component*>::lassoFillColourId,
        Colours::lightskyblue.withAlpha(0.2f));

    selectItems.addChangeListener(this);
}

LXMainContentComponent::~LXMainContentComponent()
{
    removeChildComponent(&lasso);
}

void LXMainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xFFF2F2F2));
}

void LXMainContentComponent::resized()
{
    if (appGridView)
    {
        appGridView->setBounds(getLocalBounds());

        const int itemsize = ((appGridView->getVisibleContentWidth() - appGridView->getViewport()->getScrollBarThickness()) / 5);
        appGridView->setItemWidth(itemsize);
        appGridView->setItemHeight(itemsize);
    }

    if (currentApp != nullptr)
        currentApp->setBounds(getLocalBounds());
}

int LXMainContentComponent::getNumItems()
{
    return LXAppTypeBase::getAppTypeList().size();
}

#include <map>
void LXMainContentComponent::paintGridViewItem(int index, Graphics& g, int width, int height, bool rowIsSelected)
{
    static std::map<int, Colour> mapTestColors;
    if (mapTestColors.find(index) == mapTestColors.end())
        mapTestColors[index] = ProjectInfo::getRandomBrightColour();

    int margin = 20;
    g.setColour(rowIsSelected ? ProjectInfo::colorLightBlue : mapTestColors[index]);
    g.fillRoundedRectangle(Rectangle<float>((float)margin, (float)margin, (float)width - (2 * margin), (float)height - (2 * margin)), 10.0f);

    if (LXAppTypeBase* type = LXAppTypeBase::getAppTypeList()[index])
    {
        String name(type->name.trimCharactersAtStart("0123456789").trimStart());

        AttributedString a;
        a.setJustification(Justification::centred);

        a.append(name, Font(20), Colours::white);

        margin += 10;
        a.draw(g, Rectangle<float>((float)margin, (float)margin, (float)width - (2 * margin), (float)height - (2 * margin)));
    }

    // g.setFont(width*0.5f);
    // g.setColour(Colours::white);
    // g.drawFittedText(String(index), Rectangle<int>(0, 0, width, height), Justification::centred, 1, 1.0f);
}

void LXMainContentComponent::gridViewItemClicked(int index, const MouseEvent&)
{
    selectedApp(index);
}

var LXMainContentComponent::getDragSourceDescription(const SparseSet<int>& rowsToDescribe)
{
    return rowsToDescribe[0];
}

void LXMainContentComponent::clearCurrentApp()
{
    currentApp = nullptr;
}

void LXMainContentComponent::selectedApp(int index)
{
    if (LXAppTypeBase* selectedType = LXAppTypeBase::getAppTypeList()[index])
    {
        currentApp = nullptr;
        currentApp = selectedType->createComponent();
        addAndMakeVisible(currentApp);
        currentApp->setName(selectedType->name);
        resized();
        appGridView->setVisible(false);
    }
}

int LXMainContentComponent::getCurrentAppIndex() const noexcept
{
    if (currentApp == nullptr)
        return -1;

    Array<LXAppTypeBase*>& apps(LXAppTypeBase::getAppTypeList());

    for (int i = apps.size(); --i >= 0;)
        if (apps.getUnchecked(i)->name == currentApp->getName())
            return i;

    return -1;
}

void LXMainContentComponent::initDefaultState()
{
    appGridView->setVisible(true);
    clearCurrentApp();
}

void LXMainContentComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &selectItems)
    {
        for (int i = 0; i < selectItems.getNumSelected(); ++i)
        {
            Component* test = dynamic_cast<Component*> (selectItems.getSelectedItem(i));

            // ����䴿����ʾ������ס��������ػ������ִ�й���
            if (test != nullptr)
                test->setAlpha(0.5f);
        }
        // ...��������
    }
}

// ��2�����������е���Ŀ��ӵ�1�������С��ú����������ק�����ڼ䣬��ͣ���Զ��ص�
void LXMainContentComponent::findLassoItemsInArea(Array<Component*>& itemsFound,
    const juce::Rectangle<int>& area)
{
    // ������ǰ���п��ܱ�ѡ���TestComp���͵��������
    // ���������ס�����ǣ�������ӵ�1��������
    for (int i = 0; i < getNumItems(); ++i)
    {
        if (Component* c = appGridView->getComponentForIndexNumber(i))
        /* getBounds()����ֵΪRectangle���󣬸þ��ε�4������������ڸ�������������������������intersects()��Rectangle��ĳ�Ա�����������ǰ������ú����Ĳ������Σ����������������Σ����ص����֣��򷵻�true����if���������ǣ��������Ȧ���������˱�����ڵ�ĳ����ĳЩTestComp���͵������������Щ �������ָ�룩��ӵ���������1�������С��˴������ѡ����Ŀ���ڲ��Զ�����getLassoSelection()���ṩ��SelectedItemSet����  */
        if (c->getBounds().intersects(area))
            itemsFound.add(c);
    }
}
/* �������ڱ�����ѡ��Ŀ��SelectedItemSet����findLassoItemsInArea()��1�ν��Զ���������ӵ��ö����У������κ���ʽ���롣*/
SelectedItemSet<Component*>& LXMainContentComponent::getLassoSelection()
{
    return selectItems;
}

void LXMainContentComponent::mouseDown(const MouseEvent& event)
{
    addChildComponent(&lasso);   // ��������ڴ������������
    lasso.beginLasso(event, this); // ��ʼ��ק����
}

void LXMainContentComponent::mouseDrag(const MouseEvent& event)
{
    lasso.toFront(false);   // �����ö�����ӵ�н���
    lasso.dragLasso(event); // ������ק��
}

void LXMainContentComponent::mouseUp(const MouseEvent& event)
{
    lasso.endLasso();   // ����������ק
    removeChildComponent(&lasso); // ��������ڴ��Ƴ���������

    // ������ڵ������꣬����û�а��¼��̹��ܼ�����ȡ�����е�����ѡ��
    if (event.mouseWasClicked() && !event.mods.isAnyModifierKeyDown())
        selectItems.deselectAll();
}