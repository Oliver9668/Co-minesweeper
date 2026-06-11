#include <windows.h>
#include <cstdio>
#include "gamerender.h"

GameRender::GameRender(Minesweeper *g, int cell, int mar, int scrW, int scrH)
    : game(g), cellSize(cell), margin(mar), infoBarHeight(50),
      toolbarHeight(40), screenW(scrW), screenH(scrH),
      lastClickTime(0), lastClickR(-1), lastClickC(-1),
      lastActionR(-1), lastActionC(-1),
      scrollY(0), maxScrollY(0)
{
    gridY = margin + toolbarHeight;
    windowW = margin * 2 + game->cols * cellSize;
    int fullBoardHeight = margin * 2 + game->rows * cellSize + toolbarHeight;
    windowH = fullBoardHeight;

    offsetX = (screenW - windowW) / 2;

    boardViewH = screenH;
    int availableBoardH = screenH - (margin + toolbarHeight);
    int totalBoardH = game->rows * cellSize;

    if (totalBoardH > availableBoardH)
    {
        offsetY = 0;
        maxScrollY = totalBoardH - availableBoardH + cellSize;
        scrollY = 0;
    }
    else
    {
        offsetY = (screenH - fullBoardHeight) / 2;
        maxScrollY = 0;
        scrollY = 0;
    }

    exitBtnW = 60; exitBtnH = 28;
    exitBtnX = margin + game->cols * cellSize - exitBtnW;
    exitBtnY = margin;

    bgColor = EGERGB(245, 222, 179);
    cellColor = EGERGB(210, 180, 140);
    cellHighlight = EGERGB(240, 210, 170);
    cellShadow = EGERGB(155, 125, 88);
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
    setbkcolor(bgColor);
    cleardevice();
}

void GameRender::drawCell(int r, int c)
{
    int x = offsetX + margin + c * cellSize;
    int y = offsetY + gridY + r * cellSize - scrollY;

    if (game->isRevealed[r][c])
    {
        setfillcolor(revealedColor);
        bar(x, y, x + cellSize, y + cellSize);

        if (game->isMine[r][c])
        {
            setfillcolor(EGERGB(255, 80, 80));
            bar(x, y, x + cellSize, y + cellSize);
            settextcolor(mineColor);
            setbkmode(TRANSPARENT);
            setfont(cellSize - 4, 0, "Arial");
            int tx = x + (cellSize - textwidth('*')) / 2 + 1;
            int ty = y + (cellSize - textheight('*')) / 2;
            outtextxy(tx, ty, '*');
        }
        else if (game->adjacentMines[r][c] > 0)
        {
            int num = game->adjacentMines[r][c];
            settextcolor(numberColors[num - 1]);
            setbkmode(TRANSPARENT);
            setfont(cellSize - 4, 0, "Arial");
            char str[2] = {char('0' + num), '\0'};
            int tx = x + (cellSize - textwidth(str)) / 2 + 1;
            int ty = y + (cellSize - textheight(str)) / 2;
            outtextxy(tx, ty, str);
        }
    }
    else
    {
        setfillcolor(cellColor);
        bar(x, y, x + cellSize, y + cellSize);

        setfillcolor(cellHighlight);
        bar(x, y, x + cellSize, y + 2);
        bar(x, y, x + 2, y + cellSize);

        setfillcolor(cellShadow);
        bar(x, y + cellSize - 2, x + cellSize, y + cellSize);
        bar(x + cellSize - 2, y, x + cellSize, y + cellSize);

        if (game->isFlagged[r][c])
        {
            int flagW = (int)(cellSize * 0.45);
            int flagH = (int)(cellSize * 0.35);
            int centerX = x + cellSize / 2;
            int centerY = y + cellSize / 2;

            int poleX = centerX - flagW / 2;
            int poleTop = centerY - flagH;
            int poleBottom = centerY + flagH;
            int triLeft = poleX;
            int triRight = poleX + flagW;
            int triTop = poleTop;
            int triBottom = triTop + flagH;
            int triMidY = (triTop + triBottom) / 2;

            setfillcolor(flagColor);
            setlinecolor(flagColor);
            ege_point pts[3] = {
                {(float)triLeft, (float)triTop},
                {(float)triRight, (float)triMidY},
                {(float)triLeft, (float)triBottom}
            };
            ege_fillpoly(3, pts);

            setlinecolor(EGERGB(80, 40, 0));
            line(poleX, poleTop, poleX, poleBottom);
        }
    }
}

void GameRender::drawBoard()
{
    int boardTop = offsetY + gridY;
    int firstRow = 0;
    int lastRow = game->rows - 1;

    if (maxScrollY > 0)
    {
        firstRow = scrollY / cellSize;
        lastRow = (scrollY + boardViewH - boardTop) / cellSize;
        if (lastRow >= game->rows) lastRow = game->rows - 1;
        if (firstRow < 0) firstRow = 0;
    }

    for (int r = firstRow; r <= lastRow; ++r)
        for (int c = 0; c < game->cols; ++c)
            drawCell(r, c);

    setlinecolor(borderColor);
    for (int r = 0; r <= game->rows; ++r)
    {
        int y = offsetY + gridY + r * cellSize - scrollY;
        line(offsetX + margin, y,
             offsetX + margin + game->cols * cellSize, y);
    }
    for (int c = 0; c <= game->cols; ++c)
    {
        int x = offsetX + margin + c * cellSize;
        line(x, offsetY + gridY - scrollY,
             x, offsetY + gridY + game->rows * cellSize - scrollY);
    }
}

void GameRender::drawToolbar()
{
    setfillcolor(bgColor);
    bar(0, 0, screenW, offsetY + gridY);

    int flags = game->getFlagCount();
    char flagStr[32];
    snprintf(flagStr, sizeof(flagStr), "Flags: %d / %d", flags, game->mines);
    setfont(18, 0, "Arial");
    int flagTextW = textwidth(flagStr);
    int flagTextH = textheight(flagStr);
    int flagBoxW = flagTextW + 16;
    int flagBoxH = exitBtnH;
    int flagBoxX = offsetX + margin;
    int flagBoxY = offsetY + margin;

    setfillcolor(EGERGB(255, 255, 255));
    setlinecolor(borderColor);
    bar(flagBoxX, flagBoxY, flagBoxX + flagBoxW, flagBoxY + flagBoxH);
    rectangle(flagBoxX, flagBoxY, flagBoxX + flagBoxW, flagBoxY + flagBoxH);

    settextcolor(EGERGB(80, 60, 40));
    setbkmode(TRANSPARENT);
    int tx = flagBoxX + (flagBoxW - flagTextW) / 2;
    int ty = flagBoxY + (flagBoxH - flagTextH) / 2;
    outtextxy(tx, ty, flagStr);

    setfillcolor(EGERGB(220, 80, 80));
    setlinecolor(EGERGB(180, 40, 40));
    bar(offsetX + exitBtnX, offsetY + exitBtnY,
        offsetX + exitBtnX + exitBtnW, offsetY + exitBtnY + exitBtnH);
    rectangle(offsetX + exitBtnX, offsetY + exitBtnY,
              offsetX + exitBtnX + exitBtnW, offsetY + exitBtnY + exitBtnH);
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    setfont(16, 0, "Arial");
    const char *exitText = "Exit";
    int ex = offsetX + exitBtnX + (exitBtnW - textwidth(exitText)) / 2;
    int ey = offsetY + exitBtnY + (exitBtnH - textheight(exitText)) / 2;
    outtextxy(ex, ey, exitText);
}

void GameRender::drawInfoBar()
{
}

void GameRender::drawScrollbar()
{
    if (maxScrollY <= 0) return;

    int totalBoardH = game->rows * cellSize;
    int availableBoardH = screenH - (margin + toolbarHeight);
    int trackY = margin + toolbarHeight;
    int trackH = availableBoardH;
    int trackX = screenW - 12;
    int trackW = 8;

    setfillcolor(EGERGB(200, 190, 170));
    bar(trackX, trackY, trackX + trackW, trackY + trackH);

    int thumbH = (int)((long long)availableBoardH * availableBoardH / totalBoardH);
    if (thumbH < 20) thumbH = 20;
    int thumbTravel = trackH - thumbH;
    int thumbY = trackY + (int)((long long)scrollY * thumbTravel / maxScrollY);

    setfillcolor(EGERGB(160, 140, 120));
    setlinecolor(EGERGB(120, 100, 80));
    bar(trackX, thumbY, trackX + trackW, thumbY + thumbH);
}

void GameRender::render()
{
    cleardevice();
    drawBoard();
    drawScrollbar();
    drawToolbar();
}

bool GameRender::handleScroll(mouse_msg msg)
{
    if (msg.msg != mouse_msg_wheel)
        return false;

    if (maxScrollY <= 0)
        return false;

    int scrollStep = cellSize * 3;
    if (msg.wheel > 0)
        scrollY -= scrollStep;
    else
        scrollY += scrollStep;

    if (scrollY < 0) scrollY = 0;
    if (scrollY > maxScrollY) scrollY = maxScrollY;

    return true;
}

bool GameRender::handleMouse(mouse_msg msg, bool &hitMine, bool &won,
                              bool &exitRequested)
{
    exitRequested = false;

    if (msg.msg != mouse_msg_up)
        return false;

    if (msg.x >= offsetX + exitBtnX &&
        msg.x <= offsetX + exitBtnX + exitBtnW &&
        msg.y >= offsetY + exitBtnY &&
        msg.y <= offsetY + exitBtnY + exitBtnH)
    {
        exitRequested = true;
        return true;
    }

    int c = (msg.x - offsetX - margin) / cellSize;
    int r = (msg.y - offsetY - gridY + scrollY) / cellSize;

    if (r < 0 || r >= game->rows || c < 0 || c >= game->cols)
        return false;

    if (msg.is_left())
    {
        if (game->isRevealed[r][c] && !game->isMine[r][c])
        {
            clock_t now = clock();
            if (r == lastClickR && c == lastClickC &&
                (now - lastClickTime) < CLOCKS_PER_SEC / 2)
            {
                hitMine = game->revealAdjacent(r, c);
                won = game->checkWin();
                lastClickTime = 0;
                lastClickR = -1;
                lastClickC = -1;
                lastActionR = r;
                lastActionC = c;
                lastActionType = 'C';
                return true;
            }
            lastClickTime = now;
            lastClickR = r;
            lastClickC = c;
            return false;
        }

        lastClickTime = 0;
        lastClickR = -1;
        lastClickC = -1;
        if (!game->isRevealed[r][c])
            hitMine = game->reveal(r, c);
        won = game->checkWin();
        lastActionR = r;
        lastActionC = c;
        lastActionType = 'R';
        return true;
    }

    if (msg.is_right())
    {
        lastClickTime = 0;
        lastClickR = -1;
        lastClickC = -1;
        game->toggleFlag(r, c);
        won = game->checkWin();
        lastActionR = r;
        lastActionC = c;
        lastActionType = 'F';
        return true;
    }

    return false;
}

void GameRender::showMessage(const char *text)
{
    int msgW = textwidth(text) + 40;
    int msgH = textheight(text) + 20;
    int msgX = (screenW - msgW) / 2;
    int msgY = (screenH - msgH) / 2;

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