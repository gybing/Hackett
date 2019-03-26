#ifndef __WorldScene_SCENE_H__
#define __WorldScene_SCENE_H__

#include "cocos2d.h"
#include "WorldLayer.h"
#include "Joystick.h"

class WorldScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(WorldScene);

protected:
    void onEnter();
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
    void Update(float dt);
    void onDirection( cocos2d::Vec2 direction );
    cocos2d::Vec2 m_Direction;
    WorldLayer* m_pWorldLayer;
    LayerColor* m_pBall;
    float m_maxSpeed;
};

#endif // __WorldScene_SCENE_H__
