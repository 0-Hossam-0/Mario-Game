#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include <GL/glut.h>
#include <iostream>
#include "stb_image.h"

class TextureUtils
{
public:
  static GLuint loadTexture(const char *path)
  {
    int w, h, n;
    stbi_uc *data = stbi_load(path, &w, &h, &n, 4);

    if (!data)
    {
      std::cout << "Failed to load texture: " << path << std::endl;
      return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return tex;
  }
};

#endif