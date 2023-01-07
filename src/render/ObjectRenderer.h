#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include <string>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"

struct GL_State;

class ObjectRenderer
{
public:

  virtual void SetMatrix_World_View(xn::mat33 const &) = 0;
  virtual void SetResolution(xn::vec2 const &) = 0;

protected:

  GLuint GetShaderProgram(std::string const &a_fs, std::string const &a_vs);
};

#endif