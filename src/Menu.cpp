#include "../include/Menu.h"
#include <cstring>

Menu::Menu(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    selectedOption = 0;
    // Load background texture (using the map image for now)
    backgroundTexture = TextureUtils::loadTexture("./assets/Maps/Original Map/original.jpg");
}

Menu::~Menu() {
    if (backgroundTexture != 0) {
        glDeleteTextures(1, &backgroundTexture);
    }
}

void Menu::setDimensions(int w, int h) {
    screenWidth = w;
    screenHeight = h;
}

void Menu::drawText(float x, float y, const char* text, bool selected) {
    if (selected) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    } else {
        glColor3f(0.7f, 0.7f, 0.7f); // Light Gray
    }
    
    glRasterPos2f(x, y);
    for (size_t i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
}

void Menu::draw() {
    // Draw Background Image
    if (backgroundTexture != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(0, 0);
        glTexCoord2f(1, 1); glVertex2f(screenWidth, 0);
        glTexCoord2f(1, 0); glVertex2f(screenWidth, screenHeight);
        glTexCoord2f(0, 0); glVertex2f(0, screenHeight);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    // Draw semi-transparent background overlay (Darker to make text readable)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // Reduced opacity since we have an image
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0, screenHeight);
    glEnd();

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    // Draw Title
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* title = "MAIN MENU";
    glRasterPos2f(centerX - 60, centerY + 150);
    for (size_t i = 0; i < strlen(title); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, title[i]);
    }

    // Draw Options
    // Center text approximation: 1 VS 1 is about 60px wide
    drawText(centerX - 40, centerY + 30, "1 VS 1", selectedOption == 0);
    drawText(centerX - 40, centerY - 30, "CO-OP", selectedOption == 1);
    
    // Instructions
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* instr = "Use W/S to select, ENTER to start";
    glRasterPos2f(centerX - 100, centerY - 150);
    for (size_t i = 0; i < strlen(instr); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, instr[i]);
    }
}

void Menu::handleInput(unsigned char key) {
    if (key == 'w' || key == 'W') {
        selectedOption = 0;
    }
    if (key == 's' || key == 'S') {
        selectedOption = 1;
    }
}

int Menu::getSelectedOption() const {
    return selectedOption;
}
