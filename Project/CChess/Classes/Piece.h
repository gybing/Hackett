#ifndef __PIECE_SPRITE_HH__
#define __PIECE_SPRITE_HH__

#include "cocos2d.h"
using namespace cocos2d;

class BoardLayer;

class Piece : public cocos2d::Sprite
{
public:

    void SetSelect(bool b);

protected:
    friend class BoardLayer;
    static Piece* create( int pc );
    static const std::string GetPieceImagePath(int pc);

protected:
    void initOption();
    void addEvents();
    void touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 pt);

private:
    bool m_bSelect;
};

#endif // __PIECE_SPRITE_HH__
