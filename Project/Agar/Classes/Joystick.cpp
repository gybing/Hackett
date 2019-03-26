#include "Joystick.h"

Joystick* Joystick::create(std::string chassisPath,std::string dotPath)
{
    auto joystick = new Joystick();
    joystick->initWithJoystick(chassisPath,dotPath);
    return joystick;
}

void Joystick::initWithJoystick(std::string chassisPath,std::string dotPath)
{
    _chassis = Sprite::create(chassisPath);
    this->addChild(_chassis,0);
    _touchDot = Sprite::create(dotPath);
    this->addChild(_touchDot,1);

    isDieRadius = false;
    isAutoPosition = false;
    isTouchVisible = false;
}

void Joystick::onRun()
{
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan,this);
    listener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved,this);
    listener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded,this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
}

bool Joystick::onTouchBegan(Touch* touch,Event* event)
{
    if (isTouchVisible) setVisibleJoystick(true);

    Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
    if( isAutoPosition )
    {
        this->setPosition(touch->getLocation());
        return true;
    }
    if( isAutoPosition==false && isDieRadius )
    {
        if( locationInNode.getLength() > _radius )
        {
            if (isTouchVisible) setVisibleJoystick(true);
            return false;
        }
    }
    _touchDot->setPosition(locationInNode);
    if( locationInNode.getLength() > _failradius )
    {
        callDirectionFun(locationInNode);
    }
    return true;
}
void Joystick::onTouchMoved(Touch* touch,Event* event)
{
    Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
    if( isDieRadius)
    {
        int length = locationInNode.getLength();
        if (length > _radius)
        {
            locationInNode.x = _radius * locationInNode.x / length;
            locationInNode.y = _radius * locationInNode.y / length;
            setTouchDotPosition(locationInNode, locationInNode);
            return;
        }
    }

    setTouchDotPosition(locationInNode,_touchDot->getPosition() + touch->getDelta());
}
void Joystick::onTouchEnded(Touch* touch,Event* event)
{
    if (isTouchVisible) setVisibleJoystick(false);
    _touchDot->setPosition(0,0);
    resetState();
}

void Joystick::setTouchDotPosition(Vec2 vec1,Vec2 vec2)
{
    _touchDot->setPosition(vec2);
    if( _failradius>0 )
    {
        if( vec1.getLength() < _failradius )
        {
            resetState();
            return;
        }
    }
    callDirectionFun(vec1);
}

void Joystick::setDieRadius(float radius)
{
    _radius = radius;
    isDieRadius = true;
}

void Joystick::setAutoPosition(bool value)
{
    isAutoPosition = value;
}

void Joystick::setFailRadius(float radius)
{
    _failradius = radius;
}

void Joystick::onDisable()
{
    this->_eventDispatcher->removeEventListener(listener);
    isDieRadius = false;
    isAutoPosition = false;
}

void Joystick::callDirectionFun( Vec2 position)
{
    if (onDirection)
    {
        onDirection(position);
    }

    if( onFixedDirection)
    {
        JoystickEnum direction = DEFAULT;
        auto angle = CC_RADIANS_TO_DEGREES(position.getAngle());
        if(angle > -22.5 && angle < 22.5)
        {
            direction=D_RIGHT;
        }
        else if(angle > 22.5 && angle < 67.5)
        {
            direction=D_RIGHT_UP;
        }
        else if(angle > 67.5 && angle < 112.5)
        {
            direction=D_UP;
        }
        else if(angle > 112.5 && angle < 157.5)
        {
            direction=D_LEFT_UP;
        }
        else if((angle > 157.5 && angle < 180)||(angle < -157.5 && angle > -180))
        {
            direction=D_LEFT;
        }
        else if(angle < -112.5 && angle > -157.5)
        {
            direction=D_LEFT_DOWN;
        }
        else if(angle < -67.5 && angle > -112.5)
        {
            direction=D_DOWN;
        }
        else if(angle < -22.5 && angle > -67.5)
        {
            direction=D_RIGHT_DOWN;
        }
        onFixedDirection(direction);
    }
}

void Joystick::resetState()
{
    //callDirectionFun(Vec2::ZERO);
}

void Joystick::setVisibleJoystick(bool visible)
{
    _chassis->setVisible(visible);
    _touchDot->setVisible(visible);
    isTouchVisible = true;
}
