#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>

#include "Common.h"
#include "DgMap_AVL.h"
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

  xn::FlagArray<ScenePolygonLoopFlag> flags;
  xn::PolygonLoop loop;
  xn::Transform T_Model_World;
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

  void Clear();

  xn::PolygonWithHoles BuildScenePolygon() const;

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

  LoopCollection loops;
};

#endif