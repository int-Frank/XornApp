
#include "imgui.h"
#include "ImGuiRenderer.h"
#include "Common.h"

class ImGuiRenderer::PIMPL
{
public:

  PIMPL()
    : p0()
    , p1()
    , pDrawList(nullptr)
  {

  }

  xn::vec2 p0;
  xn::vec2 p1;
  ImDrawList *pDrawList;
};

ImGuiRenderer::ImGuiRenderer()
  : m_pimpl(new PIMPL())
{

}

ImGuiRenderer::~ImGuiRenderer()
{
  delete m_pimpl;
}

void ImGuiRenderer::Set(xn::vec2 const &p0, xn::vec2 const &p1)
{
  m_pimpl->p0 = p0;
  m_pimpl->p1 = p1;
}

void ImGuiRenderer::BeginDraw()
{
  m_pimpl->pDrawList = ImGui::GetWindowDrawList();
  m_pimpl->pDrawList->AddRectFilled(ToImGui(m_pimpl->p0), ToImGui(m_pimpl->p1), IM_COL32(50, 50, 50, 255));
  m_pimpl->pDrawList->AddRect(ToImGui(m_pimpl->p0), ToImGui(m_pimpl->p1), IM_COL32(255, 255, 255, 255));
  m_pimpl->pDrawList->PushClipRect(ToImGui(m_pimpl->p0), ToImGui(m_pimpl->p1), true);
}

void ImGuiRenderer::DrawLine(xn::seg const &s, xn::LineProperties const &opts)
{
  m_pimpl->pDrawList->AddLine(ToImGui(s.GetP0()), ToImGui(s.GetP1()),
    IM_COL32(opts.clr.rgba8[0], opts.clr.rgba8[1], opts.clr.rgba8[2], opts.clr.rgba8[3]), opts.thickness);
}

void ImGuiRenderer::DrawFilledTriangle(xn::vec2 const &p0, xn::vec2 const &p1, xn::vec2 const &p2, xn::Colour clr)
{
  m_pimpl->pDrawList->AddTriangleFilled(ToImGui(p0), ToImGui(p1), ToImGui(p2),
    IM_COL32(clr.rgba8[0], clr.rgba8[1], clr.rgba8[2], clr.rgba8[3]));
}

void ImGuiRenderer::EndDraw()
{
  m_pimpl->pDrawList->PopClipRect();
}