#ifndef GAMERENDER_H
#define GAMERENDER_H

#include <graphics.h>
#include <ctime>
#include "minesweeper.h"

class GameRender
{
private:
    Minesweeper *game;
    int cellSize;
    int margin;
    int infoBarHeight;
    int toolbarHeight;
    int gridY;
    int windowW;
    int windowH;
    int screenW;
    int screenH;
    int offsetX;
    int offsetY;

    COLORREF bgColor;
    COLORREF cellColor;
    COLORREF cellHighlight;
    COLORREF cellShadow;
    COLORREF revealedColor;
    COLORREF flagColor;
    COLORREF mineColor;
    COLORREF borderColor;
    COLORREF numberColors[8];

    // 工具栏退出按钮区域
    int exitBtnX, exitBtnY, exitBtnW, exitBtnH;

    // 双击检测
    clock_t lastClickTime;
    int lastClickR;
    int lastClickC;

    // 上次操作位置
    int lastActionR;
    int lastActionC;

    // 上次操作类型: 'R'=翻开 'C'=和弦 'F'=插旗
    char lastActionType;

    void drawCell(int r, int c);
    void drawBoard();
    void drawToolbar();
    void drawInfoBar();

public:
    GameRender(Minesweeper *g, int cell = 30, int mar = 20, int scrW = 1000, int scrH = 650);
    ~GameRender();

    void init();
    void render();
    bool handleMouse(mouse_msg msg, bool &hitMine, bool &won, bool &exitRequested);
    void showMessage(const char *text);

    int getWindowWidth() const { return windowW; }
    int getWindowHeight() const { return windowH; }
    int getLastR() const { return lastActionR; }
    int getLastC() const { return lastActionC; }
    char getLastActionType() const { return lastActionType; }
};

#endif