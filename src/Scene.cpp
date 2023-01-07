
#include <algorithm>

#include "IRenderer.h"
#include "Scene.h"

//---------------------------------------------------------------------------
// Renderables
//---------------------------------------------------------------------------

class Renderable
{
public:

  Renderable(uint32_t layer)
    : layer(layer)
  {

  }

  virtual ~Renderable() {}
  virtual void Draw(IRenderer *) = 0;

  uint32_t layer;
};

class EntitySegment : public Renderable
{
public:

  EntitySegment(std::vector<xn::seg> const &segs, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer)
    : Renderable(layer)
    , m_segments(segs)
    , m_thickness(thickness)
    , m_color(clr)
    , m_flags(flags)
  {

  }

  void Draw(IRenderer *pRenderer) override
  {
    pRenderer->DrawLineGroup(m_segments, m_thickness, m_color, m_flags);
  }

private:

  std::vector<xn::seg>  m_segments;
  xn::Colour            m_color;
  float                 m_thickness;
  uint32_t              m_flags;
};

class EntityFilledCircle : public Renderable
{
public:

  EntityFilledCircle(std::vector<xn::vec2> const &positions, float radius, xn::Colour clr, uint32_t flags, uint32_t layer)
    : Renderable(layer)
    , m_positions(positions)
    , m_radius(radius)
    , m_color(clr)
    , m_flags(flags)
  {

  }

  void Draw(IRenderer *pRenderer) override
  {
    pRenderer->DrawFilledCircleGroup(m_positions, m_radius, m_color, m_flags);
  }

private:

  std::vector<xn::vec2> m_positions;
  xn::Colour            m_color;
  float                 m_radius;
  uint32_t              m_flags;
};

class EntityFilledPolygon : public Renderable
{
public:

  EntityFilledPolygon(std::vector<xn::seg> const &edges, xn::Colour clr, uint32_t flags, uint32_t layer)
    : Renderable(layer)
    , m_edges(edges)
    , m_color(clr)
    , m_flags(flags)
  {

  }

  void Draw(IRenderer *pRenderer) override
  {
    pRenderer->DrawFilledPolygon(m_edges, m_color, m_flags);
  }

private:

  std::vector<xn::seg>  m_edges;
  xn::Colour            m_color;
  uint32_t              m_flags;
};

//---------------------------------------------------------------------------
// Scene
//---------------------------------------------------------------------------

class Scene::PIMPL 
{
public:

  std::vector<Renderable*> renderables;
};

Scene::Scene()
  : m_pimpl(new PIMPL())
{

}

Scene::~Scene()
{
  delete m_pimpl;
}

void Scene::Draw(IRenderer *pRenderer)
{
  std::sort(m_pimpl->renderables.begin(), m_pimpl->renderables.end(),
    [](Renderable const *pA, Renderable const *pB) -> bool
    {return pA->layer < pB->layer; });

  for each (auto pRenderable in m_pimpl->renderables)
    pRenderable->Draw(pRenderer);
}

void Scene::Clear()
{
  for (auto pRenderable : m_pimpl->renderables)
    delete pRenderable;
  m_pimpl->renderables.clear();
}

void Scene::AddLineGroup(std::vector<xn::seg> const &segments, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  EntitySegment *pSegs = new EntitySegment(segments, thickness, clr, flags, layer);
  m_pimpl->renderables.push_back(pSegs);
}

void Scene::AddLine(xn::seg const &segment, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  std::vector<xn::seg> segments = {segment};
  EntitySegment *pSegs = new EntitySegment(segments, thickness, clr, flags, layer);
  m_pimpl->renderables.push_back(pSegs);
}

void Scene::AddFilledCircle(xn::vec2 const &position, float radius, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  std::vector<xn::vec2> positions = { position };
  EntityFilledCircle *pCircle = new EntityFilledCircle(positions, radius, clr, flags, layer);
  m_pimpl->renderables.push_back(pCircle);
}

void Scene::AddFilledCircleGroup(std::vector<xn::vec2> const &positions, float radius, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  EntityFilledCircle *pCircle = new EntityFilledCircle(positions, radius, clr, flags, layer);
  m_pimpl->renderables.push_back(pCircle);
}

void Scene::AddPolygon(xn::DgPolygon const &polygon, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  std::vector<xn::seg> segments;
  for (auto it = polygon.cEdgesBegin(); it != polygon.cEdgesEnd(); it++)
    segments.push_back(it.ToSegment());

  EntitySegment *pSegs = new EntitySegment(segments, thickness, clr, flags, layer);
  m_pimpl->renderables.push_back(pSegs);
}

void Scene::AddFilledPolygon(xn::PolygonWithHoles const &pwh, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  std::vector<xn::seg> edges;
  for (auto pit = pwh.loops.cbegin(); pit != pwh.loops.cend(); pit++)
  {
    for (auto eit = pit->cEdgesBegin(); eit != pit->cEdgesEnd(); eit++)
      edges.push_back(*eit);
  }
  EntityFilledPolygon *pPolygon = new EntityFilledPolygon(edges, clr, flags, layer);
  m_pimpl->renderables.push_back(pPolygon);
}

void Scene::AddFilledPolygon(xn::DgPolygon const &polygon, xn::Colour clr, uint32_t flags, uint32_t layer)
{
  std::vector<xn::seg> edges;
  for (auto eit = polygon.cEdgesBegin(); eit != polygon.cEdgesEnd(); eit++)
    edges.push_back(*eit);
  EntityFilledPolygon *pPolygon = new EntityFilledPolygon(edges, clr, flags, layer);
  m_pimpl->renderables.push_back(pPolygon);
}