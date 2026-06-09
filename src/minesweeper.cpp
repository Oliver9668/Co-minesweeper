#include <windows.h>
#include "minesweeper.h"
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

// ==================== Minesweeper 类实现 ====================

Minesweeper::Minesweeper(int r, int c, int m) : rows(r), cols(c), mines(m),
                                                gameOver(false), firstMove(true)
{
    // 初始化二维数组
    isMine.assign(rows, vector<bool>(cols, false));
    isRevealed.assign(rows, vector<bool>(cols, false));
    isFlagged.assign(rows, vector<bool>(cols, false));
    adjacentMines.assign(rows, vector<int>(cols, 0));
    srand(static_cast<unsigned>(time(nullptr)));
}

// 检查坐标是否在棋盘内
bool Minesweeper::inBounds(int r, int c) const
{
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

// 计算 (r,c) 周围雷数
int Minesweeper::countAdjacentMines(int r, int c) const
{
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        int nr = r + dx[i], nc = c + dy[i];
        if (inBounds(nr, nc) && isMine[nr][nc])
            ++count;
    }
    return count;
}

// 放置地雷，避开第一次点击的格子 (safeR, safeC)
void Minesweeper::placeMines(int safeR, int safeC)
{
    int totalCells = rows * cols;
    int placed = 0;
    while (placed < mines)
    {
        int r = rand() % rows;
        int c = rand() % cols;
        // 不在安全格子上放雷，且该格尚未放雷
        if ((r == safeR && c == safeC) || isMine[r][c])
            continue;
        isMine[r][c] = true;
        ++placed;
    }
    // 计算所有格子的周围雷数
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (!isMine[i][j])
                adjacentMines[i][j] = countAdjacentMines(i, j);
        }
    }
}

// 递归翻开空格子 (BFS/DFS)
void Minesweeper::revealCell(int r, int c)
{
    if (!inBounds(r, c) || isRevealed[r][c] || isFlagged[r][c])
        return;
    isRevealed[r][c] = true;
    // 如果周围没有雷，递归翻开相邻格子
    if (adjacentMines[r][c] == 0 && !isMine[r][c])
    {
        for (int i = 0; i < 8; ++i)
        {
            int nr = r + dx[i], nc = c + dy[i];
            revealCell(nr, nc);
        }
    }
}

// 执行一次翻开操作
bool Minesweeper::reveal(int r, int c)
{
    if (!inBounds(r, c) || gameOver)
        return false;
    if (isRevealed[r][c] || isFlagged[r][c])
    {
        cout << "grid has been opened or flag has been inserted, please re-enter.\n";
        return false;
    }
    // 第一步自动布雷，保证不会踩雷
    if (firstMove)
    {
        placeMines(r, c);
        firstMove = false;
    }
    if (isMine[r][c])
    {
        // 踩雷，游戏结束
        gameOver = true;
        // 翻开所有雷
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                if (isMine[i][j])
                    isRevealed[i][j] = true;
        return true; // 表示触雷
    }
    revealCell(r, c);
    return false;
}

// 标记/取消标记
void Minesweeper::toggleFlag(int r, int c)
{
    if (!inBounds(r, c) || gameOver)
        return;
    if (isRevealed[r][c])
    {
        cout << "banners cannot be inserted into opened squares.\n";
        return;
    }
    isFlagged[r][c] = !isFlagged[r][c];
}

// 检查是否胜利（所有非雷格子都已翻开）
bool Minesweeper::checkWin() const
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (!isMine[i][j] && !isRevealed[i][j])
                return false;
    return true;
}

bool Minesweeper::isGameOver() const
{
    return gameOver;
}

// 打印棋盘
void Minesweeper::printBoard() const
{
    system(CLEAR); // 清屏，使界面整洁
    // 打印列号
    cout << "    ";
    for (int j = 0; j < cols; ++j)
        cout << j % 10 << " ";
    cout << "\n";
    // 打印上边框
    cout << "  +";
    for (int j = 0; j < cols; ++j)
        cout << "--";
    cout << "-+\n";
    // 打印行
    for (int i = 0; i < rows; ++i)
    {
        cout << i % 10 << " | ";
        for (int j = 0; j < cols; ++j)
        {
            if (isRevealed[i][j])
            {
                if (isMine[i][j])
                    cout << "* ";
                else if (adjacentMines[i][j] == 0)
                    cout << "  ";
                else
                    cout << adjacentMines[i][j] << " ";
            }
            else if (isFlagged[i][j])
            {
                cout << "F ";
            }
            else
            {
                cout << "# ";
            }
        }
        cout << "|\n";
    }
    cout << "  +";
    for (int j = 0; j < cols; ++j)
        cout << "--";
    cout << "-+\n";
}

// 显示所有雷（调试用或结束后显示，此处已整合进 reveal）
void Minesweeper::revealAll()
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (isMine[i][j])
                isRevealed[i][j] = true;
}