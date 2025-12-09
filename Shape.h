#ifndef SHAPE_H // <- Start of the include guard
#define SHAPE_H

#include <GL/glut.h>

// --- Helper Struct for RGB Colors ---
struct Color
{
  float r, g, b;
};

// --- Base Class: Shape ---
class Shape
{
protected:
  float x, y;
  Color color;

public:
  Shape(float _x, float _y, Color _c) : x(_x), y(_y), color(_c) {}

  virtual ~Shape() {}

  // Pure Virtual Method
  virtual void draw() = 0;

  void setPosition(float _x, float _y)
  {
    x = _x;
    y = _y;
  }
};

#endif // SHAPE_H // <- End of the include guard