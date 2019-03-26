#include "WorldLayer.h"

USING_NS_CC;

bool WorldLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size wordSize(10000,10000);
    setContentSize(wordSize);
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage("texture.png");
    Texture2D::TexParams tp = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
    texture->setTexParameters(tp);
    auto sprite = Sprite::createWithTexture(texture, Rect(0 , 0, wordSize.width, wordSize.height));
    sprite->setPosition(Vec2(wordSize.width/2, wordSize.height/2));
    this->addChild(sprite, 0);

    return true;
}
