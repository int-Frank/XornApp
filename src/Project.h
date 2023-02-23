#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <map>

#include "Common.h"
#include "xnGeometry.h"
#include "xnFlagArray.h"

typedef uint32_t PolygonID;
#define INVALID_POLYGON_ID 0xFFFFFFFFul

enum class ScenePolygonLoopFlag
{
  Invalid
};

class ScenePolygonLoop
{
public:

  ScenePolygonLoop();
  xn::PolygonLoop GetTransformed() const;
  xn::PolygonLoop GetLoop() const;

  xn::FlagArray<ScenePolygonLoopFlag> flags;
  std::vector<xn::vec2> vertices;
  xn::Transform T_Model_World;
};

class LoopCollection
{
  typedef std::map<PolygonID, ScenePolygonLoop>::const_iterator const_iterator;
public:

  LoopCollection();

  friend std::ostream &operator<<(std::ostream &os, LoopCollection const &obj);

  PolygonID Add(ScenePolygonLoop const &);
  void Remove(PolygonID);
  ScenePolygonLoop *Get(PolygonID);

  const_iterator Begin() { return m_loopMap.cbegin(); }
  const_iterator End() { return m_loopMap.cend(); }

  std::vector<xn::PolygonLoop> GetLoops() const;

  void Clear();

  bool GetAABB(xn::aabb *) const;

private:
  PolygonID m_nextID;
  std::map<PolygonID, ScenePolygonLoop> m_loopMap;
};

class Project
{
public:

  Project();

  bool Read(std::istream& is);
  friend std::ostream& operator<<(std::ostream& os, Project const&);

  bool Read(std::string const &filePath);
  bool Write(std::string const &filePath) const;

  bool ReadFromOBJFile(std::string const &filePath);

  void Clear();

  LoopCollection loops;
};

#endif