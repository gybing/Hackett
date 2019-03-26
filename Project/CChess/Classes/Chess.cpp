#include "Chess.h"

// 用空密钥初始化密码流生成器
void RC4Struct::InitZero(void)
{
    int i, j;
    BYTE uc;

    x = y = j = 0;
    for (i = 0; i < 256; i ++)
    {
        s[i] = i;
    }
    for (i = 0; i < 256; i ++)
    {
        j = (j + s[i]) & 255;
        uc = s[i];
        s[i] = s[j];
        s[j] = uc;
    }
}

// 得到下一个走法
int SortStruct::Next(void)
{
    int mv;
    switch (nPhase)
    {
        // "nPhase"表示着法启发的若干阶段，依次为：

        // 0. 置换表着法启发，完成后立即进入下一阶段；
    case PHASE_HASH:
        nPhase = PHASE_KILLER_1;
        if (mvHash != 0)
        {
            return mvHash;
        }
        // 技巧：这里没有"break"，表示"switch"的上一个"case"执行完后紧接着做下一个"case"，下同

        // 1. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；
    case PHASE_KILLER_1:
        nPhase = PHASE_KILLER_2;
        if (mvKiller1 != mvHash && mvKiller1 != 0 && pos.LegalMove(mvKiller1))
        {
            return mvKiller1;
        }

        // 2. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
    case PHASE_KILLER_2:
        nPhase = PHASE_GEN_MOVES;
        if (mvKiller2 != mvHash && mvKiller2 != 0 && pos.LegalMove(mvKiller2))
        {
            return mvKiller2;
        }

        // 3. 生成所有着法，完成后立即进入下一阶段；
    case PHASE_GEN_MOVES:
        nPhase = PHASE_REST;
        nGenMoves = pos.GenerateMoves(mvs);
        qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
        nIndex = 0;

        // 4. 对剩余着法做历史表启发；
    case PHASE_REST:
        while (nIndex < nGenMoves)
        {
            mv = mvs[nIndex];
            nIndex ++;
            if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2)
            {
                return mv;
            }
        }

        // 5. 没有着法了，返回零。
    default:
        return 0;
    }
}
