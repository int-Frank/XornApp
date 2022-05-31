#ifndef CAMERA_H
#define CAMERA_H

#include "a2dGeometry.h"

struct Camera
{
  a2d::Transform T_Camera_World;
  a2d::mat33 T_World_View;
};

#endif