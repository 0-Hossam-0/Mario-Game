#pragma once
#include "Player.h"
#include "Fireball.h"
#include <vector>

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

    std::vector<Fireball*> fireballs;
    Player* enemy; // ADDED: Pointer to the opponent

public:
    static Mario* getInstance(float startX, float startY);
    static void destroyInstance();

    void update() override;
    void draw() override;
    void move(float dx, float dy, float &oldX, float &oldY) override;
    void jump() override;

    void setFacingRight(bool facing);
    void shootFireball();
    
    // ADDED: Set the enemy to check collision against
    void setEnemy(Player* p) { enemy = p; }
};