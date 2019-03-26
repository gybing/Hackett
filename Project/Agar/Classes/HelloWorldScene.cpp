#include "HelloWorldScene.h"

USING_NS_CC;

//���������ʶ
int ball1Mask = 1 << 0;//��1
int ball2Mask = 1 << 1;//��2
int wallMask = 1 << 2;//����

Scene* HelloWorld::createScene()
{
    //����������ռ�ĳ���
    Scene* scene=Scene::createWithPhysics();
    //����Debugģʽ����ῴ������ı��汻������Χ����ҪΪ���ڵ����и����׵ع۲�
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    HelloWorld* layer=HelloWorld::create();
    //�ѿռ䱣�����Ǵ����Ĳ��У�����������˵m_world�����ã�����������ÿռ�Ĳ���
    layer->setPhysicsBody(scene->getPhysicsBody());
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance

void HelloWorld::onEnter()
{
    Layer::onEnter();
    //��Ӽ�����
    auto contactListener=EventListenerPhysicsContact::create();
    //���ü���������ײ��ʼ����
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    //��ӵ��¼��ַ�����
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
    //�������壬�����������״ΪԲ�Σ���һ����Ϊ�뾶���ڶ�������Ϊ�������
    //����������Ϊ�ߵĺ��,������Debugģʽ�¿������������������ĺ�ȣ�Ĭ��Ϊ0
    PhysicsBody* ballBodyOne=PhysicsBody::createCircle(ballOne->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, Vec2(1,1));
    //
    //ballBodyOne->setCategoryBitmask(1);
    
    //�Ƿ���������Ϊ��̬
    //ballBody->setDynamic(false);
    //��������Ļָ���
    ballBodyOne->getShape(0)->setRestitution(1.0f);
    //���������Ħ����
    ballBodyOne->getShape(0)->setFriction(0.0f);
    //���������ܶ�
    ballBodyOne->getShape(0)->setDensity(1.0f);
    //��������
    //ballBodyOne->getShape(0)->setMass(5000);
    //���������Ƿ�������ϵ��Ӱ��
    ballBodyOne->setGravityEnable(false);
    
    //��������ĳ���
    Vect force=Vect(500000.0f, 500000.0f);
    ballBodyOne->applyImpulse(force);
    //��������ӵ�������
    ballOne->setPhysicsBody(ballBodyOne);
    //���ñ�־
    ballOne->setTag(1);
    this->addChild(ballOne);
    
    
    for (unsigned int i=0; i<1000; ++i)
    {
        //���õڶ�����
        Sprite* ballTwo=Sprite::create();
        ballTwo->setContentSize(Size(100, 100));
        ballTwo->setPosition(visibleSize.width/3,visibleSize.height/3);
        PhysicsBody* ballBodyTwo=PhysicsBody::createCircle(ballOne->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT);
        //�Ƿ���������Ϊ��̬
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
    
    //����һ�����ӣ�������ײ
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
