#ifndef RENDERERABLE_H
#define RENDERERABLE_H

#include <string>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"

struct GL_State;

class ObjectRenderer
{
public:

  void SetMatrix_World_Camera(xn::mat33 const &);

protected:

  GLuint GetShaderProgram(std::string const &a_fs, std::string const &a_vs);
  virtual void _SetMatrix_World_Camera(xn::mat33 const &) = 0;
};

#endif