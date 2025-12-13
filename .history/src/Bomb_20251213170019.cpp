#include "../include/Bomb.h"
#include <cmath>
#include <iostream>

Bomb::Bomb(float startX, float startY)
{
    x = startX;
    y = startY;
    width = 45.0f;
    height = 45.0f;
    
    velocityX = 0.0f;
    velocityY = 0.0f;
    gravity = 0.5f;
    
    state = FALLING;
    holder = nullptr;
    
    explodeTimer = 0.0f;
    explosionDuration = 0.5f; // 0.5 seconds explosion visual
    
    textureNormal = TextureUtils::loadTexture("./assets/Maps/bomb.png");
    textureExploding = TextureUtils::loadTexture("./assets/Maps/bomb_explosion.png");
}

void Bomb::pickUp(Player* p)
{
    if (state == ON_GROUND || state == FALLING)
    {
        state = HELD;
        holder = p;
        p->holdBomb(this);
        velocityX = 0;
        velocityY = 0;
    }
}

void Bomb::throwBomb(bool facingRight)
{
    if (state == HELD && holder != nullptr)
    {
        state = THROWN;
        holder->holdBomb(nullptr); // Release from player
        Player::playSound("./assets/Sounds/bomb-throw.mp3");
        
        // Throw physics
        float throwSpeedX = 10.0f;
        float throwSpeedY = 10.0f;
        
        velocityX = facingRight ? throwSpeedX : -throwSpeedX;
        velocityY = throwSpeedY;
        
        // Start slightly in front/above
        x = holder->getX() + (facingRight ? holder->getW() : -width);
        y = holder->getY() + holder->getH() / 2;
        
        holder = nullptr;
    }
}

bool Bomb::checkCollision(Player* p)
{
    if (p == nullptr) return false;
    float pX = p->getX();
    float pY = p->getY();
    float pW = p->getW();
    float pH = p->getH();

    float checkX = x;
    float checkY = y;
    float checkW = width;
    float checkH = height;

    if (state == EXPLODING)
    {
        float explosionSize = 150.0f;
        float offset = (explosionSize - width) / 2.0f;
        checkX = x - offset;
        checkY = y - offset;
        checkW = explosionSize;
        checkH = explosionSize;
    }

    bool collisionX = checkX + checkW > pX && checkX < pX + pW;
    bool collisionY = checkY + checkH > pY && checkY < pY + pH;

    return collisionX && collisionY;
}

void Bomb::update(float deltaTime)
{
    if (state == DEAD) return;

    if (state == EXPLODING)
    {
        explodeTimer += deltaTime;
        if (explodeTimer >= explosionDuration)
        {
            state = DEAD;
        }
        return;
    }

    if (state == HELD)
    {
        if (holder != nullptr)
        {
            // Follow player
            // Center above player head or in hand?
            // Let's put it above head like Mario usually does
            x = holder->getX() + (holder->getW() - width) / 2;
            y = holder->getY() + holder->getH();
        }
        return;
    }

    // Physics for FALLING and THROWN
    if (state == FALLING || state == THROWN)
    {
        velocityY -= gravity;
        x += velocityX;
        y += velocityY;
        
        // Map Collision
        Map* map = Map::getInstance();
        bool hitGround = false;
        float groundLevel = 145.0f;

        if (map != nullptr)
        {
            const std::vector<Platform*>& platforms = map->getPlatforms();
            for (Platform* platform : platforms)
            {
                if (velocityY <= 0) // Only land when falling down
                {
                    if (x + width > platform->getX() && x < platform->getX() + platform->getWidth() &&
                        y <= platform->getTopY() && y + height >= platform->getTopY())
                    {
                        y = platform->getTopY();
                        hitGround = true;
                        break;
                    }
                }
            }
        }
        
        if (!hitGround && y <= groundLevel)
        {
            y = groundLevel;
            hitGround = true;
        }

        if (hitGround)
        {
            if (state == THROWN)
            {
                // Explode on impact!
                state = EXPLODING;
                // Play explosion sound?
                Player::playSound("./assets/Sounds/bomb.mp3");
            }
            else
            {
                state = ON_GROUND;
                velocityY = 0;
                velocityX = 0;
            }
        }
    }
    
    // Screen bounds
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    if (x < -50 || x > screenWidth + 50 || y < -50)
    {
        if (state == THROWN) state = DEAD; // Fell off world
    }
}

void Bomb::draw()
{
    if (state == DEAD) return;

    GLuint tex = (state == EXPLODING) ? textureExploding : textureNormal;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1, 1, 1);

    float drawX = x;
    float drawY = y;
    float drawW = width;
    float drawH = height;

    if (state == EXPLODING)
    {
        float explosionSize = 150.0f;
        float offset = (explosionSize - width) / 2.0f;
        drawX = x - offset;
        drawY = y - offset;
        drawW = explosionSize;
        drawH = explosionSize;
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(drawX, drawY);
    glTexCoord2f(1, 1); glVertex2f(drawX + drawW, drawY);
    glTexCoord2f(1, 0); glVertex2f(drawX + drawW, drawY + drawH);
    glTexCoord2f(0, 0); glVertex2f(drawX, drawY + drawH);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
