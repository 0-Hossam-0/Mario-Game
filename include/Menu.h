#ifndef MENU_H
#define MENU_H

#include "TextureUtils.h"

class Menu {
private:
    int selectedOption; // 0 for 1vs1
    int screenWidth, screenHeight;
    GLuint backgroundTexture;
    void drawText(float x, float y, const char* text, bool selected);

public:
    Menu(int w, int h);
    ~Menu();
    void draw();
    void handleInput(unsigned char key);
    int getSelectedOption() const;
    void setDimensions(int w, int h);
};

#endif
