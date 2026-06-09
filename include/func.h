#ifndef FUNC_H
#define FUNC_H

#include <string>

using namespace std;

// 解析形如 "3,5 r" 的输入
bool parseInput(const string &input, int &r, int &c, char &action);

#endif