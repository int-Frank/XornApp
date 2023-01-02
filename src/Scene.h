#ifndef SCENE_H
#define SCENE_H

#include "xnIScene.h"

class IRenderer;

class Scene : public xn::IScene
{
public:

  Scene();
  ~Scene();

  void AddLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World) override;
  void AddLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World) override;
  void AddFilledNGon(xn::vec2 const &centre, uint32_t sides, float radius, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World) override {}
  void AddFilledNGonGroup(std::vector<xn::vec2> const &centres, uint32_t sides, float radius, uint32_t flags, uint32_t layer, xn::Colour clr, xn::mat33 T_Model_World) override {}
  void AddPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World) override;
  void AddFilledPolygon(xn::DgPolygon const &, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World) override {}

  void Draw(IRenderer *);
  void Clear();

private:

  class PIMPL;
  PIMPL *m_pimpl;
};


#endif