#include "gamesystem.h"
#include <iostream>

using namespace std;

GameSystem::GameSystem() : game(nullptr)
{
}

GameSystem::~GameSystem()
{
    delete game;
}

bool GameSystem::start()
{
    int rows, cols, mines;
    cout << "welcome to minesweeper game!\n";
    cout << "please enter number of rows and columns (e.g. 10 10 15):";
    cin >> rows >> cols >> mines;
    if (rows <= 0 || cols <= 0 || mines <= 0 || mines >= rows * cols)
    {
        cout << "参数不合法，程序退出。\n";
        return false;
    }

    delete game;
    game = new Minesweeper(rows, cols, mines);
    cin.ignore(); // 忽略之前留下的换行符
    return true;
}

bool GameSystem::start(int rows, int cols, int mines)
{
    delete game;
    game = new Minesweeper(rows, cols, mines);
    return true;
}

void GameSystem::run()
{
    while (!game->isGameOver() && !game->checkWin())
    {
        game->printBoard();
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
            bool hitMine = game->reveal(r, c);
            if (hitMine)
            {
                game->printBoard();
                cout << "unfortunately, you stepped on lightning! game over.\n";
                break;
            }
        }
        else if (action == 'f')
        {
            game->toggleFlag(r, c);
        }
    }
}

void GameSystem::end()
{
    if (game && game->checkWin())
    {
        game->printBoard();
        cout << "congratulations, you have won!\n";
    }
    system("pause");
}