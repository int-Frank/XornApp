
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
  T_Model_World.Identity();
}

xn::PolygonLoop ScenePolygonLoop::GetTransformed() const
{
  return GetLoop().GetTransformed(T_Model_World);
}

xn::PolygonLoop ScenePolygonLoop::GetLoop() const
{
  xn::PolygonLoop loop;
  for (size_t i = 0; i < vertices.size(); i++)
    loop.PushBack(vertices[i]);
  return loop;
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

bool LoopCollection::GetAABB(xn::aabb *pOut) const
{
  if (m_loopMap.empty())
    return false;

  bool first = true;
  for (auto it = m_loopMap.cbegin_rand(); it != m_loopMap.cend_rand(); it++)
  {
    auto loop = it->second.GetTransformed();
    xn::aabb aabb;
    auto code = loop.GetAABB(&aabb);
    if (code != Dg::ErrorCode::None)
      continue;

    if (first)
    {
      *pOut = aabb;
      first = false;
    }
    else
    {
      *pOut += aabb;
    }
  }
  return !first;
}

std::vector<xn::PolygonLoop> LoopCollection::GetLoops() const
{
  std::vector<xn::PolygonLoop> result;
  for (auto it = m_loopMap.cbegin_rand(); it != m_loopMap.cend_rand(); it++)
    result.push_back(it->second.GetTransformed());
  return result;
}

//--------------------------------------------------------------------------------
// Project
//--------------------------------------------------------------------------------

Project::Project()
  : newGeometry(true)
{

}

void Project::Clear()
{
  loops.Clear();
  newGeometry = true;
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
          loop.vertices.push_back(verts[index - 1]);

        loops.Add(loop);
      }
    }
    newGeometry = true;
  }

  return true;
}