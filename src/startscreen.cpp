#include "startscreen.h"
#include <windows.h>

StartScreen::StartScreen(int w, int h) : width(w), height(h)
{
    Color bgColor(250, 223, 165);
    Color hoverColor(200, 200, 200);
    Color clickColor(230, 230, 230);

    int btnW = 200;
    int btnH = 50;
    int centerX = (width - btnW) / 2;

    easyBtn = new Button(centerX, 180, btnW, btnH, "Easy (9x9/10)",
                         bgColor, hoverColor, clickColor);
    mediumBtn = new Button(centerX, 260, btnW, btnH, "Medium (16x16/40)",
                           bgColor, hoverColor, clickColor);
    hardBtn = new Button(centerX, 340, btnW, btnH, "Hard (16x30/99)",
                         bgColor, hoverColor, clickColor);
    exitBtn = new Button(centerX, 440, btnW, btnH, "Exit",
                         bgColor, hoverColor, clickColor);
}

StartScreen::~StartScreen()
{
    delete easyBtn;
    delete mediumBtn;
    delete hardBtn;
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
    outtextxy(tx, 60, title);
}

bool StartScreen::show()
{
    drawBackground();
    drawTitle();
    easyBtn->drawButton();
    mediumBtn->drawButton();
    hardBtn->drawButton();
    exitBtn->drawButton();

    mouse_msg msg = {0};

    while (true)
    {
        if (mousemsg())
        {
            msg = getmouse();

            if (easyBtn->buttonRender(msg))
            {
                rows = 9;
                cols = 9;
                mines = 10;
                return true;
            }
            if (mediumBtn->buttonRender(msg))
            {
                rows = 16;
                cols = 16;
                mines = 40;
                return true;
            }
            if (hardBtn->buttonRender(msg))
            {
                rows = 16;
                cols = 30;
                mines = 99;
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