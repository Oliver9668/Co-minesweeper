// button.cpp
#include "button.h"

Button::Button(int x, int y, int width, int height, const char *text,
               const Color &normalColor, const Color &inAreaColor, const Color &clickColor)
    : x(x), y(y), width(width), height(height), normalColor(normalColor),
      inAreaColor(inAreaColor), clickColor(clickColor)
{
    strcpy(this->text, text);
}

void Button::setMouseMsg(mouse_msg m)
{
    mouseMsg = m;
}

bool Button::inArea()
{
    return (mouseMsg.x >= x && mouseMsg.x <= x + width && mouseMsg.y >= y && mouseMsg.y <= y + height);
}

void Button::drawButton()
{
    // 设置按键颜色
    if (downInArea == true)
        setfillcolor(clickColor.toEGEColor());  // 鼠标在按键区域且左键点击
    else if (inArea())
        setfillcolor(inAreaColor.toEGEColor()); // 鼠标在按键区域
    else
        setfillcolor(normalColor.toEGEColor()); // 鼠标不在按键区域
    
    // 绘制按键图形文字
    setlinecolor(BLACK);
    fillroundrect(x, y, x + width, y + height, 5, 5);
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    setfont(20, 0, "Times New Roman");
    int hSpace = (width - textwidth(text)) / 2;
    int vSpace = (height - textheight(text)) / 2;
    outtextxy(x + hSpace, y + vSpace, text);
}

bool Button::buttonRender(mouse_msg m)
{
    setMouseMsg(m);
    drawButton();

    // 保证点击时在按键区域
    if (mouseMsg.msg == mouse_msg_down && !mouseDown)
    {
        mouseDown = true;
        if (inArea())
            downInArea = true;
    }
    bool callBack = inArea() && mouseMsg.msg == mouse_msg_up && downInArea;
    if (mouseMsg.msg == mouse_msg_up){
        mouseDown = false;
        downInArea = false;
    }
    return callBack;
}