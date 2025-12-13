#ifndef HUD_H
#define HUD_H

#include <GL/glut.h>
#include <string>
#include "TextureUtils.h"

class HUD
{
private:
    int lives;
    int score;
    GLuint iconTexture;
    GLuint heartTexture;  // heart icon for lives
    float x, y;      // position on screen
    float iconSize;  // size of character icon
    float heartSize; // size of heart icon
    
    // helper function to draw text
    void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18);
    
    // helper function to draw icon
    void drawIcon(float x, float y, GLuint texture);
    
    // helper function to draw heart with custom size
    void drawHeart(float x, float y, GLuint texture, float size);

public:
    // constructor for a single player HUD
    HUD(float xPos, float yPos,
        const char* iconPath,
        int initialLives = 3, int initialScore = 0);
    
    // destructor
    ~HUD();
    
    // draw the HUD
    void draw();
    
    // update methods
    void setLives(int l);
    void setScore(int s);
    void addScore(int points);
    void loseLife();
    void gainLife();
    void setPos(float newX, float newY) { x = newX; y = newY; }
    
    void setPowerUpTimer(float t) { powerUpTimer = t; }
    
    // getters
    int getLives() const;
    int getScore() const;
    bool isAlive() const;

private:
    float powerUpTimer;
};

#endif
