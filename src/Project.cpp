
#include <filesystem>
#include <sstream>

#include "appProject.h"
#include "appLogger.h"
#include "appDefaultData.h"
#include "DgQueryPolygonPolygon.h"

SceneObject::SceneObject()
  : geometry()
  , name()
  , transform()
  , valid(true)
{

}

void SceneObjectCollection::Clear()
{
  objectList.clear();
}

std::vector<char> SceneObjectCollection::ToImGuiNameString() const
{
  std::vector<char> result;
  for (auto const& obj : objectList)
  {
    for (char c : obj.name)
      result.push_back(c);
    result.push_back(0);
  }
  result.push_back(0);
  return result;
}

Project::Project()
  : sceneObjects()
  , currentFocus(-1)
{

}

void Project::Clear()
{
  sceneObjects.Clear();
  currentFocus = -1;
}

void Project::RemoveCurrentFocus()
{
  sceneObjects.objectList.erase(sceneObjects.objectList.begin() + currentFocus);

  currentFocus--;
  if (currentFocus == -1 && sceneObjects.objectList.size() != 0)
    currentFocus = 0;
}

// TODO This should be a Dg::Polygon2WithHoles (when it's done).
bool Project::GetSanitisedGeometry(a2d::Geometry *pOut)
{
  pOut->polygons.clear();

  if (sceneObjects.objectList.size() == 0)
  {
    LOG_ERROR("No geometry when sanitizing project geometry");
    return false;
  }

  pOut->polygons.push_back(a2d::Polygon()); //Reserve space for the boundary.
  for (size_t i = 0; i < sceneObjects.objectList.size(); i++)
  {
    SceneObject *pObj = &sceneObjects.objectList[i];
    pObj->valid = true;

    a2d::mat33 T_Model_World = pObj->transform.ToMatrix33();
    if (i == 0)
    {
      a2d::Polygon boundary = pObj->geometry.polygons[0].GetTransformed(T_Model_World);
      boundary.SetWinding(Dg::Orientation::CCW);
      pOut->polygons[0] = boundary;
    }
    else
    {
      std::vector<a2d::Polygon> goodPolygons;
      for (auto poly : pObj->geometry.polygons)
      {
        a2d::Polygon transformed = poly.GetTransformed(T_Model_World);

        for (size_t i = 0; i < pOut->polygons.size(); i++)
        {
          Dg::TI2PolygonPolygon<float> query;
          Dg::TI2PolygonPolygon<float>::Result result = query(pOut->polygons[i], transformed);

          if (((i == 0) && (result.code != Dg::QueryCode::B_in_A)) || ((i != 0) && (result.code != Dg::QueryCode::CompletelyOutside)))
          {
            pObj->valid = false;
            break;
          }
        }

        if (!pObj->valid)
          break;

        transformed.SetWinding(Dg::Orientation::CW);
        goodPolygons.push_back(transformed);
      }
      if (pObj->valid)
      {
        for (auto const &poly : goodPolygons)
          pOut->polygons.push_back(poly);
      }
    }
  }

  return true;
}

bool Project::AddNewObject(std::string const &filePath, std::string const &name)
{
  a2d::Geometry g;
  bool success = g.ReadFromOBJ(filePath);

  if (!success)
  {
    LOG_ERROR("Failed to open model: %s", filePath.c_str());
    return false;
  }

  SceneObject obj;
  obj.geometry = g;
  obj.name = name;

  sceneObjects.objectList.push_back(obj);
  currentFocus = (int)sceneObjects.objectList.size() - 1;
  return true;
}

bool Project::CompleteLoad()
{
  return true;
}

Project *Project::CreateDefaultProject()
{
  Project *pProject = new Project;

  SceneObject obj;
  obj.geometry = DefaultData::data.defaultBoundary;
  obj.name = DefaultData::data.defaultBoundaryName;
  obj.valid = true;

  pProject->sceneObjects.objectList.push_back(obj);
  pProject->currentFocus = 0;

  if (!pProject->CompleteLoad())
    LOG_ERROR("Internal error while creating default project.");

  return pProject;
}