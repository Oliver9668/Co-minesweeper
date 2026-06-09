#include "startscreen.h"
#include <windows.h>

StartScreen::StartScreen(int w, int h) : width(w), height(h), mode(0)
{
    Color bgColor(250, 223, 165);
    Color hoverColor(200, 200, 200);
    Color clickColor(230, 230, 230);

    int btnW = 200;
    int btnH = 50;
    int centerX = (width - btnW) / 2;

    easyBtn = new Button(centerX, 160, btnW, btnH, "Easy (9x9/10)",
                         bgColor, hoverColor, clickColor);
    mediumBtn = new Button(centerX, 230, btnW, btnH, "Medium (16x16/40)",
                           bgColor, hoverColor, clickColor);
    hardBtn = new Button(centerX, 300, btnW, btnH, "Hard (16x30/99)",
                         bgColor, hoverColor, clickColor);

    Color hostColor(180, 220, 255);
    Color joinColor(180, 255, 180);
    hostBtn = new Button(centerX, 380, btnW, btnH, "Host Game",
                         hostColor, Color(150, 200, 235), Color(210, 230, 255));
    joinBtn = new Button(centerX, 450, btnW, btnH, "Join Game",
                         joinColor, Color(150, 235, 150), Color(210, 255, 210));

    exitBtn = new Button(centerX, 530, btnW, btnH, "Exit",
                         bgColor, hoverColor, clickColor);
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
    initgraph(width, height);
    setbkcolor(EGERGB(245, 222, 179));
    cleardevice();
}

void StartScreen::drawBackground()
{
    cleardevice();
}

void StartScreen::drawTitle()
{
    setfillcolor(EGERGB(245, 222, 179));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    setfont(48, 0, "Arial");
    const char *title = "Minesweeper";
    int tx = (width - textwidth(title)) / 2;
    outtextxy(tx, 50, title);
}

bool StartScreen::show()
{
    drawBackground();
    drawTitle();

    setfont(18, 0, "Arial");
    const char *singleLabel = "-- Single Player --";
    int sx = (width - textwidth(singleLabel)) / 2;
    outtextxy(sx, 120, singleLabel);

    easyBtn->drawButton();
    mediumBtn->drawButton();
    hardBtn->drawButton();

    const char *multiLabel = "-- Multiplayer --";
    int mx = (width - textwidth(multiLabel)) / 2;
    outtextxy(mx, 355, multiLabel);

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
                // 弹出输入框让玩家输入主机 IP
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
    closegraph();
}