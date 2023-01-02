#ifndef LINERENDERER_H
#define LINERENDERER_H

class ILineRenderer
{
public:

  virtual ~ILineRenderer() {}
  virtual void Draw(std::vector<xn::seg> const &, xn::Colour, float thickness, uint32_t flags, xn::mat33 const &T_Model_World) = 0;
};

ILineRenderer * CreateLineRenderer();

#endif