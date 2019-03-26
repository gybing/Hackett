#include "Piece.h"
#include "BoardLayer.h"

USING_NS_CC;

void Piece::initOption()
{
    
}

Piece* Piece::create( int pc )
{
    Piece *sprite = new(std::nothrow) Piece();
    if (sprite && sprite->initWithFile(GetPieceImagePath(pc)))
    {
        sprite->setTag(pc);
        sprite->autorelease();
        sprite->initOption();
        sprite->addEvents();
    }
    else
    {
        CC_SAFE_DELETE(sprite);
    }
    return sprite;
}

void Piece::addEvents()
{
//     auto listener = cocos2d::EventListenerTouchOneByOne::create();
//     listener->setSwallowTouches(true);
//     listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
//     {   
//         if(this->getBoundingBox().containsPoint(touch->getLocation()))
//         {
//             return true; // 表示处理
//         }
// 
//         return false;
//     };
// 
//     listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
//     {
//         Piece::touchEvent(touch, touch->getLocation());
//     };
// 
//     cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void Piece::touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 pt)
{

}

const std::string Piece::GetPieceImagePath( int sq )
{
    String str;
    str.initWithFormat("PIECE_%d.png", sq);
    return str.getCString();
}

void Piece::SetSelect( bool b )
{
    if (m_bSelect = b)
    {
        setScale(1.1f);
    }
    else
    {
        setScale(1.0f);
    }
}
