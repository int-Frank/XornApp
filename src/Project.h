#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>

#include "appCommon.h"

class SceneObject
{
public:

  SceneObject();

  bool Read(std::istream& is);
  friend std::ostream& operator<<(std::ostream& os, SceneObject const&);

  a2d::Geometry geometry;
  std::string name;
  a2d::Transform transform;
  bool valid;
};

class SceneObjectCollection
{
public:

  bool Read(std::istream& is);
  friend std::ostream& operator<<(std::ostream& os, SceneObjectCollection const&);

  void Clear();
  std::vector<char> ToImGuiNameString() const;

  std::vector<SceneObject> objectList;
private:
};

class Project
{
public:

  Project();
  static Project *CreateDefaultProject();

  bool Read(std::istream& is);
  friend std::ostream& operator<<(std::ostream& os, Project const&);

  bool Read(std::string const &filePath);
  bool Write(std::string const &filePath) const;

  void RemoveCurrentFocus();
  bool GetSanitisedGeometry(a2d::Geometry *pOut);

  void Clear();
  bool AddNewObject(std::string const &filePath, std::string const &name);

  bool CompleteLoad();

  SceneObjectCollection sceneObjects; // Current active scene objects
  int currentFocus;                   // Index of the scene item currently in focus. -1 == no focus

private:

};

#endif