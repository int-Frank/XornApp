
#include <filesystem>
#include <sstream>
#include <fstream>

#include "Project.h"
#include "Logger.h"
#include "DefaultData.h"
#include "DgQueryPolygonPolygon.h"

//--------------------------------------------------------------------------------
// ScenePolygonLoop
//--------------------------------------------------------------------------------

ScenePolygonLoop::ScenePolygonLoop()
{

}

//--------------------------------------------------------------------------------
// LoopCollection
//--------------------------------------------------------------------------------

LoopCollection::LoopCollection()
  : m_nextID(0)
{

}

PolygonID LoopCollection::Add(ScenePolygonLoop const &loop)
{
  m_loopMap.insert(Dg::Pair<PolygonID, ScenePolygonLoop>(m_nextID, loop));
  PolygonID id = m_nextID;
  m_nextID++;
  return id;
}

void LoopCollection::Remove(PolygonID id)
{
  m_loopMap.erase(id);
}

ScenePolygonLoop *LoopCollection::Get(PolygonID id)
{
  auto it = m_loopMap.find(id);
  if (it == m_loopMap.end())
    return nullptr;
  return &(it->second);
}

void LoopCollection::Clear()
{
  m_loopMap.clear();
  m_nextID = 0;
}

static bool IsValidHole(xn::PolygonWithHoles const &polygon, Dg::DoublyLinkedList<xn::PolygonLoop>::iterator it_hole)
{
  if (polygon.loops.empty())
    return true;

  auto it = polygon.loops.cbegin();

  Dg::TI2PolygonPolygon<float> query;
  Dg::TI2PolygonPolygon<float>::Result result = query(*it, *it_hole);

  if (result.code != Dg::QueryCode::B_in_A)
    return false;

  it++;
  for (; it != polygon.loops.cend(); it++)
  {
    if (it == it_hole)
      continue;

    result = query(*it, *it_hole);

    if (result.code != Dg::QueryCode::NotIntersecting)
      return false;
  }
  return true;
}

static bool LoopCompare(xn::PolygonLoop const &a, xn::PolygonLoop const &b)
{
  return a.Area() > b.Area();
}

// Transformed to world space, sorted, windings set
static xn::PolygonWithHoles BuildPolygon(Dg::Map_AVL<PolygonID, ScenePolygonLoop> const &loops)
{
  xn::PolygonWithHoles result;

  if (loops.empty())
    return result;

  for (auto it = loops.cbegin_rand(); it != loops.cend_rand(); it++)
    result.loops.push_back(it->second.loop.GetTransformed(it->second.T_Model_World.ToMatrix33()));

  result.loops.sort(LoopCompare);

  result.loops.front().SetWinding(Dg::Orientation::CCW);

  for (auto it = ++result.loops.begin(); it != result.loops.end(); it++)
    it->SetWinding(Dg::Orientation::CW);

  return result;
}


#include "Logger.h"
xn::PolygonWithHoles LoopCollection::BuildScenePolygon() const
{
  xn::PolygonWithHoles result;

  if (m_loopMap.empty())
    return result;

  result = BuildPolygon(m_loopMap);

  auto it = ++result.loops.begin();
  
  for (; it != result.loops.end();)
  {
    if (!IsValidHole(result, it))
    {
      it = result.loops.erase(it);
      continue;
    }
    it++;
  }

  return result;
}


//--------------------------------------------------------------------------------
// Project
//--------------------------------------------------------------------------------

Project::Project()
{

}

void Project::Clear()
{
  loops.Clear();
}

bool Project::ReadFromOBJFile(std::string const &filePath)
{
  Clear();

  // First pass, read in verts;
  std::vector<xn::vec2> verts;
  {
    std::ifstream ifs(filePath);

    if (!ifs.good())
      return false;

    std::string line;
    while (std::getline(ifs, line))
    {
      std::istringstream iss(line);
      char c;
      if (!(iss >> c))
        continue;

      if (c == 'v')
      {
        float x, y;
        if (!(iss >> x >> y))
          continue;
        verts.push_back(xn::vec2(x, y));
      }
    }
  }

  // Second pass, read in polygons
  {
    std::ifstream ifs(filePath);

    if (!ifs.good())
      return false;

    std::string line;
    while (std::getline(ifs, line))
    {
      std::istringstream iss(line);
      char c;
      if (!(iss >> c))
        continue;

      if (c == 'l')
      {
        ScenePolygonLoop loop;
        int index;
        while (iss >> index)
          loop.loop.PushBack(verts[index - 1]);

        loops.Add(loop);
      }
    }
  }

  return true;
}