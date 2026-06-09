#include "network.h"
#include <cstdio>
#include <vector>
#include "startscreen.h"
#include "gamerender.h"

using namespace std;

static void runSinglePlayer(int rows, int cols, int mines)
{
    Minesweeper game(rows, cols, mines);
    GameRender render(&game, 30, 20);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false;

    while (!game.isGameOver())
    {
        if (mousemsg())
        {
            msg = getmouse();
            if (render.handleMouse(msg, hitMine, won))
            {
                render.render();
                if (won)
                {
                    render.showMessage("You Win!");
                    delay_ms(2000);
                    break;
                }
                if (hitMine)
                {
                    game.revealAll();
                    render.render();
                    render.showMessage("Game Over!");
                    delay_ms(2000);
                    break;
                }
            }
        }
        else
        {
            delay_ms(1);
        }
    }
    closegraph();
}

// 获取本地操作类型
static char getActionType(mouse_msg msg, Minesweeper &game, int r, int c)
{
    if (msg.is_right())
        return 'F';
    if (game.cellRevealed(r, c) && !game.cellIsMine(r, c))
        return 'C';
    return 'R';
}

// 执行操作并检测结果
static bool doAction(Minesweeper &game, char type, int r, int c,
                      bool &hitMine, bool &won)
{
    hitMine = false;
    won = false;
    if (type == 'R')
    {
        hitMine = game.reveal(r, c);
        won = game.checkWin();
    }
    else if (type == 'F')
    {
        game.toggleFlag(r, c);
        won = game.checkWin();
    }
    else if (type == 'C')
    {
        hitMine = game.revealAdjacent(r, c);
        won = game.checkWin();
    }
    return true;
}

// 发送操作
static void sendOp(Network &net, char type, int r, int c)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%c %d %d", type, r, c);
    net.sendMsg(buf);
}

// 联机主机
static void runHost(int rows, int cols, int mines)
{
    Network net;
    if (!net.host(12345))
    {
        closegraph();
        return;
    }

    // 等待客机
    {
        initgraph(400, 200);
        setbkcolor(EGERGB(245, 222, 179));
        setfont(20, 0, "Arial");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        const char *msg = "Waiting for player...";
        outtextxy((400 - textwidth(msg)) / 2, 80, msg);
        while (!net.isConnected())
        {
            net.hasData();
            delay_ms(50);
        }
        closegraph();
    }

    // 发送棋盘配置和雷位
    vector<int> mineRows, mineCols;
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "BOARD %d %d %d", rows, cols, mines);
        net.sendMsg(buf);

        Minesweeper tmpGame(rows, cols, mines);
        tmpGame.reveal(0, 0);
        tmpGame.getMinePositions(mineRows, mineCols);

        for (size_t i = 0; i < mineRows.size(); ++i)
        {
            snprintf(buf, sizeof(buf), "MINE %d %d", mineRows[i], mineCols[i]);
            net.sendMsg(buf);
        }
        net.sendMsg("END");
    }

    // 用相同雷位重建棋盘
    Minesweeper game(rows, cols, 0);
    for (size_t i = 0; i < mineRows.size(); ++i)
        game.setMine(mineRows[i], mineCols[i]);
    game.computeAdjacent();

    GameRender render(&game, 30, 20);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false;

    while (!game.isGameOver())
    {
        bool redraw = false;

        // 本地操作
        if (mousemsg())
        {
            msg = getmouse();
            bool localHit = false, localWon = false;
            if (render.handleMouse(msg, localHit, localWon))
            {
                int r = render.getLastR(), c = render.getLastC();
                char type = getActionType(msg, game, r, c);
                sendOp(net, type, r, c);
                redraw = true;
                if (localHit)
                {
                    hitMine = true;
                    game.revealAll();
                    net.sendMsg("LOSE");
                }
                if (localWon)
                {
                    won = true;
                    net.sendMsg("WIN");
                }
            }
        }

        // 远程操作
        while (net.hasData())
        {
            string line = net.recvMsg();
            if (line.empty())
                break;

            if (line == "WIN")   { won = true; }
            else if (line == "LOSE")
            {
                game.revealAll();
                hitMine = true;
            }
            else
            {
                char type; int r, c;
                sscanf(line.c_str(), "%c %d %d", &type, &r, &c);
                bool rh = false, rw = false;
                doAction(game, type, r, c, rh, rw);
                if (rh) { game.revealAll(); hitMine = true; }
                if (rw) { won = true; }
            }
            redraw = true;
        }

        if (redraw)
            render.render();

        if (won)
        {
            render.showMessage("You Win!");
            delay_ms(2000);
            break;
        }
        if (hitMine)
        {
            render.showMessage("Game Over!");
            delay_ms(2000);
            break;
        }

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    net.disconnect();
    closegraph();
}

// 联机客机
static void runClient(const char *ip)
{
    // 连接界面
    initgraph(500, 300);
    setbkcolor(EGERGB(245, 222, 179));
    setfont(20, 0, "Arial");
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);

    int cy = 60;
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "Connecting to %s ...", ip);
        outtextxy((500 - textwidth(buf)) / 2, cy, buf);
    }

    Network net;
    if (!net.join(ip, 12345))
    {
        settextcolor(EGERGB(200, 0, 0));
        const char *err = "Connection failed! Check IP / host not running.";
        outtextxy((500 - textwidth(err)) / 2, 150, err);
        delay_ms(3000);
        closegraph();
        return;
    }

    cleardevice();
    settextcolor(BLACK);
    {
        const char *ok = "Connected! Syncing board...";
        outtextxy((500 - textwidth(ok)) / 2, cy, ok);
    }

    int rows = 9, cols = 9, mines = 10;
    Minesweeper *game = nullptr;

    // 接收初始数据
    {
        while (true)
        {
            while (!net.hasData())
                delay_ms(10);
            string line = net.recvMsg();
            if (line.empty())
                continue;

            if (line.substr(0, 5) == "BOARD")
            {
                sscanf(line.c_str(), "BOARD %d %d %d", &rows, &cols, &mines);
                game = new Minesweeper(rows, cols, 0);
            }
            else if (line.substr(0, 4) == "MINE" && game)
            {
                int r, c;
                sscanf(line.c_str(), "MINE %d %d", &r, &c);
                game->setMine(r, c);
            }
            else if (line == "END" && game)
            {
                game->computeAdjacent();
                break;
            }
        }
    }

    closegraph();
    // 进入游戏

    GameRender render(game, 30, 20);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false;

    while (!game->isGameOver())
    {
        bool redraw = false;

        // 本地操作
        if (mousemsg())
        {
            msg = getmouse();
            bool localHit = false, localWon = false;
            if (render.handleMouse(msg, localHit, localWon))
            {
                int r = render.getLastR(), c = render.getLastC();
                char type = getActionType(msg, *game, r, c);
                sendOp(net, type, r, c);
                redraw = true;
                if (localHit)
                {
                    hitMine = true;
                    game->revealAll();
                    net.sendMsg("LOSE");
                }
                if (localWon)
                {
                    won = true;
                    net.sendMsg("WIN");
                }
            }
        }

        // 远程操作
        while (net.hasData())
        {
            string line = net.recvMsg();
            if (line.empty())
                break;

            if (line == "WIN")   { won = true; }
            else if (line == "LOSE")
            {
                game->revealAll();
                hitMine = true;
            }
            else
            {
                char type; int r, c;
                sscanf(line.c_str(), "%c %d %d", &type, &r, &c);
                bool rh = false, rw = false;
                doAction(*game, type, r, c, rh, rw);
                if (rh) { game->revealAll(); hitMine = true; }
                if (rw) { won = true; }
            }
            redraw = true;
        }

        if (redraw)
            render.render();

        if (won)
        {
            render.showMessage("You Win!");
            delay_ms(2000);
            break;
        }
        if (hitMine)
        {
            render.showMessage("Game Over!");
            delay_ms(2000);
            break;
        }

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    delete game;
    net.disconnect();
    closegraph();
}

int main()
{
    StartScreen startScreen;
    startScreen.init();

    if (!startScreen.show())
    {
        startScreen.close();
        return 0;
    }

    int rows, cols, mines;
    startScreen.getConfig(rows, cols, mines);
    int mode = startScreen.getMode();
    startScreen.close();

    if (mode == 1)
        runHost(rows, cols, mines);
    else if (mode == 2)
        runClient(startScreen.getHostIP());
    else
        runSinglePlayer(rows, cols, mines);

    return 0;
}