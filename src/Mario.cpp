#include "../include/Mario.h"
#include "../include/TextureUtils.h"
#include <cstdlib>
#include <iostream>

Mario* Mario::instance = nullptr;

Mario::Mario(float startX, float startY)
    : Player(startX, startY, 80, 80, "./assets/Mario/mario.png")
{
  idleTexture = textureID; 
  moveTexture1 = TextureUtils::loadTexture("./assets/Mario/mario_move1.gif");
  moveTexture2 = TextureUtils::loadTexture("./assets/Mario/mario_move2.gif");
  speed = 6.0f;
  jumpStrength = 16.0f;
  jumpTexture = TextureUtils::loadTexture("./assets/Mario/mario_jump.png");
  deadTexture = TextureUtils::loadTexture("./assets/Mario/mario_dead.png");
  holdStandTexture = TextureUtils::loadTexture("./assets/Mario/mario_hold_stand.png");
  
  goldenIdleTexture = TextureUtils::loadTexture("./assets/Mario/golden_mario.png");
  goldenJumpTexture = TextureUtils::loadTexture("./assets/Mario/golden_mario_jump.png");
  goldenMoveTexture = TextureUtils::loadTexture("./assets/Mario/golden_mario_moves.png");
  
  isMoving = false;
  facingRight = true;
  animationFrame = 0;
  frameCounter = 0;
  enemy = nullptr;
  
  int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
  if (screenHeight == 0) screenHeight = 600; 
  initHUD(20, screenHeight - 90, "./assets/Mario/mario.png", 3, 0);
}

Mario* Mario::getInstance(float startX, float startY)
{
  if (instance == nullptr) instance = new Mario(startX, startY);
  return instance;
}

void Mario::destroyInstance()
{
  if (instance != nullptr)
  {
    if (instance->moveTexture1 != 0) glDeleteTextures(1, &instance->moveTexture1);
    if (instance->moveTexture2 != 0) glDeleteTextures(1, &instance->moveTexture2);
    if (instance->jumpTexture != 0) glDeleteTextures(1, &instance->jumpTexture);
    if (instance->jumpTexture1 != 0) glDeleteTextures(1, &instance->jumpTexture1);
    if (instance->deadTexture != 0) glDeleteTextures(1, &instance->deadTexture);
    if (instance->holdTexture != 0) glDeleteTextures(1, &instance->holdTexture);
    if (instance->holdStandTexture != 0) glDeleteTextures(1, &instance->holdStandTexture);
    
    for (Fireball* fb : instance->fireballs) delete fb;
    instance->fireballs.clear();

    delete instance;
    instance = nullptr;
  }
}

void Mario::draw()
{
  if (isInvulnerable() && !isGolden) {
      // Flash every 0.1 seconds
      int flash = (int)(invulnerableTimer * 10.0f);
      if (flash % 2 == 0) {
          drawHUD();
          for (Fireball* fb : fireballs) fb->draw();
          return; // Skip drawing sprite
      }
  }

  GLuint currentTexture;
  bool flipHorizontal = false;
  
  if (getLives() <= 0)
  {
      currentTexture = deadTexture;
      if (!facingRight) flipHorizontal = true; 
  }
  else if (isJumping)
  {
      currentTexture = jumpTexture;
      if (!facingRight) flipHorizontal = true;
  }
  else if (isMoving)
  {
    if (heldBomb != nullptr) {
        currentTexture = (animationFrame == 0) ? holdStandTexture : jumpTexture;
    } else {
        currentTexture = (animationFrame == 0) ? moveTexture1 : moveTexture2;
    }
    if (!facingRight) flipHorizontal = true;
  }
  else
  {
    if (heldBomb != nullptr) {
        currentTexture = holdStandTexture;
    } else {
        currentTexture = idleTexture;
    }
    if (!facingRight) flipHorizontal = true;
  }
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, currentTexture);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);
  if (flipHorizontal)
  {
      glTexCoord2f(1, 1); glVertex2f(x, y);
      glTexCoord2f(0, 1); glVertex2f(x + width, y);
      glTexCoord2f(0, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(1, 0); glVertex2f(x, y + height);
  }
  else
  {
      glTexCoord2f(0, 1); glVertex2f(x, y);
      glTexCoord2f(1, 1); glVertex2f(x + width, y);
      glTexCoord2f(1, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(0, 0); glVertex2f(x, y + height);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  drawHUD();

  for (Fireball* fb : fireballs) fb->draw();
}

void Mario::update(float deltaTime)
{
    isMoving = false;
    Player::update(deltaTime);

    for (int i = 0; i < fireballs.size(); i++) {
        fireballs[i]->update(enemy);
        
        if (!fireballs[i]->isActive()) {
            delete fireballs[i];
            fireballs.erase(fireballs.begin() + i);
            i--;
        }
    }
}

void Mario::move(float dx, float dy, float &oldX, float &oldY)
{
  if (dx > 0) facingRight = true;
  if (dx < 0) facingRight = false;

  Player::move(dx, dy, oldX, oldY);
  
  if (dx != 0)
  {
      if (x != oldX)
      {
        isMoving = true;
        int threshold = isSprinting ? 5 : 10;
        frameCounter++;
        if (frameCounter >= threshold)
        {
          animationFrame = (animationFrame == 0) ? 1 : 0;
          frameCounter = 0;
        }
      }
      else
      {
        isMoving = false;
        frameCounter = 0;
        animationFrame = 0;
      }
  }
}

void Mario::jump()
{
    bool wasOnGround = isOnGround;
    Player::jump();
    if (wasOnGround && isJumping) {
        playJumpSound();
    }
}

void Mario::setFacingRight(bool facing)
{
    facingRight = facing;
}

void Mario::shootFireball()
{
    if (!isGolden) return;
    if (shootTimer <= 0) {
        float spawnX = facingRight ? x + width : x - 20; 
        float spawnY = y + height / 2;
        fireballs.push_back(new Fireball(this, spawnX, spawnY, facingRight));
        playShootSound();
        shootTimer = shootCooldown;
    }
}

void Mario::playJumpSound()
{
    Player::playSound("./assets/Sounds/small-jump.mp3");
}

void Mario::playShootSound()
{
    Player::playSound("./assets/Sounds/smw_fireball.wav");
}