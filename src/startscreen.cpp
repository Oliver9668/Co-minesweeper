#include "startscreen.h"
#include <windows.h>

StartScreen::StartScreen(int w, int h) : width(w), height(h), mode(0)
{
    Color btnNormal(255, 246, 210);
    Color btnHover(220, 210, 170);
    Color btnClick(200, 185, 145);

    int btnW = 240;
    int btnH = 50;
    int centerX = (width - btnW) / 2;

    easyBtn = new Button(centerX, 165, btnW, btnH, "Easy  (9 x 9 / 10)",
                         btnNormal, btnHover, btnClick);
    mediumBtn = new Button(centerX, 230, btnW, btnH, "Medium  (16 x 16 / 40)",
                           btnNormal, btnHover, btnClick);
    hardBtn = new Button(centerX, 295, btnW, btnH, "Hard  (16 x 30 / 99)",
                         btnNormal, btnHover, btnClick);

    Color hostColor(190, 210, 240);
    Color hostHover(160, 185, 220);
    Color hostClick(140, 165, 200);
    Color joinColor(190, 230, 190);
    Color joinHover(160, 210, 160);
    Color joinClick(140, 190, 140);

    hostBtn = new Button(centerX, 395, btnW, btnH, "Host Game",
                         hostColor, hostHover, hostClick);
    joinBtn = new Button(centerX, 460, btnW, btnH, "Join Game",
                         joinColor, joinHover, joinClick);
    exitBtn = new Button(centerX, 525, btnW, btnH, "Exit",
                         btnNormal, btnHover, btnClick);
}

StartScreen::~StartScreen()
{
    delete easyBtn;
    delete mediumBtn;
    delete hardBtn;
    delete hostBtn;
    delete joinBtn;
    delete exitBtn;
}

void StartScreen::init()
{
    SetConsoleOutputCP(CP_UTF8);
    setbkcolor(EGERGB(245, 222, 179));
    cleardevice();
}

void StartScreen::drawBackground()
{
    setbkcolor(EGERGB(245, 222, 179));
    cleardevice();
}

void StartScreen::drawTitle()
{
    settextcolor(EGERGB(70, 45, 25));
    setbkmode(TRANSPARENT);
    setfont(52, 0, "Arial Bold");
    const char *title = "Minesweeper";
    int tx = (width - textwidth(title)) / 2;
    outtextxy(tx, 40, title);

    setlinecolor(EGERGB(160, 130, 100));
    int lineY = 100;
    int lineW = 200;
    line((width - lineW) / 2, lineY, (width + lineW) / 2, lineY);
}

bool StartScreen::show()
{
    drawBackground();
    drawTitle();

    setfont(17, 0, "Arial");
    settextcolor(EGERGB(120, 90, 60));
    const char *singleLabel = "- Single Player -";
    int sx = (width - textwidth(singleLabel)) / 2;
    outtextxy(sx, 133, singleLabel);

    easyBtn->drawButton();
    mediumBtn->drawButton();
    hardBtn->drawButton();

    setfont(17, 0, "Arial");
    settextcolor(EGERGB(120, 90, 60));
    const char *multiLabel = "- Multiplayer -";
    int mx = (width - textwidth(multiLabel)) / 2;
    outtextxy(mx, 363, multiLabel);

    hostBtn->drawButton();
    joinBtn->drawButton();
    exitBtn->drawButton();

    mouse_msg msg = {0};

    while (true)
    {
        if (mousemsg())
        {
            msg = getmouse();

            if (easyBtn->buttonRender(msg))
            {
                rows = 9; cols = 9; mines = 10;
                mode = 0;
                return true;
            }
            if (mediumBtn->buttonRender(msg))
            {
                rows = 16; cols = 16; mines = 40;
                mode = 0;
                return true;
            }
            if (hardBtn->buttonRender(msg))
            {
                rows = 16; cols = 30; mines = 99;
                mode = 0;
                return true;
            }
            if (hostBtn->buttonRender(msg))
            {
                rows = 16; cols = 16; mines = 40;
                mode = 1;
                return true;
            }
            if (joinBtn->buttonRender(msg))
            {
                char ipBuf[64] = "127.0.0.1";
                inputbox_getline("Join Game", "Enter Host IP:", ipBuf, 63);
                hostIP = ipBuf;
                mode = 2;
                return true;
            }
            if (exitBtn->buttonRender(msg))
            {
                return false;
            }
        }
        else
        {
            delay_ms(1);
        }
    }
}

void StartScreen::getConfig(int &outRows, int &outCols, int &outMines)
{
    outRows = rows;
    outCols = cols;
    outMines = mines;
}

void StartScreen::close()
{
}