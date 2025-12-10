#pragma once
#include <GL/glut.h>
#include "TextureUtils.h"
#include "HUD.h"
#include <vector>

class Player {
protected:
    float x, y;
    float width, height;
    float speed;
    GLuint textureID;
    
    // Physics variables
    float velocityY;
    float gravity;
    float jumpStrength;
    float maxJumpTime;
    float jumpTimer;
    bool isJumping;
    bool isOnGround;
    bool isSprinting;
    
    bool keyStates[256];
    HUD* hud;

public:
    Player(float startX, float startY, float w, float h, const char* imagePath);
    virtual ~Player();

    virtual void draw();
    virtual void update();
    virtual void move(float dx, float dy, float &oldX, float &oldY);
    virtual void jump();
    
    void processInput();
    void setKeyState(unsigned char key, bool pressed);

    // HUD wrappers
    void initHUD(float x, float y, const char* iconPath, int lives, int score);
    void drawHUD();
    void addScore(int points);
    
    virtual void loseLife(); 
    void gainLife();
    int getScore() const;
    int getLives() const;
    
    // CHANGED: Static helper to play sound on Windows
    static void playSound(const char* path); 

    // Getters/Setters
    float getX() const;
    float getY() const;
    float getW() const;
    float getH() const;
    float getSpeed() const;
    
    void setPos(float newX, float newY);
    void setHUDPos(float x, float y);
    
    float getTopY() const { return y + height; }
};