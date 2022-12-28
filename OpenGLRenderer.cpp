
#include <string>
#include <vector>

#include "3rdParty/glad/include/glad/glad.h"
#include "3rdParty/GLFW/include/glfw3.h"

#include "xnRenderer.h"
#include "xnDraw.h"



class OpenGLRenderer : public xn::Renderer
{
public:

  void Set(Dg::Vector2<float> const &p0, Dg::Vector2<float> const &p1);
  void BeginDraw();
  void DrawLine(Dg::Segment2<float> const &, xn::Draw::Stroke);
  void DrawNGon(Dg::Vector2<float> const &centre, float radius, uint32_t sides, xn::Draw::Stroke);
  void DrawFilledNGon(Dg::Vector2<float> const &centre, uint32_t sides, float radius, xn::Draw::Fill);
  void DrawPolygon(Dg::Polygon2<float> const &, xn::Draw::Stroke);
  void DrawFilledPolygon(Dg::Polygon2<float> const &, xn::Draw::Fill);
  void DrawFilledTriangle(Dg::Vector2<float> const &p0, Dg::Vector2<float> const &p1, Dg::Vector2<float> const &p2, xn::Draw::Fill);
  void EndDraw();

  unsigned GetWidth() const;
  unsigned GetHeight() const;

  GLuint GetTexture() const;

};