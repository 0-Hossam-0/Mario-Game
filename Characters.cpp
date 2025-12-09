// Characters.cpp

#include "Mario.h"
#include "Luigi.h"
#include <GL/glut.h>
// --- Static Sprite Data Definitions ---

// Mario's Data
const int Mario::sprite[16][12] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 2, 2, 2, 3, 3, 2, 3, 0, 0, 0},
    {0, 2, 3, 2, 3, 3, 3, 2, 3, 3, 3, 0},
    {0, 2, 3, 2, 2, 3, 3, 3, 2, 3, 3, 3},
    {0, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 0},
    {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0},
    {0, 0, 1, 1, 2, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 0},
    {1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1},
    {3, 3, 1, 2, 3, 2, 2, 3, 2, 1, 3, 3},
    {3, 3, 3, 2, 2, 2, 2, 2, 2, 3, 3, 3},
    {3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3},
    {0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 0, 0},
    {0, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 0},
    {2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2}};

// Luigi's Data (Identical shape)
const int Luigi::sprite[16][12] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 2, 2, 2, 3, 3, 2, 3, 0, 0, 0},
    {0, 2, 3, 2, 3, 3, 3, 2, 3, 3, 3, 0},
    {0, 2, 3, 2, 2, 3, 3, 3, 2, 3, 3, 3},
    {0, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 0},
    {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0},
    {0, 0, 1, 1, 2, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 0},
    {1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1},
    {3, 3, 1, 2, 3, 2, 2, 3, 2, 1, 3, 3},
    {3, 3, 3, 2, 2, 2, 2, 2, 2, 3, 3, 3},
    {3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3},
    {0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 0, 0},
    {0, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 0},
    {2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2}};

// --- Mario Implementation ---

Mario::Mario(float _x, float _y, float _size)
    : Shape(_x, _y, {1.0f, 0.0f, 0.0f}), pixelSize(_size) {}

void Mario::setColor(int code)
{
  switch (code)
  {
  case 1:
    glColor3f(1.0f, 0.0f, 0.0f); // Mario Red
    break;
  case 2:
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    break;
  case 3:
    glColor3f(1.0f, 0.8f, 0.6f); // Skin
    break;
  default:
    glColor3f(0.0f, 0.0f, 0.0f);
    break;
  }
}

void Mario::draw()
{
  for (int row = 0; row < height; row++)
  {
    for (int col = 0; col < width; col++)
    {
      int colorCode = sprite[row][col];
      if (colorCode != 0)
      {
        setColor(colorCode);
        float px = x + (col * pixelSize);
        // Invert row calculation for top-to-bottom sprite drawing
        float py = y + ((height - row) * pixelSize);

        glBegin(GL_QUADS);
        glVertex2f(px, py);
        glVertex2f(px + pixelSize, py);
        glVertex2f(px + pixelSize, py + pixelSize);
        glVertex2f(px, py + pixelSize);
        glEnd();
      }
    }
  }
}

// --- Luigi Implementation ---

Luigi::Luigi(float _x, float _y, float _size)
    : Shape(_x, _y, {0.0f, 1.0f, 0.0f}), pixelSize(_size) {}

void Luigi::setColor(int code)
{
  switch (code)
  {
  case 1:
    glColor3f(0.0f, 0.8f, 0.0f); // Luigi Green
    break;
  case 2:
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    break;
  case 3:
    glColor3f(1.0f, 0.8f, 0.6f); // Skin
    break;
  default:
    glColor3f(0.0f, 0.0f, 0.0f);
    break;
  }
}

void Luigi::draw()
{
  for (int row = 0; row < height; row++)
  {
    for (int col = 0; col < width; col++)
    {
      int colorCode = sprite[row][col];
      if (colorCode != 0)
      {
        setColor(colorCode);
        float px = x + (col * pixelSize);
        // Invert row calculation for top-to-bottom sprite drawing
        float py = y + ((height - row) * pixelSize);

        glBegin(GL_QUADS);
        glVertex2f(px, py);
        glVertex2f(px + pixelSize, py);
        glVertex2f(px + pixelSize, py + pixelSize);
        glVertex2f(px, py + pixelSize);
        glEnd();
      }
    }
  }
}