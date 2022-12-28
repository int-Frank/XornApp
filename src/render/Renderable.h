#ifndef RENDERERABLE_H
#define RENDERERABLE_H

#include <string>

#include "../3rdParty/glad/include/glad/glad.h"
#include "../3rdParty/GLFW/include/glfw3.h"

#include "xnGeometry.h"

struct GL_State;

class Renderable
{
public:

  virtual void Clear() = 0;
  void SetMatrix_World_View(xn::mat33 const &);
  void Draw();

protected:

  GLuint GetShaderProgram(std::string const &a_fs, std::string const &a_vs);
  virtual void _Draw() = 0;
  virtual void _SetMatrix_World_View(xn::mat33 const &) = 0;
};

// Factory methods.
Renderable *GetRenderLine();

#endif