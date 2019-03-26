#ifndef __HelloWorld_SCENE_H__
#define __HelloWorld_SCENE_H__

#include "cocos2d.h"
#include "physics/CCPhysicsContact.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    CREATE_FUNC(HelloWorld);

protected:
    void onEnter();
    bool onContactBegin(const cocos2d::PhysicsContact& contact);

};

#endif // __HelloWorld_SCENE_H__
