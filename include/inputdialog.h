#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <graphics.h>
#include <cstring>
#include <cstdio>

struct InputField
{
    const char *label;
    char *buffer;
    int bufSize;
    int textLen;
    bool numericOnly;
    char origBuffer[256];
    bool dirty;
};

typedef void (*FieldChangeCallback)(InputField *fields, int count, int changedField);

inline bool showInputDialog(const char *title, InputField *fields, int count,
                            int screenW, int screenH,
                            FieldChangeCallback onFieldChange = nullptr)
{
    int dlgW = 340;
    int dlgH = 140 + count * 50;
    int dlgX = (screenW - dlgW) / 2;
    int dlgY = (screenH - dlgH) / 2;

    int fieldX = dlgX + 120, fieldW = 170, fieldH = 30;
    int labelX = dlgX + 30;
    int activeField = 0;

    int okX = dlgX + 60, okY = dlgY + 65 + count * 50 + 10, okW = 80, okH = 32;
    int cancelX = dlgX + 200, cancelY = okY, cancelW = okW, cancelH = okH;

    for (int i = 0; i < count; ++i)
    {
        strncpy(fields[i].origBuffer, fields[i].buffer, 255);
        fields[i].origBuffer[255] = '\0';
        fields[i].dirty = false;
    }

    auto restoreField = [&](int i) {
        if (!fields[i].dirty && fields[i].textLen == 0)
        {
            strncpy(fields[i].buffer, fields[i].origBuffer, fields[i].bufSize - 1);
            fields[i].buffer[fields[i].bufSize - 1] = '\0';
            fields[i].textLen = (int)strlen(fields[i].buffer);
        }
    };

    auto activateField = [&](int i) {
        restoreField(activeField);
        if (fields[i].textLen > 0)
        {
            strncpy(fields[i].origBuffer, fields[i].buffer, 255);
            fields[i].origBuffer[255] = '\0';
        }
        fields[i].buffer[0] = '\0';
        fields[i].textLen = 0;
        fields[i].dirty = false;
        activeField = i;
    };

    auto drawDialog = [&]() {
        setfillcolor(EGERGB(255, 250, 240));
        setlinecolor(EGERGB(100, 80, 60));
        bar(dlgX, dlgY, dlgX + dlgW, dlgY + dlgH);
        rectangle(dlgX, dlgY, dlgX + dlgW, dlgY + dlgH);

        settextcolor(EGERGB(70, 45, 25));
        setbkmode(TRANSPARENT);
        setfont(20, 0, "Arial Bold");
        int tx = dlgX + (dlgW - textwidth(title)) / 2;
        outtextxy(tx, dlgY + 25, title);

        setfont(18, 0, "Arial");
        settextcolor(EGERGB(80, 60, 40));
        for (int i = 0; i < count; ++i)
            outtextxy(labelX, dlgY + 65 + i * 50 + 5, fields[i].label);

        for (int i = 0; i < count; ++i)
        {
            setfillcolor(WHITE);
            setlinecolor(i == activeField
                ? EGERGB(80, 130, 200) : EGERGB(150, 140, 120));
            int fy = dlgY + 65 + i * 50;
            bar(fieldX, fy, fieldX + fieldW, fy + fieldH);
            rectangle(fieldX, fy, fieldX + fieldW, fy + fieldH);
            settextcolor(fields[i].dirty ? BLACK : EGERGB(180, 170, 160));
            outtextxy(fieldX + 6, fy + 5, fields[i].buffer);
        }

        setfillcolor(EGERGB(180, 220, 180));
        setlinecolor(EGERGB(100, 160, 100));
        bar(okX, okY, okX + okW, okY + okH);
        rectangle(okX, okY, okX + okW, okY + okH);
        settextcolor(BLACK);
        const char *okText = "OK";
        outtextxy(okX + (okW - textwidth(okText)) / 2, okY + 5, okText);

        setfillcolor(EGERGB(210, 210, 210));
        setlinecolor(EGERGB(150, 150, 150));
        bar(cancelX, cancelY, cancelX + cancelW, cancelY + cancelH);
        rectangle(cancelX, cancelY, cancelX + cancelW, cancelY + cancelH);
        const char *cancelText = "Cancel";
        outtextxy(cancelX + (cancelW - textwidth(cancelText)) / 2,
                  cancelY + 5, cancelText);
    };

    drawDialog();

    bool shouldApply = false;
    while (!shouldApply)
    {
        while (kbmsg())
        {
            key_msg k = getkey();
            InputField *f = (activeField >= 0 && activeField < count)
                ? &fields[activeField] : nullptr;

            if (f && k.msg == key_msg_char)
            {
                char ch = (char)k.key;
                bool valid = f->numericOnly
                    ? (ch >= '0' && ch <= '9')
                    : ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') ||
                       (ch >= 'A' && ch <= 'Z') || ch == '.' || ch == '-');
                if (valid && f->textLen < f->bufSize - 1)
                {
                    f->dirty = true;
                    f->buffer[f->textLen] = ch;
                    f->textLen++;
                    f->buffer[f->textLen] = '\0';
                    if (onFieldChange)
                        onFieldChange(fields, count, activeField);
                    drawDialog();
                }
            }
            else if (k.msg == key_msg_down)
            {
                if (k.key == key_back && f && f->textLen > 0)
                {
                    f->textLen--;
                    f->buffer[f->textLen] = '\0';
                    if (f->textLen == 0)
                        f->dirty = false;
                    if (onFieldChange)
                        onFieldChange(fields, count, activeField);
                    drawDialog();
                }
                else if (k.key == key_tab)
                {
                    int next = (activeField + 1) % count;
                    activateField(next);
                    drawDialog();
                }
                else if (k.key == key_enter)
                {
                    for (int i = 0; i < count; ++i)
                        restoreField(i);
                    shouldApply = true;
                    break;
                }
                else if (k.key == key_esc)
                {
                    return false;
                }
            }
        }

        if (shouldApply) break;

        if (mousemsg())
        {
            mouse_msg m = getmouse();
            if (m.msg == mouse_msg_up)
            {
                for (int i = 0; i < count; ++i)
                {
                    int fy = dlgY + 65 + i * 50;
                    if (m.x >= fieldX && m.x <= fieldX + fieldW &&
                        m.y >= fy && m.y <= fy + fieldH)
                    {
                        if (activeField != i)
                            activateField(i);
                        else
                        {
                            restoreField(activeField);
                            fields[i].buffer[0] = '\0';
                            fields[i].textLen = 0;
                            fields[i].dirty = false;
                        }
                        drawDialog();
                        break;
                    }
                }
                if (m.x >= okX && m.x <= okX + okW &&
                    m.y >= okY && m.y <= okY + okH)
                {
                    for (int i = 0; i < count; ++i)
                        restoreField(i);
                    shouldApply = true;
                }
                else if (m.x >= cancelX && m.x <= cancelX + cancelW &&
                         m.y >= cancelY && m.y <= cancelY + cancelH)
                {
                    return false;
                }
            }
        }

        delay_ms(10);
    }

    return true;
}

#endif