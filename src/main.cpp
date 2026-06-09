#include <windows.h>
#include "gamesystem.h"

using namespace std;

int main()
{
    GameSystem Game;
    if (!Game.start())
        return 1;
    Game.run();
    Game.end();
    return 0;
}