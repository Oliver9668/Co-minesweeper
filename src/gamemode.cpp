#include "gamemode.h"
#include "func.h"
#include "gamerender.h"
#include "network.h"
#include <cstdio>
#include <vector>

using namespace std;

void runSinglePlayer(int rows, int cols, int mines)
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
            if (render.handleScroll(msg))
            {
                render.render();
                continue;
            }
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
                    game.revealAll();
                    render.render();
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

void runHost(int rows, int cols, int mines)
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

    Minesweeper game(rows, cols, mines);
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
            if (render.handleScroll(msg))
            {
                redraw = true;
            }
            else
            {
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
                    }
                    if (localWon)
                    {
                        won = true;
                        net.sendMsg("WIN");
                    }
                }
            }
        }

        while (net.hasData())
        {
            string line = net.recvMsg();
            if (line.empty())
                break;

            if (line == "WIN")   { won = true; }
            else
            {
                char type; int r, c;
                sscanf(line.c_str(), "%c %d %d", &type, &r, &c);
                bool rh = false, rw = false;
                doAction(game, type, r, c, rh, rw);
                if (rh) { hitMine = true; }
                if (rw) { won = true; }
            }
            redraw = true;
        }

        if (redraw || hitMine)
            render.render();

        if (hitMine)
        {
            delay_ms(2000);
            game.hideAllMines();
            hitMine = false;
            render.render();
        }

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

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    net.disconnect();
}

void runClient(const char *ip)
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
                game->setTotalMines(mines);
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
            if (render.handleScroll(msg))
            {
                redraw = true;
            }
            else
            {
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
                    }
                    if (localWon)
                    {
                        won = true;
                        net.sendMsg("WIN");
                    }
                }
            }
        }

        while (net.hasData())
        {
            string line = net.recvMsg();
            if (line.empty())
                break;

            if (line == "WIN")   { won = true; }
            else
            {
                char type; int r, c;
                sscanf(line.c_str(), "%c %d %d", &type, &r, &c);
                bool rh = false, rw = false;
                doAction(*game, type, r, c, rh, rw);
                if (rh) { hitMine = true; }
                if (rw) { won = true; }
            }
            redraw = true;
        }

        if (redraw || hitMine)
            render.render();

        if (hitMine)
        {
            delay_ms(2000);
            game->hideAllMines();
            hitMine = false;
            render.render();
        }

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

        if (!mousemsg() && !net.hasData())
            delay_ms(1);
    }

    delete game;
    net.disconnect();
}