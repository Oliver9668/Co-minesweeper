#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "button.h"
#include <graphics.h>
#include <string>

class StartScreen
{
private:
    int width;
    int height;
    Button *easyBtn;
    Button *mediumBtn;
    Button *hardBtn;
    Button *hostBtn;
    Button *joinBtn;
    Button *exitBtn;
    int rows, cols, mines;

    // 联机模式: 0=单人, 1=主机, 2=加入
    int mode;
    std::string hostIP;

    void drawBackground();
    void drawTitle();

public:
    StartScreen(int w = 800, int h = 600);
    ~StartScreen();

    void init();

    bool show();

    void getConfig(int &outRows, int &outCols, int &outMines);

    int getMode() const { return mode; }
    const char *getHostIP() const { return hostIP.c_str(); }

    void close();
};

#endif