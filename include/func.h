#ifndef FUNC_H
#define FUNC_H

#include <string>
#include "network.h"
#include "minesweeper.h"

extern const int SCR_W;
extern const int SCR_H;

bool parseInput(const std::string &input, int &r, int &c, char &action);

void drawConnectionMsg(const char *line1, const char *line2);

bool doAction(Minesweeper &game, char type, int r, int c,
              bool &hitMine, bool &won);

void sendOp(Network &net, char type, int r, int c);

#endif