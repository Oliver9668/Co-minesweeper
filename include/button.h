// button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <graphics.h>

struct Color {
    int r, g, b;
    
    Color(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {}
    
    int toEGEColor() const {
        return EGERGB(r, g, b);
    }
};

class Button
{
private:
    int x, y, width, height;
    mouse_msg mouseMsg;
    Color normalColor; // 250, 223, 165
    Color inAreaColor; // 200, 200, 200
    Color clickColor; // 230, 230, 230
    char text[100];
    bool mouseDown = false;
    bool downInArea = false;

public:
    Button(int x, int y, int width, int height, const char *text,
           const Color &normalColor, const Color &inAreaColor, const Color &clickColor);
    void setMouseMsg(mouse_msg m);
    bool inArea();
    void drawButton();
    bool buttonRender(mouse_msg m);
};

#endif