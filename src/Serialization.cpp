
#include <fstream>

#include "Project.h"
#include "Logger.h"

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

//-------------------------------------------------------------------
// vec2 IO
//-------------------------------------------------------------------

bool ReadVec2(std::istream &is, xn::vec2 &v)
{
  float x, y;

  ASSERT_NEXT('[');
  READ_NUMBER(x);
  ASSERT_NEXT(',');
  READ_NUMBER(y);
  ASSERT_NEXT(']');

  v.x() = x;
  v.y() = y;

  return true;
}

std::ostream &operator<<(std::ostream &os, xn::vec2 const &obj)
{
  os << "[" << obj.x() << ',' << obj.y() << ']';
  return os;
}

//-------------------------------------------------------------------
// Transform IO
//-------------------------------------------------------------------

#define ID_TRANSFORM_SCALE "scale"
#define ID_TRANSFORM_POSITION "position"
#define ID_TRANSFORM_ROTATION "rotation"

bool ReadTransform(std::istream &is, xn::Transform &obj)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == ID_TRANSFORM_SCALE)
    {
      CHECK(ReadVec2(is, obj.scale));
    }
    else if (str == ID_TRANSFORM_POSITION)
    {
      CHECK(ReadVec2(is, obj.translation));
    }
    else if (str == ID_TRANSFORM_ROTATION)
    {
      READ_NUMBER(obj.rotation);
    }
    else
    {
      LOG_WARNING("Unknown tag found when reading a transform: '%s'", str.c_str());
      DiscardNextObject(is);
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

std::ostream &operator<<(std::ostream &os, xn::Transform const &obj)
{
  os << '{';

  os << "\"" ID_TRANSFORM_SCALE "\":" << obj.scale;
  os << ",\"" ID_TRANSFORM_POSITION "\":" << obj.translation;
  os << ",\"" ID_TRANSFORM_ROTATION "\":" << obj.rotation;

  os << '}';
  return os;
}

//-------------------------------------------------------------------
// PolygonLoop IO
//-------------------------------------------------------------------

bool ReadLoop(std::istream &is, std::vector<xn::vec2> &loop)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    xn::vec2 point;
    CHECK(ReadVec2(is, point));
    loop.push_back(point);

    DISCARD_NEXT_IF(',');
  }

  return true;
}

std::ostream &operator<<(std::ostream &os, std::vector<xn::vec2> const &obj)
{
  os << "[";

  for (size_t i = 0; i < obj.size(); i++)
  {
    os << obj[i];
    if (i + 1 != obj.size())
      os << ',';
    i++;
  }

  os << "]";
  return os;
}

//-------------------------------------------------------------------
// ScenePolygonLoop IO
//-------------------------------------------------------------------

#define ID_SCENEPOLYGONLOOP_POINTS "points"
#define ID_SCENEPOLYGONLOOP_TRANSFORM "transform"

bool ReadScenePolygonLoop(std::istream &is, ScenePolygonLoop &obj)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(is, str));
    ASSERT_NEXT(':');

    if (str == ID_SCENEPOLYGONLOOP_POINTS)
    {
      CHECK(ReadLoop(is, obj.vertices));
    }
    else if (str == ID_SCENEPOLYGONLOOP_TRANSFORM)
    {
      CHECK(ReadTransform(is, obj.T_Model_World));
    }
    else
    {
      LOG_WARNING("Unknown tag found in scene polygon loop: '%s'", str.c_str());
      CHECK(DiscardNextObject(is));
    }

    DISCARD_NEXT_IF(',');
  }

  return true;
}

std::ostream &operator<<(std::ostream &os, ScenePolygonLoop const &obj)
{
  os << '{';

  os << "\"" ID_SCENEPOLYGONLOOP_POINTS "\":" << obj.vertices;
  os << ",\"" ID_SCENEPOLYGONLOOP_TRANSFORM "\":" << obj.T_Model_World;

  os << '}';
  return os;
}

//-------------------------------------------------------------------
// LoopCollection IO
//-------------------------------------------------------------------

bool ReadLoops(std::istream &is, LoopCollection &loops)
{
  char c;
  std::string str;

  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    ScenePolygonLoop loop;
    CHECK(ReadScenePolygonLoop(is, loop));
    loops.Add(loop);

    DISCARD_NEXT_IF(',');
  };

  return true;
}

std::ostream& operator<<(std::ostream& os, LoopCollection const &obj)
{
  size_t i = 0;
  os << '[';
  for (auto it = obj.m_loopMap.cbegin_rand(); it != obj.m_loopMap.cend_rand(); it++, i++)
  {
    os << it->second;
    if (i + 1 != obj.m_loopMap.size())
      os << ',';
  }
  os << ']';
  return os;
}

//-------------------------------------------------------------------
// Project IO
//-------------------------------------------------------------------

#define ID_PROJECT_POLYGONS "polygon"

bool Project::Read(std::istream &is)
{
  char c;
  std::string str;

  ASSERT_NEXT('{');

  do
  {
    if (!ReadString(is, str))
      return false;

    ASSERT_NEXT(':');

    if (str == ID_PROJECT_POLYGONS)
    {
      CHECK(ReadLoops(is, loops));
    }
    else
    {
      LOG_WARNING("Unknown tag found in project: '%s'", str.c_str());
      CHECK(DiscardNextObject(is));
    }

    DISCARD_NEXT_IF(',');
    PEEK(c);
  } while (c != '}');

  return true;
}

std::ostream &operator<<(std::ostream &os, Project const &project)
{
  os << '{';

  os << "\"" ID_PROJECT_POLYGONS "\":" << project.loops;

  os << '}';
  return os;
}

bool Project::Read(std::string const &filePath)
{
  std::ifstream ifs(filePath);

  if (!ifs.good())
    return false;

  bool success = Read(ifs);
  if (!success)
    Clear();
  return success;
}

bool Project::Write(std::string const &filePath) const
{
  std::ofstream ofs(filePath);

  if (!ofs.good())
    return false;

  ofs << (*this);

  return ofs.good();
}
