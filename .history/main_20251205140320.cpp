#include <windows.h>
#include <GL/glut.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ------------------------------
// Mario position + size
// ------------------------------
float marioX = 100;
float marioY = 100;
float marioW = 80;
float marioH = 80;

// ------------------------------
// Block object
// ------------------------------
float blockX = 400;
float blockY = 200;
float blockW = 120;
float blockH = 120;

GLuint marioTexture;

// ------------------------------
// LOAD TEXTURE
// ------------------------------
GLuint loadTexture(const char *path)
{
    int w, h, n;
    stbi_uc *data = stbi_load(path, &w, &h, &n, 4);

    if (!data)
    {
        std::cout << "Failed: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

// ------------------------------
// DRAW SPRITE
// ------------------------------
void drawSprite(GLuint tex, float x, float y, float w, float h)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);
    glTexCoord2f(1, 0);
    glVertex2f(x + w, y);
    glTexCoord2f(1, 1);
    glVertex2f(x + w, y + h);
    glTexCoord2f(0, 1);
    glVertex2f(x, y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ------------------------------
// AABB COLLISION
// ------------------------------
bool isColliding(
    float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh)
{
    return !(ax + aw < bx || ax > bx + bw ||
             ay + ah < by || ay > by + bh);
}

// ------------------------------
// DRAW EVERYTHING
// ------------------------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw block (white box)
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(blockX, blockY);
    glVertex2f(blockX + blockW, blockY);
    glVertex2f(blockX + blockW, blockY + blockH);
    glVertex2f(blockX, blockY + blockH);
    glEnd();

    // Draw Mario
    glColor3f(1, 1, 1);
    drawSprite(marioTexture, marioX, marioY, marioW, marioH);

    glutSwapBuffers();
}

// ------------------------------
// MOVEMENT: WASD
// ------------------------------
void keyboard(unsigned char key, int, int)
{
    float speed = 10;

    float oldX = marioX;
    float oldY = marioY;

    if (key == 'a')
        marioX -= speed;
    if (key == 'd')
        marioX += speed;
    if (key == 'w')
        marioY += speed;
    if (key == 's')
        marioY -= speed;

    // Check collision — restore old position if colliding
    if (isColliding(marioX, marioY, marioW, marioH,
                    blockX, blockY, blockW, blockH))
    {
        marioX = oldX;
        marioY = oldY;
    }

    glutPostRedisplay();
}

// ------------------------------
// KEEP PIXEL COORDINATES WHEN RESIZED
// ------------------------------
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
}

// ------------------------------
// MAIN
// ------------------------------
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(800, 600);
    glutCreateWindow("Mario Game");
    glutFullScreen();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    marioTexture = loadTexture("./assets/Mario/mario.png");
    marioTexture = loadTexture("./assets/Mario/mario_dead.png");
    glutMainLoop();
    return 0;
}
