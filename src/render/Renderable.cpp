
#include <vector>

#include "Renderable.h"
#include "MyException.h"

struct GL_State
{
  GLint program;
  GLint texture;
  GLint active_texture;
  GLint array_buffer;
  GLint element_array_buffer;
  GLint vertex_array;
  GLint blend_src;
  GLint blend_dst;
  GLint blend_equation_rgb;
  GLint blend_equation_alpha;
  GLint viewport[4];
  GLboolean enable_blend;
  GLboolean enable_cull_face;
  GLboolean enable_depth_test;
  GLboolean enable_scissor_test;
};

static GL_State *GetGLState()
{
  GL_State *pState = new GL_State{};

  glGetIntegerv(GL_CURRENT_PROGRAM, &pState->program);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &pState->texture);
  glGetIntegerv(GL_ACTIVE_TEXTURE, &pState->active_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &pState->array_buffer);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &pState->element_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &pState->vertex_array);
  glGetIntegerv(GL_BLEND_SRC, &pState->blend_src);
  glGetIntegerv(GL_BLEND_DST, &pState->blend_dst);
  glGetIntegerv(GL_BLEND_EQUATION_RGB, &pState->blend_equation_rgb);
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &pState->blend_equation_alpha);
  glGetIntegerv(GL_VIEWPORT, pState->viewport);
  pState->enable_blend = glIsEnabled(GL_BLEND);
  pState->enable_cull_face = glIsEnabled(GL_CULL_FACE);
  pState->enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  pState->enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  return pState;
}

static void SetGLState(GL_State const *pState)
{
  glUseProgram(pState->program);
  glActiveTexture(pState->active_texture);
  glBindTexture(GL_TEXTURE_2D, pState->texture);
  glBindVertexArray(pState->vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, pState->array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pState->element_array_buffer);
  glBlendEquationSeparate(pState->blend_equation_rgb, pState->blend_equation_alpha);
  glBlendFunc(pState->blend_src, pState->blend_dst);
  if (pState->enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (pState->enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (pState->enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (pState->enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  glViewport(pState->viewport[0], pState->viewport[1], (GLsizei)pState->viewport[2], (GLsizei)pState->viewport[3]);
}

static void DestroyGLState(GL_State **ppState)
{
  delete *ppState;
  *ppState = nullptr;
}

GLuint ObjectRenderer::GetShaderProgram(std::string const &a_vs, std::string const &a_fs)
{
  //Create shader ID
  GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

  //Set shader source
  const GLchar *pvs_str = a_vs.c_str();
  glShaderSource(vsID, 1, (const GLchar **)&pvs_str, nullptr);

  const GLchar *pfs_str = a_fs.c_str();
  glShaderSource(fsID, 1, (const GLchar **)&pfs_str, nullptr);

  //Compile shader source
  glCompileShader(vsID);
  glCompileShader(fsID);

  //Check shader for errors
  GLint vsCompiled = GL_FALSE;
  GLint fsCompiled = GL_FALSE;
  glGetShaderiv(vsID, GL_COMPILE_STATUS, &vsCompiled);
  glGetShaderiv(fsID, GL_COMPILE_STATUS, &fsCompiled);
  if (vsCompiled != GL_TRUE)
  {
    GLint maxLength = 0;
    glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(vsID, maxLength, &maxLength, &errorLog[0]);

    std::string reason(errorLog.begin(), errorLog.end());
    throw MyException(reason);
  }
  if (fsCompiled != GL_TRUE)
  {
    GLint maxLength = 0;
    glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(fsID, maxLength, &maxLength, &errorLog[0]);

    std::string reason(errorLog.begin(), errorLog.end());
    throw MyException(reason);
  }

  //Create program, attach shaders to it and link it.
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vsID);
  glAttachShader(shaderProgram, fsID);
  glLinkProgram(shaderProgram);

  GLint linkStatus = GL_TRUE;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE)
  {
    GLint logLen;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
    std::vector<char> msgSrc(logLen + 1);
    GLsizei written;
    glGetProgramInfoLog(shaderProgram, logLen, &written, msgSrc.data());
    std::string message(msgSrc.begin(), msgSrc.end());
    throw MyException(message);
  }

  //Detach and delete shaders afer we have successfully linked the program.
  glDetachShader(shaderProgram, vsID);
  glDetachShader(shaderProgram, fsID);
  glDeleteShader(vsID);
  glDeleteShader(fsID);

  return shaderProgram;
}

void ObjectRenderer::SetMatrix_World_Camera(xn::mat33 const &mat)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  _SetMatrix_World_Camera(mat);
  glUseProgram(prog);
}