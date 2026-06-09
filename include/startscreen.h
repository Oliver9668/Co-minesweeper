#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "button.h"
#include <graphics.h>

class StartScreen
{
private:
    int width;
    int height;
    Button *easyBtn;
    Button *mediumBtn;
    Button *hardBtn;
    Button *exitBtn;
    int rows, cols, mines;

    void drawBackground();
    void drawTitle();

public:
    StartScreen(int w = 800, int h = 600);
    ~StartScreen();

    void init();

    bool show();

    void getConfig(int &outRows, int &outCols, int &outMines);

    void close();
};

#endif