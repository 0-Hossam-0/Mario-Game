#pragma once
#include "Player.h"
#include "Fireball.h" // ADDED
#include <vector>     // ADDED

class Luigi : public Player {
private:
    static Luigi* instance;
    Luigi(float startX, float startY);

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

    // ADDED: Fireball support
    std::vector<Fireball*> fireballs;
    Player* enemy; 

public:
    static Luigi* getInstance(float startX, float startY);
    static void destroyInstance();

    void update() override;
    void draw() override;
    void move(float dx, float dy, float &oldX, float &oldY) override;
    void jump() override;

    void setFacingRight(bool facing);
    
    // ADDED: Shooting
    void shootFireball();
    void setEnemy(Player* p) { enemy = p; }
};