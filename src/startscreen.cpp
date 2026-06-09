#include "startscreen.h"
#include "inputdialog.h"
#include <cstdio>
#include <cstdlib>
#include <windows.h>

StartScreen::StartScreen(int w, int h)
    : width(w), height(h), currentPage(MENU_MAIN),
      rows(9), cols(9), mines(10), mode(0)
{
    Color btnNormal(255, 246, 210);
    Color btnHover(220, 210, 170);
    Color btnClick(200, 185, 145);

    Color backColor(210, 210, 210);
    Color backHover(180, 180, 180);
    Color backClick(155, 155, 155);

    int mainBtnW = 280, mainBtnH = 60;
    int mainCX = (width - mainBtnW) / 2;

    singleBtn = new Button(mainCX, 270, mainBtnW, mainBtnH, "Single Player",
                           btnNormal, btnHover, btnClick);
    multiBtn  = new Button(mainCX, 370, mainBtnW, mainBtnH, "Multiplayer",
                           btnNormal, btnHover, btnClick);
    exitBtn   = new Button(mainCX, 470, mainBtnW, mainBtnH, "Exit",
                           btnNormal, btnHover, btnClick);

    int subBtnW = 240, subBtnH = 50;
    int subCX = (width - subBtnW) / 2;

    easyBtn   = new Button(subCX, 210, subBtnW, subBtnH, "Easy  (9 x 9 / 10)",
                           btnNormal, btnHover, btnClick);
    mediumBtn = new Button(subCX, 280, subBtnW, subBtnH, "Medium  (16 x 16 / 40)",
                           btnNormal, btnHover, btnClick);
    hardBtn   = new Button(subCX, 350, subBtnW, subBtnH, "Hard  (16 x 30 / 99)",
                           btnNormal, btnHover, btnClick);
    customBtn = new Button(subCX, 420, subBtnW, subBtnH, "Custom",
                           btnNormal, btnHover, btnClick);
    backBtn1  = new Button(subCX, 490, subBtnW, subBtnH, "Back",
                           backColor, backHover, backClick);

    Color hostColor(190, 210, 240);
    Color hostHover(160, 185, 220);
    Color hostClick(140, 165, 200);
    Color joinColor(190, 230, 190);
    Color joinHover(160, 210, 160);
    Color joinClick(140, 190, 140);

    hostBtn  = new Button(subCX, 280, subBtnW, subBtnH, "Host Game",
                          hostColor, hostHover, hostClick);
    joinBtn  = new Button(subCX, 350, subBtnW, subBtnH, "Join Game",
                          joinColor, joinHover, joinClick);
    backBtn2 = new Button(subCX, 490, subBtnW, subBtnH, "Back",
                          backColor, backHover, backClick);
}

StartScreen::~StartScreen()
{
    delete singleBtn;
    delete multiBtn;
    delete exitBtn;
    delete easyBtn;
    delete mediumBtn;
    delete hardBtn;
    delete customBtn;
    delete backBtn1;
    delete hostBtn;
    delete joinBtn;
    delete backBtn2;
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
    setfont(96, 0, "Arial Bold");
    const char *title = "Minesweeper";
    int tx = (width - textwidth(title)) / 2;
    outtextxy(tx, 60, title);

    setlinecolor(EGERGB(160, 130, 100));
    int lineY = 160;
    int lineW = 350;
    line((width - lineW) / 2, lineY, (width + lineW) / 2, lineY);
}

void StartScreen::drawMainMenu()
{
    drawBackground();
    drawTitle();
    singleBtn->drawButton();
    multiBtn->drawButton();
    exitBtn->drawButton();
}

void StartScreen::drawSingleMenu()
{
    drawBackground();
    drawTitle();

    setfont(17, 0, "Arial");
    settextcolor(EGERGB(120, 90, 60));
    const char *label = "- Single Player -";
    int sx = (width - textwidth(label)) / 2;
    outtextxy(sx, 180, label);
    
    easyBtn->drawButton();
    mediumBtn->drawButton();
    hardBtn->drawButton();
    customBtn->drawButton();
    backBtn1->drawButton();
}

void StartScreen::drawMultiMenu()
{
    drawBackground();
    drawTitle();

    setfont(17, 0, "Arial");
    settextcolor(EGERGB(120, 90, 60));
    const char *label = "- Multiplayer -";
    int mx = (width - textwidth(label)) / 2;
    outtextxy(mx, 180, label);

    hostBtn->drawButton();
    joinBtn->drawButton();
    backBtn2->drawButton();
}

bool StartScreen::promptCustomDifficulty()
{
    char bufR[8], bufC[8], bufM[8];
    snprintf(bufR, sizeof(bufR), "%d", rows);
    snprintf(bufC, sizeof(bufC), "%d", cols);
    snprintf(bufM, sizeof(bufM), "%d", mines);

    InputField fields[3] = {
        { "Rows:",  bufR, 8, (int)strlen(bufR), true },
        { "Cols:",  bufC, 8, (int)strlen(bufC), true },
        { "Mines:", bufM, 8, (int)strlen(bufM), true },
    };

    if (!showInputDialog("Custom Difficulty", fields, 3, width, height))
        return false;

    int newR = atoi(bufR);
    int newC = atoi(bufC);
    int newM = atoi(bufM);
    if (newR >= 5 && newR <= 30) rows = newR;
    if (newC >= 5 && newC <= 50) cols = newC;
    if (newM > 0 && newM < rows * cols) mines = newM;
    return true;
}

bool StartScreen::show()
{
    currentPage = MENU_MAIN;
    drawMainMenu();

    mouse_msg msg = {0};

    while (true)
    {
        if (mousemsg())
        {
            msg = getmouse();

            switch (currentPage)
            {
            case MENU_MAIN:
                if (exitBtn->buttonRender(msg))
                    return false;
                if (singleBtn->buttonRender(msg))
                {
                    currentPage = MENU_SINGLE;
                    drawSingleMenu();
                }
                else if (multiBtn->buttonRender(msg))
                {
                    currentPage = MENU_MULTI;
                    drawMultiMenu();
                }
                break;

            case MENU_SINGLE:
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
                if (customBtn->buttonRender(msg))
                {
                    rows = 16; cols = 16; mines = 40;
                    if (!promptCustomDifficulty())
                    {
                        drawSingleMenu();
                        break;
                    }
                    mode = 0;
                    return true;
                }
                if (backBtn1->buttonRender(msg))
                {
                    currentPage = MENU_MAIN;
                    drawMainMenu();
                }
                break;

            case MENU_MULTI:
                if (hostBtn->buttonRender(msg))
                {
                    rows = 16; cols = 16; mines = 40;
                    if (!promptCustomDifficulty())
                    {
                        drawMultiMenu();
                        break;
                    }
                    mode = 1;
                    return true;
                }
                if (joinBtn->buttonRender(msg))
                {
                    char ipBuf[64] = "127.0.0.1";
                    InputField ipField = { "Host IP:", ipBuf, 64,
                                           (int)strlen(ipBuf), false };
                    if (showInputDialog("Join Game", &ipField, 1, width, height))
                    {
                        hostIP = ipBuf;
                        mode = 2;
                        return true;
                    }
                    drawMultiMenu();
                }
                if (backBtn2->buttonRender(msg))
                {
                    currentPage = MENU_MAIN;
                    drawMainMenu();
                }
                break;
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