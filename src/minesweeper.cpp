#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <algorithm>
#include <windows.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

// 方向数组，用于遍历周围8格
const int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

class Minesweeper
{
private:
    int rows, cols, mines;
    vector<vector<bool>> isMine;       // 是否有雷
    vector<vector<bool>> isRevealed;   // 是否已翻开
    vector<vector<bool>> isFlagged;    // 是否插旗
    vector<vector<int>> adjacentMines; // 周围雷数
    bool gameOver;
    bool firstMove; // 是否为第一步（可用于避免第一步踩雷）

    // 检查坐标是否在棋盘内
    bool inBounds(int r, int c) const
    {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    // 计算 (r,c) 周围雷数
    int countAdjacentMines(int r, int c) const
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
    void placeMines(int safeR, int safeC)
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
    void revealCell(int r, int c)
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

public:
    Minesweeper(int r, int c, int m) : rows(r), cols(c), mines(m),
                                       gameOver(false), firstMove(true)
    {
        // 初始化二维数组
        isMine.assign(rows, vector<bool>(cols, false));
        isRevealed.assign(rows, vector<bool>(cols, false));
        isFlagged.assign(rows, vector<bool>(cols, false));
        adjacentMines.assign(rows, vector<int>(cols, 0));
        srand(static_cast<unsigned>(time(nullptr)));
    }

    // 执行一次翻开操作
    bool reveal(int r, int c)
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
    void toggleFlag(int r, int c)
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
    bool checkWin() const
    {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                if (!isMine[i][j] && !isRevealed[i][j])
                    return false;
        return true;
    }

    bool isGameOver() const { return gameOver; }

    // 打印棋盘
    void printBoard() const
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
    void revealAll()
    {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                if (isMine[i][j])
                    isRevealed[i][j] = true;
    }
};

// 解析形如 "3,5 r" 的输入
bool parseInput(const string &input, int &r, int &c, char &action)
{
    stringstream ss(input);
    string coord, act;
    if (!(ss >> coord >> act))
        return false;
    // 从坐标中提取行和列，格式为 "row,col"
    size_t comma = coord.find(',');
    if (comma == string::npos)
        return false;
    try
    {
        r = stoi(coord.substr(0, comma));
        c = stoi(coord.substr(comma + 1));
    }
    catch (...)
    {
        return false;
    }
    if (act.size() != 1 || (act[0] != 'r' && act[0] != 'f'))
        return false;
    action = act[0];
    return true;
}

int main()
{
    int rows, cols, mines;
    cout << "welcome to minesweeper game!\n";
    cout << "please enter number of rows and columns (e.g. 10 10 15):";
    cin >> rows >> cols >> mines;
    if (rows <= 0 || cols <= 0 || mines <= 0 || mines >= rows * cols)
    {
        cout << "参数不合法，程序退出。\n";
        return 1;
    }

    Minesweeper game(rows, cols, mines);
    cin.ignore(); // 忽略之前留下的换行符

    while (!game.isGameOver() && !game.checkWin())
    {
        game.printBoard();
        cout << "input format: row, column operation (r=flip, f=flag insertion/cancel)\n";
        cout << "example: 3,5 r\n";
        cout << "please enter:";
        string line;
        getline(cin, line);
        int r, c;
        char action;
        if (!parseInput(line, r, c, action))
        {
            cout << "input format error, please re-enter.\n";
            continue;
        }
        if (action == 'r')
        {
            bool hitMine = game.reveal(r, c);
            if (hitMine)
            {
                game.printBoard();
                cout << "unfortunately, you stepped on lightning! game over.\n";
                break;
            }
        }
        else if (action == 'f')
        {
            game.toggleFlag(r, c);
        }
    }

    if (game.checkWin())
    {
        game.printBoard();
        cout << "congratulations, you have won!\n";
    }

    system("pause");
    return 0;
}