#pragma once
#include "../AXBase/AXNetwork.h"
#include "AXDBStructs.h"

class AXDown : public Thread
{
public:
    AXDown(const TDSong* u);
    ~AXDown();

private:
    virtual void run() override;
    TDSong music;
};
