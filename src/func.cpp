#include "func.h"
#include <cstdio>
#include <graphics.h>
#include <sstream>

using namespace std;

const int SCR_W = 1000;
const int SCR_H = 650;

bool parseInput(const string &input, int &r, int &c, char &action)
{
    stringstream ss(input);
    string coord, act;
    if (!(ss >> coord >> act))
        return false;
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

void drawConnectionMsg(const char *line1, const char *line2)
{
    cleardevice();
    setbkcolor(EGERGB(245, 222, 179));
    setfont(22, 0, "Arial");
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    outtextxy((SCR_W - textwidth(line1)) / 2, 280, line1);
    if (line2)
        outtextxy((SCR_W - textwidth(line2)) / 2, 315, line2);
}

bool doAction(Minesweeper &game, char type, int r, int c,
              bool &hitMine, bool &won)
{
    hitMine = false;
    won = false;
    if (type == 'R')
    {
        hitMine = game.reveal(r, c);
        won = game.checkWin();
    }
    else if (type == 'F')
    {
        game.toggleFlag(r, c);
        won = game.checkWin();
    }
    else if (type == 'C')
    {
        hitMine = game.revealAdjacent(r, c);
        won = game.checkWin();
    }
    return true;
}

void sendOp(Network &net, char type, int r, int c)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%c %d %d", type, r, c);
    net.sendMsg(buf);
}