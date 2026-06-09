#include "func.h"
#include <sstream>

using namespace std;

// 解析形如 "3,5 r" 的输入
bool parseInput(const string &input, int &r, int &c, char &action)
{
    stringstream ss(input);
    string coord, act;
    if (!(ss >> coord >> act))
        return false;
    // 从坐标中提取行和列，格式为 "row,col"
    size_t comma = coord.find(',');
    if (comma == string::npos)
        return false;
    try
    {
        r = stoi(coord.substr(0, comma));
        c = stoi(coord.substr(comma + 1));
    }
    catch (...)
    {
        return false;
    }
    if (act.size() != 1 || (act[0] != 'r' && act[0] != 'f'))
        return false;
    action = act[0];
    return true;
}