#include "WorldScene.h"

USING_NS_CC;

//���������ʶ
int ball1Mask = 1 << 0;//��1
int ball2Mask = 1 << 1;//��2

Scene* WorldScene::createScene()
{
    //����������ռ�ĳ���
    Scene* scene=Scene::createWithPhysics();
    //����Debugģʽ����ῴ������ı��汻������Χ����ҪΪ���ڵ����и����׵ع۲�
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    WorldScene* layer=WorldScene::create();
    //�ѿռ䱣�����Ǵ����Ĳ��У�����������˵m_world�����ã�����������ÿռ�Ĳ���
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

    //����һ�����ӣ�������ײ
    m_pWorldLayer = WorldLayer::create();
    m_pWorldLayer->setTag(0);
    this->addChild(m_pWorldLayer);

    // ����ҡ��
    auto joystick = Joystick::create("rocker.png","rocker_joy.png");//ͨ�^�ɏ��DƬ��ʼ���ؼ�
    this->addChild(joystick);
    joystick->setPosition(Vec2(200,200));//�O�ó�ʼλ��
    joystick->setDieRadius(64);//�O�������돽����Ȧ��
    joystick->setAutoPosition(true);//�Ƿ����ɸ�׃����
    joystick->setVisibleJoystick(false);//�Ƿ��@ʾ
    joystick->onDirection=CC_CALLBACK_1(WorldScene::onDirection,this);//�Ƕ�׃�����£�onDirection(JoystickEnum enums)��
    joystick->onRun();//����

    // �Լ�������
    
    m_pBall = LayerColor::create(Color4B(random<int>(30, 200), random<int>(30, 200), random<int>(30, 200), 255));
    m_pBall->setContentSize(Size(50, 50));
    m_pBall->setPosition(visibleSize.width/2,visibleSize.height/2);
    //�������壬�����������״ΪԲ�Σ���һ����Ϊ�뾶���ڶ�������Ϊ�������
    //����������Ϊ�ߵĺ��,������Debugģʽ�¿������������������ĺ�ȣ�Ĭ��Ϊ0
    PhysicsBody* bodyOne = PhysicsBody::createCircle(m_pBall->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, m_pBall->getContentSize()/2);
    //bodyOne->setCategoryBitmask(1);
    //�Ƿ���������Ϊ��̬
    //ballBody->setDynamic(false);
    //��������Ļָ���
    bodyOne->getShape(0)->setRestitution(1.0f);
    //���������Ħ����
    bodyOne->getShape(0)->setFriction(0.0f);
    //���������ܶ�
    bodyOne->getShape(0)->setDensity(1.0f);
    //��������
    //bodyOne->getShape(0)->setMass(5000);
    //���������Ƿ�������ϵ��Ӱ��
    bodyOne->setGravityEnable(false);
    //��������ĳ���
    // Vect force=Vect(500000.0f, 500000.0f);
    // bodyOne->applyImpulse(force);
    //��������ӵ�������
    m_pBall->setPhysicsBody(bodyOne);
    //���ñ�־
    m_pBall->setTag(1);
    m_pWorldLayer->addChild(m_pBall);
    bodyOne->setCategoryBitmask(ball1Mask);
    bodyOne->setCollisionBitmask(ball1Mask|ball2Mask);
    bodyOne->setContactTestBitmask(ball1Mask|ball2Mask);

    LayerColor* ball;
    for (unsigned int i=0; i<1000; ++i)
    {
        //���õڶ�����
        ball=LayerColor::create(Color4B(random<int>(30, 200), random<int>(30, 200), random<int>(30, 200), 255));
        ball->setContentSize(Size(20, 20));
        ball->setPosition(random<int>(visibleSize.width/2, 10000-(visibleSize.width/2)), random<int>(visibleSize.height/2, 10000-(visibleSize.height/2)));
        PhysicsBody* bodyBall = PhysicsBody::createCircle(ball->getContentSize().width/2,PHYSICSBODY_MATERIAL_DEFAULT, Vec2(10,10));

        //�Ƿ���������Ϊ��̬
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
    //��Ӽ�����
    auto contactListener=EventListenerPhysicsContact::create();
    //���ü���������ײ��ʼ����
    contactListener->onContactBegin = CC_CALLBACK_1(WorldScene::onContactBegin, this);
    //��ӵ��¼��ַ�����
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
