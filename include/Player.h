#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glut.h>
#include "HUD.h"

class Player
{
protected:
  float x, y, width, height;
  float speed;
  GLuint textureID;
  HUD* hud;  // HUD for this player
  
  // Physics properties
  float velocityY;
  float gravity;
  float jumpStrength;
  bool isJumping;
  bool isOnGround;
  bool isSprinting;
  
  // Variable jump properties
  float jumpTimer;
  float maxJumpTime;
  
  // Input state
  bool keyStates[256];

public:
  // Constructor
  Player(float startX, float startY, float w, float h, const char *imagePath);
  
  // Destructor
  virtual ~Player();

  // Core Methods (Declarations only)
  virtual void draw(); // Made virtual to allow overriding
  virtual void move(float dx, float dy, float &oldX, float &oldY);
  void setKeyState(unsigned char key, bool pressed);
  void processInput(); // Process held keys
  virtual void jump();
  virtual void update();
  
  // HUD Methods
  void initHUD(float xPos, float yPos, const char* iconPath, int lives = 3, int score = 0);
  void drawHUD();
  void addScore(int points);
  void loseLife();
  void gainLife();
  int getScore() const;
  int getLives() const;
  void setHUDPos(float x, float y);

  // Getters and Setters
  float getX() const;
  float getY() const;
  float getW() const;
  float getH() const;
  float getSpeed() const;
  void setPos(float newX, float newY);
};

#endif