#ifndef GAMERENDER_H
#define GAMERENDER_H

#include <graphics.h>
#include "minesweeper.h"

class GameRender
{
private:
    Minesweeper *game;
    int cellSize;
    int margin;
    int infoBarHeight;
    int windowW;
    int windowH;

    COLORREF bgColor;
    COLORREF cellColor;
    COLORREF revealedColor;
    COLORREF flagColor;
    COLORREF mineColor;
    COLORREF borderColor;
    COLORREF numberColors[8];

    void drawCell(int r, int c);
    void drawBoard();
    void drawInfoBar();

public:
    GameRender(Minesweeper *g, int cell = 30, int mar = 20);
    ~GameRender();

    void init();
    void render();
    bool handleMouse(mouse_msg msg, bool &hitMine, bool &won);
    void showMessage(const char *text);

    int getWindowWidth() const { return windowW; }
    int getWindowHeight() const { return windowH; }
};

#endif