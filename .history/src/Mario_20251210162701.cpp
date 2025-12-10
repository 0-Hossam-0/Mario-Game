#include "../include/Mario.h"
#include "../include/TextureUtils.h"
#include <cstdlib>
#include <iostream>

// Initialize static instance pointer to nullptr
Mario* Mario::instance = nullptr;

// Private constructor
Mario::Mario(float startX, float startY)
    : Player(startX, startY, 80, 80, "./assets/Mario/mario.png")
{
  // Load animation textures
  idleTexture = textureID; // Use the default mario.png as idle
  moveTexture1 = TextureUtils::loadTexture("./assets/Mario/mario_move1.gif");
  moveTexture2 = TextureUtils::loadTexture("./assets/Mario/mario_move2.gif");
  
  // Mario's physics (Matched to Luigi)
  speed = 6.0f;
  jumpStrength = 16.0f;
  
  // Load new textures
  jumpTexture = TextureUtils::loadTexture("./assets/Mario/mario_jump.png");
  jumpTexture1 = TextureUtils::loadTexture("./assets/Mario/mario_jump_1.png");
  
  // Initialize animation state
  isMoving = false;
  facingRight = true;
  animationFrame = 0;
  frameCounter = 0;
  
  // Initialize HUD (Top-Left)
  int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
  if (screenHeight == 0) screenHeight = 600; // Fallback
  
  initHUD(20, screenHeight - 90, "./assets/Mario/mario.png", 3, 0);
}

// Get the singleton instance
Mario* Mario::getInstance(float startX, float startY)
{
  if (instance == nullptr)
  {
    instance = new Mario(startX, startY);
  }
  return instance;
}

// Destroy the singleton instance
void Mario::destroyInstance()
{
  if (instance != nullptr)
  {
    // Clean up animation textures
    if (instance->moveTexture1 != 0) glDeleteTextures(1, &instance->moveTexture1);
    if (instance->moveTexture2 != 0) glDeleteTextures(1, &instance->moveTexture2);
    if (instance->jumpTexture != 0) glDeleteTextures(1, &instance->jumpTexture);
    if (instance->jumpTexture1 != 0) glDeleteTextures(1, &instance->jumpTexture1);
    
    // ADDED: Clean up fireballs
    for (Fireball* fb : instance->fireballs) {
        delete fb;
    }
    instance->fireballs.clear();

    delete instance;
    instance = nullptr;
  }
}

// Override draw to use animated textures
void Mario::draw()
{
  GLuint currentTexture;
  bool flipHorizontal = false;
  
  if (isJumping)
  {
      // Show jumpTexture1 (takeoff) only for the very first frame of the jump
      if (velocityY > 16.5f)
      {
          currentTexture = jumpTexture1;
      }
      else
      {
          currentTexture = jumpTexture;
      }
      
      // Handle direction
      if (!facingRight) flipHorizontal = true;
  }
  else if (isMoving)
  {
    // Switch between move1 and move2 based on animation frame
    currentTexture = (animationFrame == 0) ? moveTexture1 : moveTexture2;
    
    // Handle direction
    if (!facingRight) flipHorizontal = true;
  }
  else
  {
    // Idle
    currentTexture = idleTexture;
    
    // Handle direction
    if (!facingRight) flipHorizontal = true;
  }
  
  // Draw with the current texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, currentTexture);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);
  
  if (flipHorizontal)
  {
      // Flip X coordinates
      glTexCoord2f(1, 1); glVertex2f(x, y);
      glTexCoord2f(0, 1); glVertex2f(x + width, y);
      glTexCoord2f(0, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(1, 0); glVertex2f(x, y + height);
  }
  else
  {
      // Normal orientation
      glTexCoord2f(0, 1); glVertex2f(x, y);
      glTexCoord2f(1, 1); glVertex2f(x + width, y);
      glTexCoord2f(1, 0); glVertex2f(x + width, y + height);
      glTexCoord2f(0, 0); glVertex2f(x, y + height);
  }
  
  glEnd();

  glDisable(GL_TEXTURE_2D);
  
  // Draw HUD
  drawHUD();

  // ADDED: Draw Fireballs
  for (Fireball* fb : fireballs) {
      fb->draw();
  }
}

// Override update to manage animation state
void Mario::update()
{
    // Reset moving state at start of frame
    isMoving = false;
    
    // Call base update (processes input and gravity)
    Player::update();

    // ADDED: Update Fireballs
    for (int i = 0; i < fireballs.size(); i++) {
        fireballs[i]->update();
        
        // Remove if inactive
        if (!fireballs[i]->isActive()) {
            delete fireballs[i];
            fireballs.erase(fireballs.begin() + i);
            i--;
        }
    }
}

// Override move to trigger animation and direction
void Mario::move(float dx, float dy, float &oldX, float &oldY)
{
  // Update direction
  if (dx > 0) facingRight = true;
  if (dx < 0) facingRight = false;

  // Call base class move which handles collision
  Player::move(dx, dy, oldX, oldY);
  
  // Only update walking animation state if this is a horizontal move
  if (dx != 0)
  {
      // Check if Mario actually moved
      if (x != oldX)
      {
        isMoving = true;
        
        // Update animation frame based on speed
        int threshold = isSprinting ? 5 : 10;
        
        frameCounter++;
        if (frameCounter >= threshold)
        {
          animationFrame = (animationFrame == 0) ? 1 : 0;
          frameCounter = 0;

          // Play footstep sounds
          if (animationFrame == 0)
          {
             system("gst-launch-1.0 filesrc location=./assets/Sounds/1st.wav ! decodebin ! autoaudiosink &");
          }
          else
          {
             system("gst-launch-1.0 filesrc location=./assets/Sounds/2nd.wav ! decodebin ! autoaudiosink &");
          }
        }
      }
      else
      {
        // Wall collision
        isMoving = false;
        frameCounter = 0;
        animationFrame = 0;
      }
  }
}

// Override jump to play sound
void Mario::jump()
{
    bool wasOnGround = isOnGround;
    Player::jump();
    
    if (wasOnGround && isJumping)
    {
        std::cout << "JUMP SOUND TRIGGERED" << std::endl;
        // Try explicit GStreamer pipeline
        system("gst-launch-1.0 filesrc location=./assets/Sounds/small-jump.mp3 ! decodebin ! autoaudiosink &");
    }
}

// Set facing direction manually
void Mario::setFacingRight(bool facing)
{
    facingRight = facing;
}

// ADDED: Shoot Fireball function
void Mario::shootFireball()
{
    // Spawn slightly in front of Mario
    float spawnX = facingRight ? x + width : x - 20; 
    float spawnY = y + height / 2;
    
    fireballs.push_back(new Fireball(spawnX, spawnY, facingRight));
    
    // Play Sound
    system("gst-launch-1.0 filesrc location=./assets/Sounds/fireball.wav ! decodebin ! autoaudiosink &");
}