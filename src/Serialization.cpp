
#include <fstream>

#include "appProject.h"
#include "appLogger.h"
#include "a2dLineProperties.h"

#define KILL(...) do { LOG_ERROR(__VA_ARGS__); return false;} while (false)

#define GET_NEXT(c) do {if (!is.get(c).good()) KILL("GET_NEXT failed to read a character");} while (false)
#define GET_NEXT_IGNORE_WS(c) do {is >> std::ws >> c; if (!is.good()) KILL("GET_NEXT_IGNORE_WS failed to read a character");} while (false)
#define CHECK(fn) do {if (!fn) KILL(#fn " failed" );} while(false)
#define PEEK(c) do {is >> std::ws; int _intChar = is.peek(); if (_intChar == EOF) KILL("PEEK failed to read a character"); c = char(_intChar);} while (false)
#define ASSERT_NEXT(c)      do {char _tempChar = 0; PEEK(_tempChar); if (_tempChar != c) KILL("Invalid token. Expected '%c' got '%c'", c, _tempChar); if (!is.good()) KILL("ASSERT_NEXT failed to read character."); GET_NEXT_IGNORE_WS(_tempChar);} while (false)
#define DISCARD_NEXT_IF(c)  do {char _tempChar = 0; PEEK(_tempChar); if (_tempChar == c) GET_NEXT_IGNORE_WS(_tempChar);} while (false)
#define BREAK_ON(c) {char _tempChar = 0; PEEK(_tempChar); if (_tempChar == c) {GET_NEXT_IGNORE_WS(_tempChar); break;}}
#define READ_NUMBER(x) do {is >> x; if (!is.good()) KILL("Failed to read a number");} while (false)

bool ReadString(std::istream& is, std::string& str)
{
  str.clear();
  char c = 0;

  ASSERT_NEXT('\"');

  while (true)
  {
    GET_NEXT(c);
    if (c == '\"')
      break;

    if (c == '\\')
      GET_NEXT(c);

    str.push_back(c);
  }
  return true;
}

bool ReadVec2(std::istream& is, a2d::vec2& v)
{
  char c = 0;

  ASSERT_NEXT('[');

  is >> v[0] >> c >> v[1] >> c;

  return (is.good() && c == ']');
}

template<typename T>
bool ReadNumberArray(std::istream& is, T* pAry, size_t count)
{
  char c = 0;

  ASSERT_NEXT('[');

  for (size_t i = 0; i < count; i++)
    is >> pAry[i] >> c;

  return (is.good() && c == ']');
}

template<>
bool ReadNumberArray<uint8_t>(std::istream& is, uint8_t* pAry, size_t count)
{
  char c = 0;
  uint32_t t = 0;

  ASSERT_NEXT('[');

  for (size_t i = 0; i < count; i++)
  {
    is >> t >> c;
    pAry[i] = (uint8_t)t;
  }

  return (is.good() && c == ']');
}

bool DiscardObject(std::istream& is)
{
  char c = 0;
  int level = 0;
  do
  {
    GET_NEXT_IGNORE_WS(c);

    if (c == '{')
      level++;
    else if (c == '}')
      level--;

  } while (level != 0);

  return true;
}

bool DiscardList(std::istream& is)
{
  char c = 0;
  do
  {
    GET_NEXT_IGNORE_WS(c);
  } while (c != ']');

  return true;
}

bool DiscardNextObject(std::istream& is)
{
  char c = 0;
  std::string str;

  PEEK(c);
  if (c == '[')
    CHECK(DiscardList(is));
  else if (c == '{')
    CHECK(DiscardObject(is));
  else if (c == '"')
    CHECK(ReadString(is, str));
  else
    return false;

  return true;
}

bool TransformRead(a2d::Transform *pObj, std::istream& is)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "scale")
    {
      CHECK(ReadVec2(is, pObj->scale));
    }
    else if (str == "position")
    {
      CHECK(ReadVec2(is, pObj->position));
    }
    else if (str == "rotation")
    {
      READ_NUMBER(pObj->rotation);
    }
    else
    {
      LOG_WARNING("Unknown tag found when reading a transform: '%s'", str.c_str());
      DiscardNextObject(is);
    }

    DISCARD_NEXT_IF(',');
  };

  return true;
}

bool ColourRead(a2d::Colour *pObj, std::istream &is)
{
  READ_NUMBER(pObj->rgba32);
  return true;
}

bool LinePropertiesRead(a2d::LineProperties *pObj, std::istream& is)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "colour")
    {
      CHECK(ColourRead(&pObj->clr, is));
    }
    else if (str == "thickness")
    {
      READ_NUMBER(pObj->thickness);
    }
    else
    {
      LOG_WARNING("Unknown tag found when reading Line Render Option: '%s'", str.c_str());
      DiscardNextObject(is);
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadPointArray(a2d::DgPolygon *pPoly, std::istream &is)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    a2d::vec2 point;

    READ_NUMBER(point.x());
    ASSERT_NEXT(',');
    READ_NUMBER(point.y());

    pPoly->PushBack(point);

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool PolygonRead(a2d::Polygon *pObj, std::istream &is)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "points")
    {
      pObj->Clear();
      CHECK(ReadPointArray(pObj, is));
    }
    else
    {
      LOG_WARNING("Unknown tag found when reading Line Render Option: '%s'", str.c_str());
      DiscardNextObject(is);
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadPolygonArray(std::vector<a2d::Polygon> *pPolygons, std::istream &is)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    a2d::Polygon poly;
    CHECK(PolygonRead(&poly, is));
    pPolygons->push_back(poly);

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool GeometryRead(a2d::Geometry *pObj, std::istream &is)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "polygons")
    {
      CHECK(ReadPolygonArray(&pObj->polygons, is));
    }
    else
    {
      LOG_WARNING("Unknown tag found when reading Line Render Option: '%s'", str.c_str());
      DiscardNextObject(is);
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadGeometry(std::istream &is, a2d::Geometry *pGeom, std::string &name)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "name")
    {
      CHECK(ReadString(is, name));
    }
    else if (str == "geometry")
    {
      CHECK(GeometryRead(pGeom, is));
    }
    else
    {
      LOG_WARNING("Unknown tag found in geometry: '%s'", str.c_str());
      CHECK(DiscardNextObject(is));
    }

    DISCARD_NEXT_IF(',');
  }

    return true;
}

bool GeometryCollectionRead(a2d::GeometryCollection *pObj, std::istream &is)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    std::string name;
    a2d::Geometry geom;
    CHECK(ReadGeometry(is, &geom, name));
    pObj->PushBack(name, geom);

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool SceneObject::Read(std::istream& is)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == "geometry")
    {
      CHECK(GeometryRead(&geometry, is));
    }
    else if (str == "name")
    {
      CHECK(ReadString(is, name));
    }
    else if (str == "transform")
    {
      CHECK(TransformRead(&transform, is));
    }
    else
    {
      LOG_WARNING("Unknown tag found in active object: '%s'", str.c_str());
      CHECK(DiscardNextObject(is));
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool SceneObjectCollection::Read(std::istream& is)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    SceneObject data = {};
    CHECK(data.Read(is));
    objectList.push_back(data);

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool Project::Read(std::istream& is)
{
  char c;
  std::string str;

  ASSERT_NEXT('{');

  do
  {
    if (!ReadString(is, str))
      return false;

    ASSERT_NEXT(':');

    if (str == "active_objects")
    {
      CHECK(sceneObjects.Read(is));
    }
    else if (str == "current_focus")
    {
      READ_NUMBER(currentFocus);
    }
    else
    {
      LOG_WARNING("Unknown tag found in root: '%s'", str.c_str());
      CHECK(DiscardNextObject(is));
    }

    DISCARD_NEXT_IF(',');
    PEEK(c);
  } while (c != '}');

  return CompleteLoad();
}

std::string WrapStr(std::string const &str)
{
  return std::string("\"") + str + std::string("\"");
}

std::ostream& operator<<(std::ostream& os, a2d::Transform const &obj)
{
  os << '{';

  os << "\"scale\":[" << obj.scale[0] << ',' << obj.scale[1] << ']';
  os << ",\"position\":[" << obj.position[0] << ',' << obj.position[1] << ']';
  os << ",\"rotation\":" << obj.rotation;

  os << '}';
  return os;
}

std::ostream &operator<<(std::ostream &os, a2d::Colour const &obj)
{
  os << obj.rgba32;
  return os;
}

std::ostream& operator<<(std::ostream& os, a2d::LineProperties const &obj)
{
  os << '{';

  os << "\"colour\":" << obj.clr;
  os << ",\"thickness\":" << obj.thickness;

  os << '}';
  return os;
}

std::ostream &operator<<(std::ostream &os, a2d::Polygon const &obj)
{
  os << "{\"points\":[";

  size_t i = 0;
  for (auto it = obj.cPointsBegin(); it != obj.cPointsEnd(); it++)
  {
    os << it->x() << ',' << it->y();
    if (i + 1 != obj.Size())
      os << ',';
    i++;
  }

  os << "]}";
  return os;
}

std::ostream &operator<<(std::ostream &os, a2d::Geometry const &obj)
{
  os << "{\"polygons\":[";

  size_t i = 0;
  for (auto const &polygon : obj.polygons)
  {
    os << polygon;
    if (i + 1 != obj.polygons.size())
      os << ',';
    i++;
  }

  os << "]}";
  return os;
}

std::ostream &operator<<(std::ostream &os, a2d::GeometryCollection const &obj)
{
  std::map<std::string, a2d::Geometry> const *pList = obj.GetList();

  size_t i = 0;
  for (auto const &kv : *pList)
  {
    os << '{';

    os << "\"name\":" << WrapStr(kv.first);
    os << ",\"geometry\":" << kv.second;

    os << '}';

    if (i + 1 != pList->size())
      os << ',';
    i++;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, SceneObject const &obj)
{
  os << '{';

  os << "\"geometry\":" << obj.geometry;
  os << ",\"name\":" << WrapStr(obj.name);
  os << ",\"transform\":" << obj.transform;

  os << '}';
  return os;
}

std::ostream& operator<<(std::ostream& os, SceneObjectCollection const &objs)
{
  for (size_t i = 0; i < objs.objectList.size(); i++)
  {
    os << objs.objectList[i];
    if (i + 1 != objs.objectList.size())
      os << ',';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, Project const &project)
{
  os << '{';

  os << "\"active_objects\":[" << project.sceneObjects << "],";
  os << "\"current_focus\":" << project.currentFocus;

  os << '}';
  return os;
}

bool Project::Read(std::string const& filePath)
{
  std::ifstream ifs(filePath);

  if (!ifs.good())
    return false;

  bool success = Read(ifs);
  if (!success)
    Clear();
  return success;
}

bool Project::Write(std::string const& filePath) const
{
  std::ofstream ofs(filePath);

  if (!ofs.good())
    return false;

  ofs << (*this);

  return ofs.good();
}