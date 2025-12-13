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

    // Fireball cooldown
    float shootCooldown;
    float shootTimer;

    // Invulnerability
    float invulnerableTimer;

    // Golden State
    bool isGolden;
    float goldenTimer;

public:
    Player(float startX, float startY, float w, float h, const char* imagePath);
    virtual ~Player();

    virtual void draw();
    virtual void update(float deltaTime);
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

    // Sudden Death Mode
    static bool isSuddenDeathMode;
    static void setSuddenDeathMode(bool active); 

    // Getters/Setters
    float getX() const;
    float getY() const;
    float getW() const;
    float getH() const;
    float getSpeed() const;
    
    void setPos(float newX, float newY);
    void setHUDPos(float x, float y);
    
    float getTopY() const { return y + height; }
    
    virtual bool isFacingRight() const { return true; }

    // Bomb interaction
    void holdBomb(void* b) { heldBomb = b; } // Using void* to avoid circular include issues in header for now, or forward declare
    void* getHeldBomb() const { return heldBomb; }
    
    bool isInvulnerable() const { return invulnerableTimer > 0.0f; }
    
    void activateGolden();
    void updateGolden(float deltaTime);
    bool getIsGolden() const { return isGolden; }

protected:
    void* heldBomb; // Stored as void* to avoid circular dependency complexity in this step, cast in cpp
};