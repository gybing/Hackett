#ifndef  __APP_INC_CHESS_HH__
#define  __APP_INC_CHESS_HH__

#include "cocos2d.h"
using namespace cocos2d;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#include <stdlib.h>
#include <time.h>
#include <string>
#include "Convert.h"

#define IDR_CLICK      "CLICK.WAV"
#define IDR_ILLEGAL    "ILLEGAL.WAV"
#define IDR_MOVE       "MOVE.WAV"
#define IDR_MOVE2      "MOVE2.WAV"
#define IDR_CAPTURE    "CAPTURE.WAV"
#define IDR_CAPTURE2   "CAPTURE2.WAV"
#define IDR_CHECK      "CHECK.WAV"
#define IDR_CHECK2     "CHECK2.WAV"
#define IDR_WIN        "WIN.WAV"
#define IDR_DRAW       "DRAW.WAV"
#define IDR_LOSS       "LOSS.WAV"

// 棋盘范围
const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_RIGHT = 11;

// 棋子编号
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;

// 其他常数
const int MAX_GEN_MOVES = 128; // 最大的生成走法数
const int MAX_MOVES = 256;     // 最大的历史走法数
const int LIMIT_DEPTH = 64;    // 最大的搜索深度
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int BAN_VALUE = MATE_VALUE - 100; // 长将判负的分值，低于该值将不写入置换表
const int WIN_VALUE = MATE_VALUE - 200; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int DRAW_VALUE = 20;     // 和棋时返回的分数(取负值)
const int ADVANCED_VALUE = 3;  // 先行权分值
const int RANDOM_MASK = 7;     // 随机性分值
const int NULL_MARGIN = 400;   // 空步裁剪的子力边界
const int NULL_DEPTH = 2;      // 空步裁剪的裁剪深度
const int HASH_SIZE = 1 << 20; // 置换表大小
const int HASH_ALPHA = 1;      // ALPHA节点的置换表项
const int HASH_BETA = 2;       // BETA节点的置换表项
const int HASH_PV = 3;         // PV节点的置换表项
const int BOOK_SIZE = 16384;   // 开局库大小

// 判断棋子是否在棋盘中的数组
static const char ccInBoard[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断棋子是否在九宫的数组
static const char ccInFort[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断步长是否符合特定走法的数组，1=帅(将)，2=仕(士)，3=相(象)
static const char ccLegalSpan[512] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};

// 根据步长判断马是否蹩腿的数组
static const char ccKnightPin[512] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0
};

// 帅(将)的步长
static const char ccKingDelta[4] = {-16, -1, 1, 16};
// 仕(士)的步长
static const char ccAdvisorDelta[4] = {-17, -15, 15, 17};
// 马的步长，以帅(将)的步长作为马腿
static const char ccKnightDelta[4][2] = {{-33, -31}, {-18, 14}, {-14, 18}, {31, 33}};
// 马被将军的步长，以仕(士)的步长作为马腿
static const char ccKnightCheckDelta[4][2] = {{-33, -18}, {-31, -14}, {14, 31}, {18, 33}};

// 棋盘初始设置
static const BYTE cucpcStartup[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
    0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
    0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 子力位置价值表
static const BYTE cucvlPiecePos[7][256] = {
    { // 帅(将)
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // 仕(士)
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // 相(象)
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
        }, { // 马
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
                0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
                0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
                0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
                0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
                0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
                0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
                0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
                0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
                0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
        }, { // 车
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
                0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
                0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
                0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
                0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
                0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
                0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
                0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
                0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
                0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
        }, { // 炮
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
                0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
                0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
                0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
                0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
                0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
                0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
                0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
                0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
                0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
            }, { // 兵(卒)
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
                    0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
                    0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
                    0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
                    0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
                    0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
                    0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
            }
};

// 判断棋子是否在棋盘中
inline BOOL IN_BOARD(int sq)
{
    return ccInBoard[sq] != 0;
}

// 判断棋子是否在九宫中
inline BOOL IN_FORT(int sq)
{
    return ccInFort[sq] != 0;
}

// 获得格子的横坐标
inline int RANK_Y(int sq)
{
    return sq >> 4;
}

// 获得格子的纵坐标
inline int FILE_X(int sq)
{
    return sq & 15;
}

// 根据纵坐标和横坐标获得格子
inline int COORD_XY(int x, int y)
{
    return x + (y << 4);
}

// 翻转格子
inline int SQUARE_FLIP(int sq)
{
    return 254 - sq;
}

// 纵坐标水平镜像
inline int FILE_FLIP(int x)
{
    return 14 - x;
}

// 横坐标垂直镜像
inline int RANK_FLIP(int y)
{
    return 15 - y;
}

// 格子水平镜像
inline int MIRROR_SQUARE(int sq)
{
    return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// 格子水平镜像
inline int SQUARE_FORWARD(int sq, int sd)
{
    return sq - 16 + (sd << 5);
}

// 走法是否符合帅(将)的步长
inline BOOL KING_SPAN(int sqSrc, int sqDst)
{
    return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// 走法是否符合仕(士)的步长
inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst)
{
    return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// 走法是否符合相(象)的步长
inline BOOL BISHOP_SPAN(int sqSrc, int sqDst)
{
    return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// 相(象)眼的位置
inline int BISHOP_PIN(int sqSrc, int sqDst)
{
    return (sqSrc + sqDst) >> 1;
}

// 马腿的位置
inline int KNIGHT_PIN(int sqSrc, int sqDst)
{
    return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// 是否未过河
inline BOOL HOME_HALF(int sq, int sd)
{
    return (sq & 0x80) != (sd << 7);
}

// 是否已过河
inline BOOL AWAY_HALF(int sq, int sd)
{
    return (sq & 0x80) == (sd << 7);
}

// 是否在河的同一边
inline BOOL SAME_HALF(int sqSrc, int sqDst)
{
    return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// 是否在同一行
inline BOOL SAME_RANK(int sqSrc, int sqDst)
{
    return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// 是否在同一列
inline BOOL SAME_FILE(int sqSrc, int sqDst)
{
    return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// 获得红黑标记(红子是8，黑子是16)
inline int SIDE_TAG(int sd)
{
    return 8 + (sd << 3);
}

// 获得对方红黑标记
inline int OPP_SIDE_TAG(int sd)
{
    return 16 - (sd << 3);
}

// 获得走法的起点
inline int SRC(int mv)
{
    return mv & 255;
}

// 获得走法的终点
inline int DST(int mv)
{
    return mv >> 8;
}

// 根据起点和终点获得走法
inline int MOVE(int sqSrc, int sqDst)
{
    return sqSrc + sqDst * 256;
}

// 走法水平镜像
inline int MIRROR_MOVE(int mv)
{
    return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

// RC4密码流生成器
struct RC4Struct
{
    BYTE s[256];
    int x, y;

    void InitZero(void);   // 用空密钥初始化密码流生成器
    BYTE NextByte(void)
    {  // 生成密码流的下一个字节
        BYTE uc;
        x = (x + 1) & 255;
        y = (y + s[x]) & 255;
        uc = s[x];
        s[x] = s[y];
        s[y] = uc;
        return s[(s[x] + s[y]) & 255];
    }
    DWORD NextLong(void)
    { // 生成密码流的下四个字节
        BYTE uc0, uc1, uc2, uc3;
        uc0 = NextByte();
        uc1 = NextByte();
        uc2 = NextByte();
        uc3 = NextByte();
        return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
    }
};

// Zobrist结构
struct ZobristStruct
{
    DWORD dwKey, dwLock0, dwLock1;

    void InitZero(void)
    {                 // 用零填充Zobrist
        dwKey = dwLock0 = dwLock1 = 0;
    }
    void InitRC4(RC4Struct &rc4)
    {        // 用密码流填充Zobrist
        dwKey = rc4.NextLong();
        dwLock0 = rc4.NextLong();
        dwLock1 = rc4.NextLong();
    }
    void Xor(const ZobristStruct &zobr)
    { // 执行XOR操作
        dwKey ^= zobr.dwKey;
        dwLock0 ^= zobr.dwLock0;
        dwLock1 ^= zobr.dwLock1;
    }
    void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2)
    {
        dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
        dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
        dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
    }
};

// Zobrist表
static struct
{
    ZobristStruct Player;
    ZobristStruct Table[14][256];
} Zobrist;

// 初始化Zobrist表
static void InitZobrist(void)
{
    int i, j;
    RC4Struct rc4;

    rc4.InitZero();
    Zobrist.Player.InitRC4(rc4);
    for (i = 0; i < 14; i ++)
    {
        for (j = 0; j < 256; j ++)
        {
            Zobrist.Table[i][j].InitRC4(rc4);
        }
    }
}

// 历史走法信息(占4字节)
struct MoveStruct
{
    WORD wmv;
    BYTE ucpcCaptured, ucbCheck;
    DWORD dwKey;

    void Set(int mv, int pcCaptured, BOOL bCheck, DWORD dwKey_)
    {
        wmv = mv;
        ucpcCaptured = pcCaptured;
        ucbCheck = bCheck;
        dwKey = dwKey_;
    }
}; // mvs

// 局面结构
struct PositionStruct
{
    int sdPlayer;                   // 轮到谁走，0=红方，1=黑方
    BYTE ucpcSquares[256];          // 棋盘上的棋子
    int vlWhite, vlBlack;           // 红、黑双方的子力价值
    int nDistance, nMoveNum;        // 距离根节点的步数，历史走法数
    MoveStruct mvsList[MAX_MOVES];  // 历史走法信息列表
    ZobristStruct zobr;             // Zobrist

    void ClearBoard(void); // 清空棋盘
    void SetIrrev(void); // 清空(初始化)历史走法信息
    void Startup(void);             // 初始化棋盘
    void ChangeSide(void); // 交换走子方
    void AddPiece(int sq, int pc); // 在棋盘上放一枚棋子
    void DelPiece(int sq, int pc); // 从棋盘上拿走一枚棋子
    int Evaluate(void) const; // 局面评价函数
    BOOL InCheck(void) const; // 是否被将军
    BOOL Captured(void) const; // 上一步是否吃子
    int MovePiece(int mv);                      // 搬一步棋的棋子
    void UndoMovePiece(int mv, int pcCaptured); // 撤消搬一步棋的棋子
    BOOL MakeMove(int mv);                      // 走一步棋
    void UndoMakeMove(void); // 撤消走一步棋
    void NullMove(void); // 走一步空步
    void UndoNullMove(void); // 撤消走一步空步
    // 生成所有走法，如果"bCapture"为"TRUE"则只生成吃子走法
    int GenerateMoves(int *mvs, BOOL bCapture = FALSE) const;
    BOOL LegalMove(int mv) const;               // 判断走法是否合理
    BOOL Checked(void) const;                   // 判断是否被将军
    BOOL IsMate(void);                          // 判断是否被杀
    int DrawValue(void) const;  // 和棋分值
    int RepStatus(int nRecur = 1) const;        // 检测重复局面
    int RepValue(int nRepStatus) const;  // 重复局面分值
    BOOL NullOkay(void) const; // 判断是否允许空步裁剪
    void Mirror(PositionStruct &posMirror) const; // 对局面镜像
};

static PositionStruct pos; // 局面实例

// 与图形界面有关的全局变量
static struct
{
    int sqSelected, mvLast;                       // 选中的格子，上一步棋
    BOOL bFlipped, bGameOver;                     // 是否翻转棋盘，是否游戏结束(不让继续玩下去)
} Xqwl;

// 置换表项结构
struct HashItem {
    BYTE ucDepth, ucFlag;
    short svl;
    WORD wmv, wReserved;
    DWORD dwLock0, dwLock1;
};

// 开局库项结构
struct BookItem {
    DWORD dwLock;
    WORD wmv, wvl;
};

// 与搜索有关的全局变量
static struct
{
    int mvResult;                  // 电脑走的棋
    int nHistoryTable[65536];      // 历史表
    int mvKillers[LIMIT_DEPTH][2]; // 杀手走法表
    HashItem HashTable[HASH_SIZE]; // 置换表
    int nBookSize;                 // 开局库大小
    BookItem BookTable[BOOK_SIZE]; // 开局库
} Search;

// 装入开局库
static void LoadBook(void)
{
    cocos2d::Data data = FileUtils::getInstance()->getDataFromFile("BOOK.DAT");
    Search.nBookSize = data.getSize() / sizeof(BookItem);
    if (Search.nBookSize > BOOK_SIZE)
    {
        Search.nBookSize = BOOK_SIZE;
    }
    memcpy(Search.BookTable, data.getBytes(),
        Search.nBookSize * sizeof(BookItem));
}

static int CompareBook(const void *lpbk1, const void *lpbk2)
{
    DWORD dw1, dw2;
    dw1 = ((BookItem *) lpbk1)->dwLock;
    dw2 = ((BookItem *) lpbk2)->dwLock;
    return dw1 > dw2 ? 1 : dw1 < dw2 ? -1 : 0;
}

// 搜索开局库
static int SearchBook(void)
{
    int i, vl, nBookMoves, mv;
    int mvs[MAX_GEN_MOVES], vls[MAX_GEN_MOVES];
    BOOL bMirror;
    BookItem bkToSearch, *lpbk;
    PositionStruct posMirror;
    // 搜索开局库的过程有以下几个步骤

    // 1. 如果没有开局库，则立即返回
    if (Search.nBookSize == 0)
    {
        return 0;
    }
    // 2. 搜索当前局面
    bMirror = FALSE;
    bkToSearch.dwLock = pos.zobr.dwLock1;
    lpbk = (BookItem *) bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
    // 3. 如果没有找到，那么搜索当前局面的镜像局面
    if (lpbk == NULL)
    {
        bMirror = TRUE;
        pos.Mirror(posMirror);
        bkToSearch.dwLock = posMirror.zobr.dwLock1;
        lpbk = (BookItem *) bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
    }
    // 4. 如果镜像局面也没找到，则立即返回
    if (lpbk == NULL)
    {
        return 0;
    }
    // 5. 如果找到，则向前查第一个开局库项
    while (lpbk >= Search.BookTable && lpbk->dwLock == bkToSearch.dwLock)
    {
        lpbk --;
    }
    lpbk ++;
    // 6. 把走法和分值写入到"mvs"和"vls"数组中
    vl = nBookMoves = 0;
    while (lpbk < Search.BookTable + Search.nBookSize && lpbk->dwLock == bkToSearch.dwLock)
    {
        mv = (bMirror ? MIRROR_MOVE(lpbk->wmv) : lpbk->wmv);
        if (pos.LegalMove(mv))
        {
            mvs[nBookMoves] = mv;
            vls[nBookMoves] = lpbk->wvl;
            vl += vls[nBookMoves];
            nBookMoves ++;
            if (nBookMoves == MAX_GEN_MOVES)
            {
                break; // 防止"BOOK.DAT"中含有异常数据
            }
        }
        lpbk ++;
    }
    if (vl == 0)
    {
        return 0; // 防止"BOOK.DAT"中含有异常数据
    }
    // 7. 根据权重随机选择一个走法
    vl = rand() % vl;
    for (i = 0; i < nBookMoves; i ++)
    {
        vl -= vls[i];
        if (vl < 0)
        {
            break;
        }
    }
    return mvs[i];
}

// 提取置换表项
static int ProbeHash(int vlAlpha, int vlBeta, int nDepth, int &mv)
{
    BOOL bMate; // 杀棋标志：如果是杀棋，那么不需要满足深度条件
    HashItem hsh;

    hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
    if (hsh.dwLock0 != pos.zobr.dwLock0 || hsh.dwLock1 != pos.zobr.dwLock1)
    {
        mv = 0;
        return -MATE_VALUE;
    }
    mv = hsh.wmv;
    bMate = FALSE;
    if (hsh.svl > WIN_VALUE)
    {
        if (hsh.svl < BAN_VALUE)
        {
            return -MATE_VALUE; // 可能导致搜索的不稳定性，立刻退出，但最佳着法可能拿到
        }
        hsh.svl -= pos.nDistance;
        bMate = TRUE;
    }
    else if (hsh.svl < -WIN_VALUE)
    {
        if (hsh.svl > -BAN_VALUE)
        {
            return -MATE_VALUE; // 同上
        }
        hsh.svl += pos.nDistance;
        bMate = TRUE;
    }
    if (hsh.ucDepth >= nDepth || bMate)
    {
        if (hsh.ucFlag == HASH_BETA)
        {
            return (hsh.svl >= vlBeta ? hsh.svl : -MATE_VALUE);
        }
        else if (hsh.ucFlag == HASH_ALPHA)
        {
            return (hsh.svl <= vlAlpha ? hsh.svl : -MATE_VALUE);
        }
        return hsh.svl;
    }
    return -MATE_VALUE;
};

// 保存置换表项
static void RecordHash(int nFlag, int vl, int nDepth, int mv)
{
    HashItem hsh;
    hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
    if (hsh.ucDepth > nDepth)
    {
        return;
    }
    hsh.ucFlag = nFlag;
    hsh.ucDepth = nDepth;
    if (vl > WIN_VALUE)
    {
        if (mv == 0 && vl <= BAN_VALUE)
        {
            return; // 可能导致搜索的不稳定性，并且没有最佳着法，立刻退出
        }
        hsh.svl = vl + pos.nDistance;
    }
    else if (vl < -WIN_VALUE)
    {
        if (mv == 0 && vl >= -BAN_VALUE)
        {
            return; // 同上
        }
        hsh.svl = vl - pos.nDistance;
    }
    else
    {
        hsh.svl = vl;
    }
    hsh.wmv = mv;
    hsh.dwLock0 = pos.zobr.dwLock0;
    hsh.dwLock1 = pos.zobr.dwLock1;
    Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)] = hsh;
};

// MVV/LVA每种子力的价值
static BYTE cucMvvLva[24] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 1, 1, 3, 4, 3, 2, 0,
    5, 1, 1, 3, 4, 3, 2, 0
};

// 求MVV/LVA值
inline int MvvLva(int mv)
{
    return (cucMvvLva[pos.ucpcSquares[DST(mv)]] << 3) - cucMvvLva[pos.ucpcSquares[SRC(mv)]];
}

// "qsort"按MVV/LVA值排序的比较函数
static int CompareMvvLva(const void *lpmv1, const void *lpmv2)
{
    return MvvLva(*(int *) lpmv2) - MvvLva(*(int *) lpmv1);
}

// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2)
{
    return Search.nHistoryTable[*(int *) lpmv2] - Search.nHistoryTable[*(int *) lpmv1];
}


// 走法排序阶段
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

// 走法排序结构
struct SortStruct
{
    int mvHash, mvKiller1, mvKiller2; // 置换表走法和两个杀手走法
    int nPhase, nIndex, nGenMoves;    // 当前阶段，当前采用第几个走法，总共有几个走法
    int mvs[MAX_GEN_MOVES];           // 所有的走法

    void Init(int mvHash_)
    { // 初始化，设定置换表走法和两个杀手走法
        mvHash = mvHash_;
        mvKiller1 = Search.mvKillers[pos.nDistance][0];
        mvKiller2 = Search.mvKillers[pos.nDistance][1];
        nPhase = PHASE_HASH;
    }
    int Next(void); // 得到下一个走法
};



// 对最佳走法的处理
inline void SetBestMove(int mv, int nDepth)
{
    int *lpmvKillers;
    Search.nHistoryTable[mv] += nDepth * nDepth;
    lpmvKillers = Search.mvKillers[pos.nDistance];
    if (lpmvKillers[0] != mv)
    {
        lpmvKillers[1] = lpmvKillers[0];
        lpmvKillers[0] = mv;
    }
}

// "GenerateMoves"参数
const BOOL GEN_CAPTURE = TRUE;

// 静态(Quiescence)搜索过程
static int SearchQuiesc(int vlAlpha, int vlBeta)
{
    int i, nGenMoves;
    int vl, vlBest;
    int mvs[MAX_GEN_MOVES];
    // 一个静态搜索分为以下几个阶段

    // 1. 检查重复局面
    vl = pos.RepStatus();
    if (vl != 0)
    {
        return pos.RepValue(vl);
    }

    // 2. 到达极限深度就返回局面评价
    if (pos.nDistance == LIMIT_DEPTH)
    {
        return pos.Evaluate();
    }

    // 3. 初始化最佳值
    vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)

    if (pos.InCheck())
    {
        // 4. 如果被将军，则生成全部走法
        nGenMoves = pos.GenerateMoves(mvs);
        qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
    }
    else
    {

        // 5. 如果不被将军，先做局面评价
        vl = pos.Evaluate();
        if (vl > vlBest)
        {
            vlBest = vl;
            if (vl >= vlBeta)
            {
                return vl;
            }
            if (vl > vlAlpha)
            {
                vlAlpha = vl;
            }
        }

        // 6. 如果局面评价没有截断，再生成吃子走法
        nGenMoves = pos.GenerateMoves(mvs, GEN_CAPTURE);
        qsort(mvs, nGenMoves, sizeof(int), CompareMvvLva);
    }

    // 7. 逐一走这些走法，并进行递归
    for (i = 0; i < nGenMoves; i ++)
    {
        if (pos.MakeMove(mvs[i]))
        {
            vl = -SearchQuiesc(-vlBeta, -vlAlpha);
            pos.UndoMakeMove();

            // 8. 进行Alpha-Beta大小判断和截断
            if (vl > vlBest)
            {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
                vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
                if (vl >= vlBeta)
                { // 找到一个Beta走法
                    return vl;        // Beta截断
                }
                if (vl > vlAlpha)
                { // 找到一个PV走法
                    vlAlpha = vl;     // 缩小Alpha-Beta边界
                }
            }
        }
    }

    // 9. 所有走法都搜索完了，返回最佳值
    return vlBest == -MATE_VALUE ? pos.nDistance - MATE_VALUE : vlBest;
}

// "SearchFull"的参数
const BOOL NO_NULL = TRUE;

// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
static int SearchFull(int vlAlpha, int vlBeta, int nDepth, BOOL bNoNull = FALSE)
{
    int nHashFlag, vl, vlBest;
    int mv, mvBest, mvHash, nNewDepth;
    SortStruct Sort;
    // 一个Alpha-Beta完全搜索分为以下几个阶段

    // 1. 到达水平线，则调用静态搜索(注意：由于空步裁剪，深度可能小于零)
    if (nDepth <= 0)
    {
        return SearchQuiesc(vlAlpha, vlBeta);
    }

    // 1-1. 检查重复局面(注意：不要在根节点检查，否则就没有走法了)
    vl = pos.RepStatus();
    if (vl != 0)
    {
        return pos.RepValue(vl);
    }

    // 1-2. 到达极限深度就返回局面评价
    if (pos.nDistance == LIMIT_DEPTH)
    {
        return pos.Evaluate();
    }

    // 1-3. 尝试置换表裁剪，并得到置换表走法
    vl = ProbeHash(vlAlpha, vlBeta, nDepth, mvHash);
    if (vl > -MATE_VALUE)
    {
        return vl;
    }

    // 1-4. 尝试空步裁剪(根节点的Beta值是"MATE_VALUE"，所以不可能发生空步裁剪)
    if (!bNoNull && !pos.InCheck() && pos.NullOkay())
    {
        pos.NullMove();
        vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
        pos.UndoNullMove();
        if (vl >= vlBeta)
        {
            return vl;
        }
    }

    // 2. 初始化最佳值和最佳走法
    nHashFlag = HASH_ALPHA;
    vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
    mvBest = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

    // 3. 初始化走法排序结构
    Sort.Init(mvHash);

    // 4. 逐一走这些走法，并进行递归
    while ((mv = Sort.Next()) != 0)
    {
        if (pos.MakeMove(mv))
        {
            // 将军延伸
            nNewDepth = pos.InCheck() ? nDepth : nDepth - 1;
            // PVS
            if (vlBest == -MATE_VALUE)
            {
                vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
            }
            else
            {
                vl = -SearchFull(-vlAlpha - 1, -vlAlpha, nNewDepth);
                if (vl > vlAlpha && vl < vlBeta)
                {
                    vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
                }
            }
            pos.UndoMakeMove();

            // 5. 进行Alpha-Beta大小判断和截断
            if (vl > vlBest)
            {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
                vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
                if (vl >= vlBeta)
                { // 找到一个Beta走法
                    nHashFlag = HASH_BETA;
                    mvBest = mv;      // Beta走法要保存到历史表
                    break;            // Beta截断
                }
                if (vl > vlAlpha)
                { // 找到一个PV走法
                    nHashFlag = HASH_PV;
                    mvBest = mv;      // PV走法要保存到历史表
                    vlAlpha = vl;     // 缩小Alpha-Beta边界
                }
            }
        }
    }

    // 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
    if (vlBest == -MATE_VALUE)
    {
        // 如果是杀棋，就根据杀棋步数给出评价
        return pos.nDistance - MATE_VALUE;
    }
    // 记录到置换表
    RecordHash(nHashFlag, vlBest, nDepth, mvBest);
    if (mvBest != 0)
    {
        // 如果不是Alpha走法，就将最佳走法保存到历史表
        SetBestMove(mvBest, nDepth);
    }
    return vlBest;
}

// 根节点的Alpha-Beta搜索过程
static int SearchRoot(int nDepth)
{
    int vl, vlBest, mv, nNewDepth;
    SortStruct Sort;

    vlBest = -MATE_VALUE;
    Sort.Init(Search.mvResult);
    while ((mv = Sort.Next()) != 0)
    {
        if (pos.MakeMove(mv))
        {
            nNewDepth = pos.InCheck() ? nDepth : nDepth - 1;
            if (vlBest == -MATE_VALUE)
            {
                vl = -SearchFull(-MATE_VALUE, MATE_VALUE, nNewDepth, NO_NULL);
            }
            else
            {
                vl = -SearchFull(-vlBest - 1, -vlBest, nNewDepth);
                if (vl > vlBest)
                {
                    vl = -SearchFull(-MATE_VALUE, -vlBest, nNewDepth, NO_NULL);
                }
            }
            pos.UndoMakeMove();
            if (vl > vlBest)
            {
                vlBest = vl;
                Search.mvResult = mv;
                if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE)
                {
                    vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
                }
            }
        }
    }
    RecordHash(HASH_PV, vlBest, nDepth, Search.mvResult);
    SetBestMove(Search.mvResult, nDepth);
    return vlBest;
}

// 迭代加深搜索过程
static void SearchMain(void)
{
    int i, t, vl, nGenMoves;
    int mvs[MAX_GEN_MOVES];

    // 初始化
    memset(Search.nHistoryTable, 0, 65536 * sizeof(int));       // 清空历史表
    memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // 清空杀手走法表
    memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));  // 清空置换表
    t = clock();       // 初始化定时器
    pos.nDistance = 0; // 初始步数

    // 搜索开局库
    Search.mvResult = SearchBook();
    if (Search.mvResult != 0)
    {
        pos.MakeMove(Search.mvResult);
        if (pos.RepStatus(3) == 0)
        {
            pos.UndoMakeMove();
            return;
        }
        pos.UndoMakeMove();
    }

    // 检查是否只有唯一走法
    vl = 0;
    nGenMoves = pos.GenerateMoves(mvs);
    for (i = 0; i < nGenMoves; i ++)
    {
        if (pos.MakeMove(mvs[i]))
        {
            pos.UndoMakeMove();
            Search.mvResult = mvs[i];
            vl ++;
        }
    }
    if (vl == 1)
    {
        return;
    }

    // 迭代加深过程
    for (i = 1; i <= LIMIT_DEPTH; i ++)
    {
        vl = SearchRoot(i);
        // 搜索到杀棋，就终止搜索
        if (vl > WIN_VALUE || vl < -WIN_VALUE)
        {
            break;
        }
        // 超过一秒，就终止搜索
        if (clock() - t > CLOCKS_PER_SEC)
        {
            break;
        }
    }
}

// TransparentBlt 的替代函数，用来修正原函数在 Windows 98 下资源泄漏的问题
static void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
                            HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent)
{
    HDC hImageDC, hMaskDC;
    HBITMAP hOldImageBMP, hImageBMP, hOldMaskBMP, hMaskBMP;

    hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);
    hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);
    hImageDC = CreateCompatibleDC(hdcDest);
    hMaskDC = CreateCompatibleDC(hdcDest);
    hOldImageBMP = (HBITMAP) SelectObject(hImageDC, hImageBMP);
    hOldMaskBMP = (HBITMAP) SelectObject(hMaskDC, hMaskBMP);

    if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
    {
        BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
            hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
    }
    else
    {
        StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
            hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
    }
    SetBkColor(hImageDC, crTransparent);
    BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
    SetBkColor(hImageDC, RGB(0,0,0));
    SetTextColor(hImageDC, RGB(255,255,255));
    BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);
    SetBkColor(hdcDest, RGB(255,255,255));
    SetTextColor(hdcDest, RGB(0,0,0));
    BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
        hMaskDC, 0, 0, SRCAND);
    BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
        hImageDC, 0, 0, SRCPAINT);

    SelectObject(hImageDC, hOldImageBMP);
    DeleteDC(hImageDC);
    SelectObject(hMaskDC, hOldMaskBMP);
    DeleteDC(hMaskDC);
    DeleteObject(hImageBMP);
    DeleteObject(hMaskBMP);
}


// 播放资源声音
inline void PlayResWav(const char* nResId)
{
    SimpleAudioEngine::getInstance()->playEffect(nResId);
}

// 弹出不带声音的提示框
static void MessageBoxMute(const char* lpszText)
{
    MessageBox(lpszText, _U("中国象棋"));
}

static void SetSelect(int sq, bool bSel = false)
{

}

// 电脑回应一步棋
static void ResponseMove(void)
{
    int vlRep;
    // 电脑走一步棋
    SearchMain();
    pos.MakeMove(Search.mvResult);
    // 清除上一步棋的选择标记
    SetSelect(SRC(Xqwl.mvLast));
    SetSelect(DST(Xqwl.mvLast));
    // 把电脑走的棋标记出来
    Xqwl.mvLast = Search.mvResult;
    SetSelect(SRC(Xqwl.mvLast), true);
    SetSelect(DST(Xqwl.mvLast), true);
    // 检查重复局面
    vlRep = pos.RepStatus(3);
    if (pos.IsMate())
    {
        // 如果分出胜负，那么播放胜负的声音，并且弹出不带声音的提示框
        PlayResWav(IDR_LOSS);
        MessageBoxMute("请再接再厉！");
        Xqwl.bGameOver = TRUE;
    }
    else if (vlRep > 0)
    {
        vlRep = pos.RepValue(vlRep);
        // 注意："vlRep"是对玩家来说的分值
        PlayResWav(vlRep < -WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
        MessageBoxMute(vlRep < -WIN_VALUE ? "长打作负，请不要气馁！" :
            vlRep > WIN_VALUE ? "电脑长打作负，祝贺你取得胜利！" : "双方不变作和，辛苦了！");
        Xqwl.bGameOver = TRUE;
    }
    else if (pos.nMoveNum > 100)
    {
        PlayResWav(IDR_DRAW);
        MessageBoxMute("超过自然限着作和，辛苦了！");
        Xqwl.bGameOver = TRUE;
    }
    else
    {
        // 如果没有分出胜负，那么播放将军、吃子或一般走子的声音
        PlayResWav(pos.InCheck() ? IDR_CHECK2 : pos.Captured() ? IDR_CAPTURE2 : IDR_MOVE2);
        if (pos.Captured())
        {
            pos.SetIrrev();
        }
    }
}

// 点击格子事件处理
static void ClickSquare(int sq)
{
    int pc, mv, vlRep;
    sq = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
    pc = pos.ucpcSquares[sq];

    if ((pc & SIDE_TAG(pos.sdPlayer)) != 0)
    {
        // 如果点击自己的子，那么直接选中该子
        if (Xqwl.sqSelected != 0)
        {
            SetSelect(Xqwl.sqSelected);
        }
        Xqwl.sqSelected = sq;
        SetSelect(sq, true);
        if (Xqwl.mvLast != 0)
        {
            SetSelect(SRC(Xqwl.mvLast));
            SetSelect(DST(Xqwl.mvLast));
        }
        PlayResWav(IDR_CLICK); // 播放点击的声音

    }
    else if (Xqwl.sqSelected != 0 && !Xqwl.bGameOver)
    {
        // 如果点击的不是自己的子，但有子选中了(一定是自己的子)，那么走这个子
        mv = MOVE(Xqwl.sqSelected, sq);
        if (pos.LegalMove(mv))
        {
            if (pos.MakeMove(mv))
            {
                Xqwl.mvLast = mv;
                SetSelect(Xqwl.sqSelected, true);
                SetSelect(sq, true);
                Xqwl.sqSelected = 0;
                // 检查重复局面
                vlRep = pos.RepStatus(3);
                if (pos.IsMate())
                {
                    // 如果分出胜负，那么播放胜负的声音，并且弹出不带声音的提示框
                    PlayResWav(IDR_WIN);
                    MessageBoxMute("祝贺你取得胜利！");
                    Xqwl.bGameOver = TRUE;
                }
                else if (vlRep > 0)
                {
                    vlRep = pos.RepValue(vlRep);
                    // 注意："vlRep"是对电脑来说的分值
                    PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
                    MessageBoxMute(vlRep > WIN_VALUE ? "长打作负，请不要气馁！" :
                        vlRep < -WIN_VALUE ? "电脑长打作负，祝贺你取得胜利！" : "双方不变作和，辛苦了！");
                    Xqwl.bGameOver = TRUE;
                }
                else if (pos.nMoveNum > 100)
                {
                    PlayResWav(IDR_DRAW);
                    MessageBoxMute("超过自然限着作和，辛苦了！");
                    Xqwl.bGameOver = TRUE;
                }
                else
                {
                    // 如果没有分出胜负，那么播放将军、吃子或一般走子的声音
                    PlayResWav(pos.InCheck() ? IDR_CHECK : pos.Captured() ? IDR_CAPTURE : IDR_MOVE);
                    if (pos.Captured())
                    {
                        pos.SetIrrev();
                    }
                    ResponseMove(); // 轮到电脑走棋
                }
            }
            else
            {
                PlayResWav(IDR_ILLEGAL); // 播放被将军的声音
            }
        }
        // 如果根本就不符合走法(例如马不走日字)，那么程序不予理会
    }
}

// 初始化棋局
static void Startup(void)
{
    pos.Startup();
    Xqwl.sqSelected = Xqwl.mvLast = 0;
    Xqwl.bGameOver = FALSE;
}

static void SetPieceColor(bool bRed)
{
    Xqwl.bFlipped = !bRed;
    Startup();
    if (Xqwl.bFlipped)
    {
        ResponseMove();
    }
}

static void OnBoardTouch()
{
    int lParam;
    int x = FILE_LEFT + (LOWORD(lParam) );
    int y = RANK_TOP + (HIWORD(lParam) );
    if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM)
    {
        ClickSquare(COORD_XY(x, y));
    }
}

#include "BoardLayer.h"

// 入口过程
extern BoardLayer* spBoard;

static void Initialize(BoardLayer* pBoard)
{
    spBoard = pBoard;

    // 初始化全局变量
    srand((DWORD) time(NULL));
    InitZobrist();
    LoadBook();
    Xqwl.bFlipped = FALSE;
    Startup();
}

#endif // __APP_INC_CHESS_HH__

