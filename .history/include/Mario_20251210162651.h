#pragma once
#include "Player.h"
#include "Fireball.h" // ADDED
#include <vector>     // ADDED

class Mario : public Player {
private:
    static Mario* instance;
    Mario(float startX, float startY);

    // Textures
    GLuint idleTexture;
    GLuint moveTexture1;
    GLuint moveTexture2;
    GLuint jumpTexture;
    GLuint jumpTexture1;

    // Animation state
    bool isMoving;
    bool facingRight;
    int animationFrame;
    int frameCounter;

    // ADDED: List of fireballs
    std::vector<Fireball*> fireballs;

public:
    static Mario* getInstance(float startX, float startY);
    static void destroyInstance();

    void update() override;
    void draw() override;
    void move(float dx, float dy, float &oldX, float &oldY) override;
    void jump() override;

    void setFacingRight(bool facing);
    
    // ADDED: Shooting function
    void shootFireball();
};