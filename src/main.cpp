#include <windows.h>
#include "startscreen.h"
#include "gamesystem.h"

using namespace std;

int main()
{
    StartScreen startScreen;
    startScreen.init();

    if (!startScreen.show())
    {
        startScreen.close();
        return 1;
    }

    int rows, cols, mines;
    startScreen.getConfig(rows, cols, mines);
    startScreen.close();

    GameSystem Game;
    if (!Game.start(rows, cols, mines))
        return 1;
    Game.run();
    Game.end();
    return 0;
}