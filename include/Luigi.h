#pragma once
#include "Player.h"
#include "FireBall.h"
#include <vector>

class Luigi : public Player {
private:
    static Luigi* instance;
    Luigi(float startX, float startY);

    // textures
    GLuint idleTexture;
    GLuint moveTexture1;
    GLuint moveTexture2;
    GLuint jumpTexture;
    GLuint jumpTexture1;
    GLuint deadTexture; // added: dead sprite
    GLuint holdTexture;

    GLuint holdStandTexture;
    
    // golden Texture
    GLuint goldenTexture;
    GLuint goldenMoveTexture;

    // animation state
    bool isMoving;
    bool facingRight;
    int animationFrame;
    int frameCounter;

    std::vector<Fireball*> fireballs;
    Player* enemy; 

public:
    static Luigi* getInstance(float startX, float startY);
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