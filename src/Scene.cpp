
#include <algorithm>

#include "IRenderer.h"
#include "Scene.h"

class Renderable
{
public:

  Renderable()
    : layer(0)
  {

  }

  virtual void Draw(IRenderer *) = 0;

  uint32_t              layer;
};

class EntitySegment : public Renderable
{
public:

  EntitySegment()
    : Renderable()
    , segments()
    , T_Model_World()
    , color(0xFFFFFFFF)
    , thickness(2.0f)
    , flags(0)
  {
    T_Model_World.Identity();
  }

  void Draw(IRenderer *pRenderer) override
  {
    pRenderer->DrawLineGroup(segments, thickness, color, flags, T_Model_World);
  }

  std::vector<xn::seg>  segments;
  xn::mat33             T_Model_World;
  xn::Colour            color;
  float                 thickness;
  uint32_t              flags;
};

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
  for each (auto pRenderable in m_pimpl->renderables)
    delete pRenderable;
  m_pimpl->renderables.clear();
}

void Scene::AddLineGroup(std::vector<xn::seg> const &segments, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World)
{
  EntitySegment *pSegs = new EntitySegment();
  pSegs->segments = segments;
  pSegs->T_Model_World = T_Model_World;
  pSegs->thickness = thickness;
  pSegs->color = clr;
  pSegs->flags = flags;
  pSegs->layer = layer;
  m_pimpl->renderables.push_back(pSegs);
}

void Scene::AddLine(xn::seg const &segment, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World)
{
  EntitySegment *pSegs = new EntitySegment();
  pSegs->segments.push_back(segment);
  pSegs->T_Model_World = T_Model_World;
  pSegs->thickness = thickness;
  pSegs->color = clr;
  pSegs->flags = flags;
  pSegs->layer = layer;
  m_pimpl->renderables.push_back(pSegs);
}

void Scene::AddPolygon(xn::DgPolygon const &polygon, float thickness, xn::Colour clr, uint32_t flags, uint32_t layer, xn::mat33 T_Model_World)
{
  EntitySegment *pSegs = new EntitySegment();

  for (auto it = polygon.cEdgesBegin(); it != polygon.cEdgesEnd(); it++)
    pSegs->segments.push_back(it.ToSegment());

  pSegs->T_Model_World = T_Model_World;
  pSegs->thickness = thickness;
  pSegs->color = clr;
  pSegs->flags = flags;
  pSegs->layer = layer;
  m_pimpl->renderables.push_back(pSegs);
}