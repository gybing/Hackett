#include "JuceHeader.h"
#include "LXMainContentComponent.h"
#include "../Apps/LXAppTypeBase.h"

//==============================================================================
LXMainContentComponent::LXMainContentComponent()
{
    addAndMakeVisible(appGridView = new LXGridView(String::empty, this));
    appGridView->setItemSelectedOnMouseDown(false);
    // appGridView->setInterceptsMouseClicks(false, false);

    // 套索设置其背景颜色为淡蓝色，边线颜色采用默认的黑色
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

            // 此语句纯属演示，所套住的子组件重绘自身或执行功能
            if (test != nullptr)
                test->setAlpha(0.5f);
        }
        // ...其他代码
    }
}

// 将2参套索区域中的项目添加到1参数组中。该函数在鼠标拖拽套索期间，不停的自动回调
void LXMainContentComponent::findLassoItemsInArea(Array<Component*>& itemsFound,
    const juce::Rectangle<int>& area)
{
    // 遍历当前所有可能被选择的TestComp类型的子组件，
    // 如果套索套住了它们，则将其添加到1参数组中
    for (int i = 0; i < getNumItems(); ++i)
    {
        if (Component* c = appGridView->getComponentForIndexNumber(i))
        /* getBounds()返回值为Rectangle对象，该矩形的4个坐标点均相对于父级组件，即本例的内容组件。intersects()是Rectangle类的成员函数，如果当前矩形与该函数的参数矩形（即本例的套索矩形）有重叠部分，则返回true。该if语句的作用是：如果套索圈到或碰到了本组件内的某个或某些TestComp类型的子组件，则这些 子组件（指针）添加到本函数的1参数组中。此处添加所选的项目后，内部自动更新getLassoSelection()所提供的SelectedItemSet对象  */
        if (c->getBounds().intersects(area))
            itemsFound.add(c);
    }
}
/* 给出用于保存所选项目的SelectedItemSet对象，findLassoItemsInArea()的1参将自动把数据添加到该对象中，无需任何显式编码。*/
SelectedItemSet<Component*>& LXMainContentComponent::getLassoSelection()
{
    return selectItems;
}

void LXMainContentComponent::mouseDown(const MouseEvent& event)
{
    addChildComponent(&lasso);   // 内容组件在此添加套索对象
    lasso.beginLasso(event, this); // 开始拖拽套索
}

void LXMainContentComponent::mouseDrag(const MouseEvent& event)
{
    lasso.toFront(false);   // 套索置顶，不拥有焦点
    lasso.dragLasso(event); // 套索拖拽中
}

void LXMainContentComponent::mouseUp(const MouseEvent& event)
{
    lasso.endLasso();   // 套索结束拖拽
    removeChildComponent(&lasso); // 内容组件在此移除套索对象

    // 本组件内点击了鼠标，并且没有按下键盘功能键，则取消已有的所有选择
    if (event.mouseWasClicked() && !event.mods.isAnyModifierKeyDown())
        selectItems.deselectAll();
}