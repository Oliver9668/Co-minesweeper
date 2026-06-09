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

    enum MenuPage { MENU_MAIN, MENU_SINGLE, MENU_MULTI };
    MenuPage currentPage;

    // 主菜单按钮
    Button *singleBtn;
    Button *multiBtn;
    Button *exitBtn;

    // 单人子菜单按钮
    Button *easyBtn;
    Button *mediumBtn;
    Button *hardBtn;
    Button *customBtn;
    Button *backBtn1;

    // 多人子菜单按钮
    Button *hostBtn;
    Button *joinBtn;
    Button *backBtn2;

    int rows, cols, mines;

    // 联机模式: 0=单人, 1=主机, 2=加入
    int mode;
    std::string hostIP;

    void drawBackground();
    void drawTitle();
    void drawMainMenu();
    void drawSingleMenu();
    void drawMultiMenu();
    bool promptCustomDifficulty();

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