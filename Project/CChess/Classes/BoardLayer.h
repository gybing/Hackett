#ifndef __BOARD_LAYER_HH__
#define __BOARD_LAYER_HH__

#include "Piece.h"

class BoardLayer : public cocos2d::Layer
{
public:
    BoardLayer();
    ~BoardLayer();

    void AddPiece( int sq, int pc );
    void DeletePiece( int sq );

    cocos2d::Vec2 GetPointByPos( int sq );
    int GetPosByPoint( Point pt );

    virtual bool init();
    CREATE_FUNC(BoardLayer);
public:
    // ≥ı ºªØ∆Â≈Ã
    void InitDefaultPosition(); 
    
    bool onTouchBegin(Touch* touch, Event* event);
    void onTouchMove(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);

private:
    Piece* m_pcSquares[256];
};

#endif // __BOARD_LAYER_HH__
