#ifndef GAMEMODE_H
#define GAMEMODE_H

void runSinglePlayer(int rows, int cols, int mines);
void runHost(int rows, int cols, int mines);
void runClient(const char *ip);

#endif