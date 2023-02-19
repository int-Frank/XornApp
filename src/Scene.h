//#ifndef SCENE_H
//#define SCENE_H
//
//#include "xnIScene.h"
//
//class IRenderer;
//
//class Scene : public xn::IScene
//{
//public:
//
//  Scene();
//  ~Scene();
//
//  void AddLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddFilledCircle(xn::vec2 const &centre, float radius, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddFilledCircleGroup(std::vector<xn::vec2> const &centres, float radius, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddFilledPolygon(xn::DgPolygon const &, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//  void AddFilledPolygon(xn::PolygonWithHoles const &, xn::Colour clr, uint32_t flags, uint32_t layer) override;
//
//  void Draw(IRenderer *);
//  void Clear();
//
//private:
//
//  class PIMPL;
//  PIMPL *m_pimpl;
//};
//
//
//#endif