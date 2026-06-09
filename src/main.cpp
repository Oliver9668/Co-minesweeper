#include <windows.h>
#include "startscreen.h"
#include "gamerender.h"

using namespace std;

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
    startScreen.close();

    Minesweeper game(rows, cols, mines);
    GameRender render(&game, 30, 20);
    render.init();
    render.render();
    delay_ms(10);

    mouse_msg msg = {0};
    bool hitMine = false;
    bool won = false;

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
    return 0;
}