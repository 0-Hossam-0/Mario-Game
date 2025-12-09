#include "Shape.h" // Include the base class definition

// --- Derived Class: Mario ---
class Mario : public Shape
{
private:
    float pixelSize;
    static const int height = 16;
    static const int width = 12;
    // Static sprite data is declared here, but defined (initialized) in the .cpp file
    static const int sprite[height][width];

public:
    Mario(float _x, float _y, float _size);

    // Implement the pure virtual draw method
    void draw() override;

private:
    void setColor(int code);
};
