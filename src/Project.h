#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>

#include "DgMap_AVL.h"
#include "DgDynamicArray.h"

#include "xnGeometry.h"
#include "xnFlagArray.h"

#include "Common.h"

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
  Dg::DynamicArray<xn::vec2> vertices;
  xn::mat33 T_Model_World;
};

class LoopCollection
{
  typedef Dg::Map_AVL<PolygonID, ScenePolygonLoop>::const_iterator_rand const_iterator;
public:

  LoopCollection();

  friend std::ostream &operator<<(std::ostream &os, LoopCollection const &obj);

  PolygonID Add(ScenePolygonLoop const &);
  void Remove(PolygonID);
  ScenePolygonLoop *Get(PolygonID);

  const_iterator Begin() { return m_loopMap.cbegin_rand(); }
  const_iterator End() { return m_loopMap.cend_rand(); }

  std::vector<xn::PolygonLoop> GetLoops() const;

  void Clear();

  bool GetAABB(xn::aabb *) const;

private:
  PolygonID m_nextID;
  Dg::Map_AVL<PolygonID, ScenePolygonLoop> m_loopMap;
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

  bool newGeometry; // This is set to true whenever the project geoetry changes.
  LoopCollection loops;
};

#endif