#include "WorldScene.h"

USING_NS_CC;

//物理引擎标识
int ball1Mask = 1 << 0;//球1
int ball2Mask = 1 << 1;//球2

Scene* WorldScene::createScene()
{
    //创建有物理空间的场景
    Scene* scene=Scene::createWithPhysics();
    //设置Debug模式，你会看到物体的表面被线条包围，主要为了在调试中更容易地观察
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    WorldScene* layer=WorldScene::create();
    //把空间保持我们创建的层中，就是上面所说m_world的作用，方便后面设置空间的参数
    layer->setPhysicsBody(scene->getPhysicsBody());
    scene->addChild(layer);
    return scene;
}

bool WorldScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    m_maxSpeed = 8.0f;
    m_Direction = Vec2::ZERO;


    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //创建一个盒子，用来碰撞
    m_pWorldLayer = WorldLayer::create();
    m_pWorldLayer->setTag(0);
    this->addChild(m_pWorldLayer);

    // 创建摇杆
    auto joystick = Joystick::create("rocker.png","rocker_joy.png");//通過兩張圖片初始化控件
    this->addChild(joystick);
    joystick->setPosition(Vec2(200,200));//設置初始位置
    joystick->setDieRadius(64);//設置死亡半徑（外圈）
    joystick->setAutoPosition(true);//是否自由改變座標
    joystick->setVisibleJoystick(false);//是否顯示
    joystick->onDirection=CC_CALLBACK_1(WorldScene::onDirection,this);//角度變化更新（onDirection(JoystickEnum enums)）
    joystick->onRun();//啟動

    // 自己的球球
    
    m_pBall = LayerColor::create(Color4B(random<int>(30, 200), random<int>(30, 200), random<int>(30, 200), 255));
    m_pBall->setContentSize(Size(50, 50));
    m_pBall->setPosition(visibleSize.width/2,visibleSize.height/2);
    //创建物体，并且物体的形状为圆形，第一参数为半径，第二个参数为物体材质
    //第三个参数为边的厚度,就是在Debug模式下看到的物体外面线条的厚度，默认为0
    PhysicsBody* bodyOne = PhysicsBody::createCircle(m_pBall->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, m_pBall->getContentSize()/2);
    //bodyOne->setCategoryBitmask(1);
    //是否设置物体为静态
    //ballBody->setDynamic(false);
    //设置物体的恢复力
    bodyOne->getShape(0)->setRestitution(1.0f);
    //设置物体的摩擦力
    bodyOne->getShape(0)->setFriction(0.0f);
    //设置物体密度
    bodyOne->getShape(0)->setDensity(1.0f);
    //设置质量
    //bodyOne->getShape(0)->setMass(5000);
    //设置物体是否受重力系数影响
    bodyOne->setGravityEnable(false);
    //设置物体的冲力
    // Vect force=Vect(500000.0f, 500000.0f);
    // bodyOne->applyImpulse(force);
    //把物体添加到精灵中
    m_pBall->setPhysicsBody(bodyOne);
    //设置标志
    m_pBall->setTag(1);
    m_pWorldLayer->addChild(m_pBall);
    bodyOne->setCategoryBitmask(ball1Mask);
    bodyOne->setCollisionBitmask(ball1Mask|ball2Mask);
    bodyOne->setContactTestBitmask(ball1Mask|ball2Mask);

    LayerColor* ball;
    for (unsigned int i=0; i<1000; ++i)
    {
        //设置第二个球
        ball=LayerColor::create(Color4B(random<int>(30, 200), random<int>(30, 200), random<int>(30, 200), 255));
        ball->setContentSize(Size(20, 20));
        ball->setPosition(random<int>(visibleSize.width/2, 10000-(visibleSize.width/2)), random<int>(visibleSize.height/2, 10000-(visibleSize.height/2)));
        PhysicsBody* bodyBall = PhysicsBody::createCircle(ball->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, Vec2(10,10));

        //是否设置物体为静态
        bodyBall->setDynamic(true);
        bodyBall->getShape(0)->setRestitution(1.0f);
        bodyBall->getShape(0)->setFriction(0.0f);
        bodyBall->getShape(0)->setDensity(1.0f);
        bodyBall->setGravityEnable(false);
        ball->setPhysicsBody(bodyBall);
        ball->setTag(i+1);
        m_pWorldLayer->addChild(ball);
        bodyBall->setCategoryBitmask(ball2Mask);
        bodyBall->setCollisionBitmask(ball1Mask|ball2Mask);
        bodyBall->setContactTestBitmask(ball1Mask|ball2Mask);
    }

    schedule(schedule_selector(WorldScene::Update));
    return true;
}

void WorldScene::onDirection( Vec2 direction )
{
    if (abs(direction.x) > m_maxSpeed)
        direction.x = direction.x > 0 ? m_maxSpeed : -m_maxSpeed;
    if (abs(direction.y) > m_maxSpeed)
        direction.y = direction.y > 0 ? m_maxSpeed : -m_maxSpeed;
    m_Direction = direction;
}

void WorldScene::Update( float dt )
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 oldpos = m_pWorldLayer->getPosition();
    Vec2 pos = oldpos - m_Direction;
    if (pos.x > 0 || pos.x < -10000 + visibleSize.width)
    {
        pos.x = oldpos.x;
    }
    if (pos.y > 0 || pos.y < -10000 + visibleSize.height)
    {
        pos.y = oldpos.y;
    }
    // CCLOG("x:%.2f y:%.2f", oldpos.x, oldpos.y);
    Node* pNode = m_pWorldLayer->getChildByTag(1);
    pNode->setPosition(pNode->getPosition() - (pos - oldpos));
    m_pWorldLayer->setPosition(pos);
}

void WorldScene::onEnter()
{
    Layer::onEnter();
    //添加监听器
    auto contactListener=EventListenerPhysicsContact::create();
    //设置监听器的碰撞开始函数
    contactListener->onContactBegin = CC_CALLBACK_1(WorldScene::onContactBegin, this);
    //添加到事件分发器中
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

bool WorldScene::onContactBegin( const cocos2d::PhysicsContact& contact )
{
    Node* node = NULL;
    if ((contact.getShapeA()->getBody()->getCategoryBitmask() & ball1Mask) == ball1Mask &&
        (contact.getShapeB()->getBody()->getCategoryBitmask() & ball2Mask) == ball2Mask)
    {
        node = contact.getShapeB()->getBody()->getNode();
    }

    if ((contact.getShapeB()->getBody()->getCategoryBitmask() & ball1Mask) == ball1Mask &&
        (contact.getShapeA()->getBody()->getCategoryBitmask() & ball2Mask) == ball2Mask)
    {
        node = contact.getShapeA()->getBody()->getNode();
    }

    if (node)
    {
        Size szBall = m_pBall->getContentSize();
        Size szNode = node->getContentSize();
        float fBall = szBall.width*szBall.height;
        float fNode = szNode.width*szNode.height;
        fBall = fBall + fNode;
        fBall = sqrt(fBall);

        Size size(fBall, fBall);
        PhysicsBody* bodyOne = PhysicsBody::createCircle(size.width/2,PHYSICSBODY_MATERIAL_DEFAULT, size/2);
        bodyOne->getShape(0)->setRestitution(1.0f);
        bodyOne->getShape(0)->setFriction(0.0f);
        bodyOne->getShape(0)->setDensity(1.0f);
        bodyOne->setGravityEnable(false);
        bodyOne->setCategoryBitmask(ball1Mask);
        bodyOne->setCollisionBitmask(ball1Mask|ball2Mask);
        bodyOne->setContactTestBitmask(ball1Mask|ball2Mask);
        
        m_pBall->setContentSize(size);
        m_pBall->setPhysicsBody(bodyOne);
        m_pWorldLayer->removeChild(node);

        if (m_maxSpeed > 1.0f)
        {
            static float frate = 0.1f;
            m_maxSpeed -= frate;
            if (frate > 0.02f)
            {
                frate -= 0.01f;
            }
        }
    }
    return true;
}
