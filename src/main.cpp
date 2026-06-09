#include "network.h"
#include <cstdio>
#include <vector>
#include "startscreen.h"
#include "gamerender.h"

using namespace std;

static const int SCR_W = 1000;
static const int SCR_H = 650;

static void drawConnectionMsg(const char *line1, const char *line2)
{
    cleardevice();
    setbkcolor(EGERGB(245, 222, 179));
    setfont(22, 0, "Arial");
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    outtextxy((SCR_W - textwidth(line1)) / 2, 280, line1);
    if (line2)
        outtextxy((SCR_W - textwidth(line2)) / 2, 315, line2);
}

static void runSinglePlayer(int rows, int cols, int mines)
{
    Minesweeper game(rows, cols, mines);
    GameRender render(&game, 30, 20, SCR_W, SCR_H);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false, exitRequested = false;

    while (!game.isGameOver())
    {
        if (mousemsg())
        {
            msg = getmouse();
            if (render.handleMouse(msg, hitMine, won, exitRequested))
            {
                render.render();
                if (exitRequested)
                    break;
                if (won)
                {
                    render.showMessage("You Win!");
                    while (mousemsg()) getmouse();
                    while (true)
                    {
                        if (mousemsg())
                        {
                            mouse_msg m = getmouse();
                            if (m.msg == mouse_msg_up) break;
                        }
                        delay_ms(10);
                    }
                    break;
                }
                if (hitMine)
                {
                    render.showMessage("Game Over!");
                    while (mousemsg()) getmouse();
                    while (true)
                    {
                        if (mousemsg())
                        {
                            mouse_msg m = getmouse();
                            if (m.msg == mouse_msg_up) break;
                        }
                        delay_ms(10);
                    }
                    break;
                }
            }
        }
        else
        {
            delay_ms(1);
        }
    }
}

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

static void sendOp(Network &net, char type, int r, int c)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%c %d %d", type, r, c);
    net.sendMsg(buf);
}

static void runHost(int rows, int cols, int mines)
{
    Network net;
    if (!net.host(12345))
    {
        drawConnectionMsg("Failed to host.", "Click to return...");
        while (!mousemsg()) delay_ms(10);
        getmouse();
        return;
    }

    drawConnectionMsg("Waiting for player...", nullptr);
    while (!net.isConnected())
    {
        net.hasData();
        delay_ms(50);
    }

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

    Minesweeper game(rows, cols, 0);
    for (size_t i = 0; i < mineRows.size(); ++i)
        game.setMine(mineRows[i], mineCols[i]);
    game.computeAdjacent();

    GameRender render(&game, 30, 20, SCR_W, SCR_H);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false, exitRequested = false;

    while (!game.isGameOver())
    {
        bool redraw = false;

        if (mousemsg())
        {
            msg = getmouse();
            bool localHit = false, localWon = false;
            if (render.handleMouse(msg, localHit, localWon, exitRequested))
            {
                if (exitRequested) break;
                int r = render.getLastR(), c = render.getLastC();
                char type = render.getLastActionType();
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
            while (mousemsg()) getmouse();
            while (true)
            {
                if (mousemsg())
                {
                    mouse_msg m = getmouse();
                    if (m.msg == mouse_msg_up) break;
                }
                delay_ms(10);
            }
            break;
        }
        if (hitMine)
        {
            render.showMessage("Game Over!");
            while (mousemsg()) getmouse();
            while (true)
            {
                if (mousemsg())
                {
                    mouse_msg m = getmouse();
                    if (m.msg == mouse_msg_up) break;
                }
                delay_ms(10);
            }
            break;
        }

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    net.disconnect();
}

static void runClient(const char *ip)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "Connecting to %s ...", ip);
    drawConnectionMsg(buf, nullptr);

    Network net;
    if (!net.join(ip, 12345))
    {
        drawConnectionMsg("Connection failed!", "Check IP / host not running. Click to return...");
        while (!mousemsg()) delay_ms(10);
        getmouse();
        return;
    }

    drawConnectionMsg("Connected! Syncing board...", nullptr);

    int rows = 9, cols = 9, mines = 10;
    Minesweeper *game = nullptr;

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

    GameRender render(game, 30, 20, SCR_W, SCR_H);
    render.init();
    render.render();
    delay_ms(10);

    while (mousemsg())
        getmouse();

    mouse_msg msg = {0};
    bool hitMine = false, won = false, exitRequested = false;

    while (!game->isGameOver())
    {
        bool redraw = false;

        if (mousemsg())
        {
            msg = getmouse();
            bool localHit = false, localWon = false;
            if (render.handleMouse(msg, localHit, localWon, exitRequested))
            {
                if (exitRequested) break;
                int r = render.getLastR(), c = render.getLastC();
                char type = render.getLastActionType();
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
            while (mousemsg()) getmouse();
            while (true)
            {
                if (mousemsg())
                {
                    mouse_msg m = getmouse();
                    if (m.msg == mouse_msg_up) break;
                }
                delay_ms(10);
            }
            break;
        }
        if (hitMine)
        {
            render.showMessage("Game Over!");
            while (mousemsg()) getmouse();
            while (true)
            {
                if (mousemsg())
                {
                    mouse_msg m = getmouse();
                    if (m.msg == mouse_msg_up) break;
                }
                delay_ms(10);
            }
            break;
        }

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    delete game;
    net.disconnect();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    initgraph(SCR_W, SCR_H);

    while (true)
    {
        StartScreen startScreen(SCR_W, SCR_H);
        startScreen.init();

        if (!startScreen.show())
        {
            break;
        }

        int rows, cols, mines;
        startScreen.getConfig(rows, cols, mines);
        int mode = startScreen.getMode();

        if (mode == 1)
            runHost(rows, cols, mines);
        else if (mode == 2)
            runClient(startScreen.getHostIP());
        else
            runSinglePlayer(rows, cols, mines);
    }

    closegraph();
    return 0;
}