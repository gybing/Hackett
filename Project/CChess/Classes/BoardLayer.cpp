#include "BoardLayer.h"
#include "Chess.h"

USING_NS_CC;

BoardLayer::BoardLayer()
{
}

BoardLayer::~BoardLayer()
{
}

bool BoardLayer::init()
{
    if ( !Layer::init() )
        return false;

    // 点击监听事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BoardLayer::onTouchBegin, this);
    listener->onTouchMoved = CC_CALLBACK_2(BoardLayer::onTouchMove, this);
    listener->onTouchEnded = CC_CALLBACK_2(BoardLayer::onTouchEnded, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 棋盘背景
    auto spBackgroud = Sprite::create("board.png");
    spBackgroud->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(spBackgroud);

    Initialize(this);
    InitDefaultPosition();

    return true;
}

void BoardLayer::InitDefaultPosition()
{
    for (unsigned int i=0; i<256; ++i)
    {
    }
}

bool BoardLayer::onTouchBegin( Touch* touch, Event* event )
{
    return true;
}

void BoardLayer::onTouchMove(Touch* touch, Event* event)
{

}

void BoardLayer::onTouchEnded(Touch* touch, Event* event)
{
    Point loction = convertToNodeSpace(touch->getLocation());
    int sq = GetPosByPoint(loction);
    if (sq != -1)
    {
        ClickSquare(sq);
    }
}

cocos2d::Vec2 BoardLayer::GetPointByPos( int sq )
{
    // 横向9*2等分，纵向10*2等分
    Size size = Director::getInstance()->getVisibleSize();
    float step = size.width / 18.0f;
    int nxcnt = 1 + ((FILE_X(sq) - FILE_LEFT) * 2);
    int nycnt = 1 + (RANK_FLIP((RANK_Y(sq))) * 2);
    return Vec2(step*nxcnt, step*nycnt);
}

void BoardLayer::AddPiece( int sq, int pc )
{
    DeletePiece(sq);
    if (pc != 0)
    {
        Piece* piece = Piece::create(pc);
        piece->setPosition(GetPointByPos(sq));
        this->addChild(piece);
        m_pcSquares[sq] = piece;
    }
}

void BoardLayer::DeletePiece( int sq )
{
    if (m_pcSquares[sq])
    {
        removeChild(m_pcSquares[sq]);
        m_pcSquares[sq] = nullptr;
    }
}

int BoardLayer::GetPosByPoint( Point pt )
{
    Size size = Director::getInstance()->getVisibleSize();
    float step = size.width / 9.0f;
    int x = FILE_LEFT + pt.x / step;
    int y = RANK_FLIP(pt.y / step);
    if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM)
    {
        return COORD_XY(x, y);
    }
    return -1;
}