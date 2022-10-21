
#include "imgui.h"
#include "ImGuiRenderer.h"
#include "Common.h"

#define TO_IMG_CLR(clr) IM_COL32(clr.rgba.r, clr.rgba.g, clr.rgba.b, clr.rgba.a)

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

void ImGuiRenderer::DrawLine(xn::seg const &s, xn::Draw::Stroke opts)
{
  m_pimpl->pDrawList->AddLine(ToImGui(s.GetP0()), ToImGui(s.GetP1()), TO_IMG_CLR(opts.clr), opts.thickness);
}

void ImGuiRenderer::DrawNGon(Dg::Vector2<float> const &centre, float radius, uint32_t sides, xn::Draw::Stroke opts)
{
  m_pimpl->pDrawList->AddNgon(ToImGui(centre), radius, TO_IMG_CLR(opts.clr), (int)sides, opts.thickness);
}

void ImGuiRenderer::DrawFilledNGon(Dg::Vector2<float> const &centre, uint32_t sides, float radius, xn::Draw::Fill opts)
{
  m_pimpl->pDrawList->AddNgonFilled(ToImGui(centre), radius, TO_IMG_CLR(opts.clr), (int)sides);
}

void ImGuiRenderer::DrawFilledTriangle(xn::vec2 const &p0, xn::vec2 const &p1, xn::vec2 const &p2, xn::Draw::Fill opts)
{
  m_pimpl->pDrawList->AddTriangleFilled(ToImGui(p0), ToImGui(p1), ToImGui(p2), TO_IMG_CLR(opts.clr));
}

void ImGuiRenderer::EndDraw()
{
  m_pimpl->pDrawList->PopClipRect();
}