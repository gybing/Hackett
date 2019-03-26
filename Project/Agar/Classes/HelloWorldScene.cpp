#include "HelloWorldScene.h"

USING_NS_CC;

//物理引擎标识
int ball1Mask = 1 << 0;//球1
int ball2Mask = 1 << 1;//球2
int wallMask = 1 << 2;//地面

Scene* HelloWorld::createScene()
{
    //创建有物理空间的场景
    Scene* scene=Scene::createWithPhysics();
    //设置Debug模式，你会看到物体的表面被线条包围，主要为了在调试中更容易地观察
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    HelloWorld* layer=HelloWorld::create();
    //把空间保持我们创建的层中，就是上面所说m_world的作用，方便后面设置空间的参数
    layer->setPhysicsBody(scene->getPhysicsBody());
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance

void HelloWorld::onEnter()
{
    Layer::onEnter();
    //添加监听器
    auto contactListener=EventListenerPhysicsContact::create();
    //设置监听器的碰撞开始函数
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    //添加到事件分发器中
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    
    
    Size visibleSize=Director::getInstance()->getVisibleSize();
    Point origin=Director::getInstance()->getVisibleOrigin();
    
    Sprite* ballOne=Sprite::create();
    ballOne->setContentSize(Size(50, 50));
    ballOne->setPosition(visibleSize.width/2,visibleSize.height/2);
    //创建物体，并且物体的形状为圆形，第一参数为半径，第二个参数为物体材质
    //第三个参数为边的厚度,就是在Debug模式下看到的物体外面线条的厚度，默认为0
    PhysicsBody* ballBodyOne=PhysicsBody::createCircle(ballOne->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, Vec2(1,1));
    //
    //ballBodyOne->setCategoryBitmask(1);
    
    //是否设置物体为静态
    //ballBody->setDynamic(false);
    //设置物体的恢复力
    ballBodyOne->getShape(0)->setRestitution(1.0f);
    //设置物体的摩擦力
    ballBodyOne->getShape(0)->setFriction(0.0f);
    //设置物体密度
    ballBodyOne->getShape(0)->setDensity(1.0f);
    //设置质量
    //ballBodyOne->getShape(0)->setMass(5000);
    //设置物体是否受重力系数影响
    ballBodyOne->setGravityEnable(false);
    
    //设置物体的冲力
    Vect force=Vect(500000.0f, 500000.0f);
    ballBodyOne->applyImpulse(force);
    //把物体添加到精灵中
    ballOne->setPhysicsBody(ballBodyOne);
    //设置标志
    ballOne->setTag(1);
    this->addChild(ballOne);
    
    
    for (unsigned int i=0; i<1000; ++i)
    {
        //设置第二个球
        Sprite* ballTwo=Sprite::create();
        ballTwo->setContentSize(Size(100, 100));
        ballTwo->setPosition(visibleSize.width/3,visibleSize.height/3);
        PhysicsBody* ballBodyTwo=PhysicsBody::createCircle(ballOne->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT);
        //是否设置物体为静态
        //ballBodyTwo->setDynamic(false);
        ballBodyTwo->getShape(0)->setRestitution(1.0f);
        ballBodyTwo->getShape(0)->setFriction(0.0f);
        ballBodyTwo->getShape(0)->setDensity(1.0f);

        ballBodyTwo->setGravityEnable(false);

        force=Vect(i-500000.0f, i-500000.0f);
        ballBodyTwo->applyImpulse(force);
        ballTwo->setPhysicsBody(ballBodyTwo);
        ballTwo->setTag(5+i);
        this->addChild(ballTwo);

        ballBodyTwo->setCategoryBitmask(ball2Mask);
        ballBodyTwo->setCollisionBitmask(wallMask|ball1Mask|ball2Mask);
        ballBodyTwo->setContactTestBitmask(wallMask|ball1Mask|ball2Mask);
    }
    
    //创建一个盒子，用来碰撞
    Sprite* edgeSpace=Sprite::create();
    PhysicsBody* boundBody=PhysicsBody::createEdgeBox(visibleSize,PHYSICSBODY_MATERIAL_DEFAULT,3);
    boundBody->getShape(0)->setFriction(0.0f);
    boundBody->getShape(0)->setRestitution(1.0f);
    
    edgeSpace->setPhysicsBody(boundBody);
    edgeSpace->setPosition(Point(visibleSize.width/2,visibleSize.height/2));
    this->addChild(edgeSpace);
    edgeSpace->setTag(0);
    
    
    
    
    ballBodyOne->setCategoryBitmask(ball1Mask);
    ballBodyOne->setCollisionBitmask(wallMask|ball1Mask|ball2Mask);
    ballBodyOne->setContactTestBitmask(wallMask|ball1Mask|ball2Mask);
    
//     ballBodyTwo->setCategoryBitmask(ball2Mask);
//     ballBodyTwo->setCollisionBitmask(wallMask|ball1Mask|ball2Mask);
//     ballBodyTwo->setContactTestBitmask(wallMask|ball1Mask|ball2Mask);
    
    boundBody->setCategoryBitmask(wallMask);
    boundBody->setCollisionBitmask(wallMask | ball1Mask|ball2Mask);
    boundBody->setContactTestBitmask(wallMask|ball1Mask|ball2Mask);
    
    
    return true;
}

bool HelloWorld::onContactBegin(const PhysicsContact& contact)
{
   
    if ((contact.getShapeA()->getBody()->getCategoryBitmask() & ball1Mask) == ball1Mask) {
        CCLOG("ball1 touch something");
    }
    if ((contact.getShapeB()->getBody()->getCategoryBitmask() & ball1Mask) == ball1Mask) {
        CCLOG("ball1 touch something");
    }
    
    
    if ((contact.getShapeA()->getBody()->getCategoryBitmask() & ball2Mask) == ball2Mask) {
        CCLOG("ball2 touch something");
    }
    if ((contact.getShapeB()->getBody()->getCategoryBitmask() & ball2Mask) == ball2Mask) {
        CCLOG("ball2 touch something");
    }
    
    return true;
}
void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
