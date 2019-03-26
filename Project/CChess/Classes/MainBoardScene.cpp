#include "MainBoardScene.h"
#include "BoardLayer.h"

USING_NS_CC;

Scene* MainBoardScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainBoardScene::create();
    scene->addChild(layer);
    return scene;
}

bool MainBoardScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(MainBoardScene::menuCloseCallback, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
        origin.y + closeItem->getContentSize().height/2));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);


    auto board = BoardLayer::create();
    board->setContentSize(Size(1080, 1215));
    board->setPosition(Vec2::ZERO);
    this->addChild(board);

    return true;
}


void MainBoardScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
