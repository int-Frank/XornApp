
#include "xnRenderer.h"
#include "xnMessageBus.h"
#include "XornAppMessages.h"

#include "imgui.h"
#include "Canvas.h"
#include "Common.h"

float const Canvas::s_minCanvasSize = 50.f;

Canvas::Canvas(std::string const &name, xn::Renderer *pRenderer, xn::MessageBus *pMsgBus)
  : m_name(name)
  , m_pMsgBus(pMsgBus)
  , m_pRenderer(pRenderer)
  , m_position(0.f, 0.f)
  , m_size(100.f, 100.f)
  , m_scroll(0.f)
{

}

Canvas::~Canvas()
{
  delete m_pRenderer;
}

void Canvas::SetPosition(xn::vec2 const &v)
{
  m_position = v;
}

void Canvas::SetSize(xn::vec2 const &v)
{
  m_size = v;
}

xn::mat33 Canvas::Get_T_Camera_View() const
{
  xn::mat33 T_Camera_View;
  T_Camera_View.Translation(m_size / 2.0f);
  T_Camera_View[4] *= -1.f;
  return T_Camera_View;
}

xn::Renderer *Canvas::GetRenderer() const
{
  return m_pRenderer;
}

void Canvas::BeginFrame()
{
  ImGui::SetNextWindowPos(ToImGui(m_position));
  ImGui::SetNextWindowSize(ToImGui(m_size));
  ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

  // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
  ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
  if (canvas_sz.x < s_minCanvasSize) canvas_sz.x = s_minCanvasSize;
  if (canvas_sz.y < s_minCanvasSize) canvas_sz.y = s_minCanvasSize;
  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

  // Draw border and background color
  ImGuiIO &io = ImGui::GetIO();
  m_pRenderer->Set(xn::vec2(canvas_p0.x, canvas_p0.y), xn::vec2(canvas_p1.x, canvas_p1.y));

  // This will catch our interactions
  ImGui::InvisibleButton(std::string(std::string("canvas##") + m_name).c_str(), canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
  const bool is_hovered = ImGui::IsItemHovered(); // Hovered
  const bool is_active = ImGui::IsItemActive();   // Held

  if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.f))
  {
    Message_MoveCamera *pMsg = m_pMsgBus->NewMessage<Message_MoveCamera>();
    pMsg->v = Dg::Vector2<float>(io.MouseDelta.x, io.MouseDelta.y);
    m_pMsgBus->Post(pMsg);
  }

  if (is_hovered && m_scroll != 0.f)
  {
    Message_ZoomCamera *pMsg = m_pMsgBus->NewMessage<Message_ZoomCamera>();
    pMsg->val = m_scroll < 0.f ? 1.15f : 0.85f;
    m_pMsgBus->Post(pMsg);
  }

  m_scroll = 0.f;
}

void Canvas::EndFrame()
{
  ImGui::End();
}

void Canvas::Handle(xn::Message *pMsg)
{
  if (pMsg->GetType() == (uint32_t)MessageType::MouseScroll)
    m_scroll = ((Message_MouseScroll *)(pMsg))->val;
}
