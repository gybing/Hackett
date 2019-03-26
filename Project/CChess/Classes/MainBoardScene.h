#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "Chess.h"

class MainBoardScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void InitBoard();

    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(MainBoardScene);
};

#endif // __HELLOWORLD_SCENE_H__
