#include <windows.h>
#include "gamerender.h"

GameRender::GameRender(Minesweeper *g, int cell, int mar)
    : game(g), cellSize(cell), margin(mar), infoBarHeight(50)
{
    windowW = margin * 2 + game->cols * cellSize;
    windowH = margin * 2 + game->rows * cellSize + infoBarHeight;

    bgColor = EGERGB(245, 222, 179);
    cellColor = EGERGB(210, 180, 140);
    revealedColor = EGERGB(235, 230, 200);
    flagColor = EGERGB(220, 60, 60);
    mineColor = EGERGB(50, 50, 50);
    borderColor = EGERGB(100, 80, 60);

    numberColors[0] = BLUE;
    numberColors[1] = EGERGB(0, 128, 0);
    numberColors[2] = EGERGB(220, 50, 50);
    numberColors[3] = EGERGB(0, 0, 139);
    numberColors[4] = EGERGB(139, 0, 0);
    numberColors[5] = EGERGB(0, 128, 128);
    numberColors[6] = BLACK;
    numberColors[7] = EGERGB(128, 128, 128);
}

GameRender::~GameRender()
{
}

void GameRender::init()
{
    initgraph(windowW, windowH);
    setbkcolor(bgColor);
    cleardevice();
}

void GameRender::drawCell(int r, int c)
{
    int x = margin + c * cellSize;
    int y = margin + r * cellSize;
    int pad = 2;

    if (game->isRevealed[r][c])
    {
        setfillcolor(revealedColor);
        bar(x + pad, y + pad, x + cellSize - pad, y + cellSize - pad);

        if (game->isMine[r][c])
        {
            setfillcolor(EGERGB(255, 80, 80));
            bar(x + pad, y + pad, x + cellSize - pad, y + cellSize - pad);
            settextcolor(mineColor);
            setbkmode(TRANSPARENT);
            setfont(cellSize - 4, 0, "Arial");
            int tx = x + (cellSize - textwidth('*')) / 2;
            int ty = y + (cellSize - textheight('*')) / 2 - 2;
            outtextxy(tx, ty, '*');
        }
        else if (game->adjacentMines[r][c] > 0)
        {
            int num = game->adjacentMines[r][c];
            settextcolor(numberColors[num - 1]);
            setbkmode(TRANSPARENT);
            setfont(cellSize - 4, 0, "Arial");
            char str[2] = {char('0' + num), '\0'};
            int tx = x + (cellSize - textwidth(str)) / 2;
            int ty = y + (cellSize - textheight(str)) / 2 - 2;
            outtextxy(tx, ty, str);
        }
    }
    else
    {
        setfillcolor(cellColor);
        bar(x + pad, y + pad, x + cellSize - pad, y + cellSize - pad);

        if (game->isFlagged[r][c])
        {
            settextcolor(flagColor);
            setbkmode(TRANSPARENT);
            setfont(cellSize - 4, 0, "Arial");
            int tx = x + (cellSize - textwidth('F')) / 2;
            int ty = y + (cellSize - textheight('F')) / 2 - 2;
            outtextxy(tx, ty, 'F');
        }
    }
}

void GameRender::drawBoard()
{
    for (int r = 0; r < game->rows; ++r)
    {
        for (int c = 0; c < game->cols; ++c)
        {
            drawCell(r, c);
        }
    }

    setlinecolor(borderColor);
    for (int r = 0; r <= game->rows; ++r)
    {
        int y = margin + r * cellSize;
        line(margin, y, margin + game->cols * cellSize, y);
    }
    for (int c = 0; c <= game->cols; ++c)
    {
        int x = margin + c * cellSize;
        line(x, margin, x, margin + game->rows * cellSize);
    }
}

void GameRender::drawInfoBar()
{
    int y = margin + game->rows * cellSize + 10;
    settextcolor(EGERGB(80, 60, 40));
    setbkmode(TRANSPARENT);
    setfont(16, 0, "Arial");
    outtextxy(margin, y, "Left Click: Reveal   Right Click: Flag");
}

void GameRender::render()
{
    cleardevice();
    drawBoard();
    drawInfoBar();
}

bool GameRender::handleMouse(mouse_msg msg, bool &hitMine, bool &won)
{
    if (msg.msg != mouse_msg_up)
        return false;

    int c = (msg.x - margin) / cellSize;
    int r = (msg.y - margin) / cellSize;

    if (r < 0 || r >= game->rows || c < 0 || c >= game->cols)
        return false;

    if (msg.is_left())
    {
        hitMine = game->reveal(r, c);
        won = game->checkWin();
        return true;
    }

    if (msg.is_right())
    {
        game->toggleFlag(r, c);
        return true;
    }

    return false;
}

void GameRender::showMessage(const char *text)
{
    int msgW = textwidth(text) + 40;
    int msgH = textheight(text) + 20;
    int msgX = (windowW - msgW) / 2;
    int msgY = (windowH - msgH) / 2 - infoBarHeight / 2;

    setfillcolor(EGERGB(255, 255, 255));
    setlinecolor(borderColor);
    bar(msgX, msgY, msgX + msgW, msgY + msgH);
    rectangle(msgX, msgY, msgX + msgW, msgY + msgH);

    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    setfont(20, 0, "Arial");
    int tx = msgX + (msgW - textwidth(text)) / 2;
    int ty = msgY + (msgH - textheight(text)) / 2;
    outtextxy(tx, ty, text);
}