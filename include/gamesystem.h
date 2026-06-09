#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include "minesweeper.h"
#include "func.h"

class GameSystem
{
private:
    Minesweeper *game;

public:
    GameSystem();
    ~GameSystem();

    // 开始游戏（读取参数、初始化）
    bool start();

    // 游戏主循环
    void run();

    // 结束处理（显示结果）
    void end();
};

#endif