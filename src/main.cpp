#include "func.h"
#include "gamemode.h"
#include "startscreen.h"
#include <graphics.h>
#include <windows.h>

using namespace std;

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