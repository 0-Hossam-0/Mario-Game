#pragma once
#include "Player.h"
#include "FireBall.h"
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
    GLuint deadTexture; // ADDED: Dead sprite texture
    GLuint holdTexture;

    GLuint holdStandTexture;

    // Golden Textures
    GLuint goldenIdleTexture;
    GLuint goldenJumpTexture;
    GLuint goldenMoveTexture;

    // Animation state
    bool isMoving;
    bool facingRight;
    int animationFrame;
    int frameCounter;

    std::vector<Fireball*> fireballs;
    Player* enemy;

public:
    static Mario* getInstance(float startX, float startY);
    static void destroyInstance();

    void update(float deltaTime) override;
    void draw() override;
    void move(float dx, float dy, float &oldX, float &oldY) override;
    void jump() override;

    void setFacingRight(bool facing);
    bool isFacingRight() const override { return facingRight; }
    void shootFireball();
    
    void setEnemy(Player* p) { enemy = p; }
    
private:
    void playJumpSound();
    void playShootSound();
};