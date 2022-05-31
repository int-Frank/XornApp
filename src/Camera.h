#ifndef CAMERA_H
#define CAMERA_H

#include "xnGeometry.h"

struct Camera
{
  xn::Transform T_Camera_World;
  xn::mat33 T_World_View;
};

#endif