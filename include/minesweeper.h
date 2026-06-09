#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// 方向数组，用于遍历周围8格
const int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

class Minesweeper
{
    friend class GameRender;

private:
    int rows, cols, mines;
    vector<vector<bool>> isMine;       // 是否有雷
    vector<vector<bool>> isRevealed;   // 是否已翻开
    vector<vector<bool>> isFlagged;    // 是否插旗
    vector<vector<int>> adjacentMines; // 周围雷数
    bool gameOver;
    bool firstMove; // 是否为第一步（可用于避免第一步踩雷）

    // 检查坐标是否在棋盘内
    bool inBounds(int r, int c) const;

    // 计算 (r,c) 周围雷数
    int countAdjacentMines(int r, int c) const;

    // 放置地雷，避开第一次点击的格子 (safeR, safeC)
    void placeMines(int safeR, int safeC);

    // 递归翻开空格子 (BFS/DFS)
    void revealCell(int r, int c);

public:
    Minesweeper(int r, int c, int m);

    // 执行一次翻开操作
    bool reveal(int r, int c);

    // 标记/取消标记
    void toggleFlag(int r, int c);

    // 检查是否胜利（所有非雷格子都已翻开）
    bool checkWin() const;

    bool isGameOver() const;

    // 打印棋盘
    void printBoard() const;

    // 显示所有雷（调试用或结束后显示，此处已整合进 reveal）
    void revealAll();
};

#endif