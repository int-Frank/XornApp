
#include "xnCommon.h"
#include "XornAppMessages.h"
#include "MessageBus.h"

#include "IRenderer.h"
#include "imgui.h"
#include "Canvas.h"
#include "Common.h"
#include <Logger.h>

using namespace xn;

class Canvas::PIMPL 
{
public:

  PIMPL(std::string const &name, MessageBus *pMsgBus, IRenderer *pRenderer)
    : name(name)
    , pMsgBus(pMsgBus)
    , pRenderer(pRenderer)
    , position(0.f, 0.f)
    , windowSize(100.f, 100.f)
    , renderSize(100.f, 100.f)
    , scroll(0.f)
    , mouseButtonDown{}
    , mousePosition(FLT_MAX, FLT_MAX)
  {}

  ~PIMPL()
  {
    delete pRenderer;
  }

  static float const s_minCanvasSize;

  std::string name;
  MessageBus *pMsgBus;
  IRenderer *pRenderer;
  xn::vec2 windowSize;
  xn::vec2 renderSize;
  xn::vec2 position;
  float scroll;
  bool mouseButtonDown[(int)xn::MouseInput::COUNT];
  Dg::Vector2<float> mousePosition;
};

float const Canvas::PIMPL::s_minCanvasSize = 50.f;

Canvas::Canvas(std::string const &name, MessageBus *pMsgBus, IRenderer *pRenderer)
  : m_pimpl(new PIMPL(name, pMsgBus, pRenderer))
{

}

Canvas::~Canvas()
{
  delete m_pimpl;
}

void Canvas::SetPosition(xn::vec2 const &v)
{
  m_pimpl->position = v;
}

void Canvas::SetSize(xn::vec2 const &v)
{
  m_pimpl->windowSize = v;
  m_pimpl->renderSize = v;
}

void Canvas::BeginFrame()
{
  ImGui::SetNextWindowPos(ToImGui(m_pimpl->position));
  ImGui::SetNextWindowSize(ToImGui(m_pimpl->windowSize));
  ImGui::Begin(m_pimpl->name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

  // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
  ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
  if (canvas_sz.x < PIMPL::s_minCanvasSize) canvas_sz.x = PIMPL::s_minCanvasSize;
  if (canvas_sz.y < PIMPL::s_minCanvasSize) canvas_sz.y = PIMPL::s_minCanvasSize;
  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

  // Draw border and background color
  ImGuiIO &io = ImGui::GetIO();
  m_pimpl->renderSize = xn::vec2(canvas_sz.x, canvas_sz.y);
  m_pimpl->pRenderer->SetSize(uint32_t(m_pimpl->renderSize.x()), uint32_t(m_pimpl->renderSize.y()));

  // This will catch our interactions
  ImGui::InvisibleButton(std::string(std::string("canvas##") + m_pimpl->name).c_str(), canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
  const bool is_hovered = ImGui::IsItemHovered(); // Hovered
  const bool is_active = ImGui::IsItemActive();   // Held

  vec2 mousePos(io.MousePos.x, io.MousePos.y);

  // Handle mouse button up...
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_pimpl->mouseButtonDown[(int)xn::MouseInput::LeftButton])
  {
    Message_MouseButtonUp *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonUp>();
    pMsg->button = xn::MouseInput::LeftButton;
    pMsg->position = mousePos;
    m_pimpl->pMsgBus->Post(pMsg);
    m_pimpl->mouseButtonDown[(int)xn::MouseInput::LeftButton] = false;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Right) && m_pimpl->mouseButtonDown[(int)xn::MouseInput::RightButton])
  {
    Message_MouseButtonUp *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonUp>();
    pMsg->button = xn::MouseInput::RightButton;
    pMsg->position = mousePos;
    m_pimpl->pMsgBus->Post(pMsg);
    m_pimpl->mouseButtonDown[(int)xn::MouseInput::RightButton] = false;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle) && m_pimpl->mouseButtonDown[(int)xn::MouseInput::MiddleButton])
  {
    Message_MouseButtonUp *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonUp>();
    pMsg->button = xn::MouseInput::MiddleButton;
    pMsg->position = mousePos;
    m_pimpl->pMsgBus->Post(pMsg);
    m_pimpl->mouseButtonDown[(int)xn::MouseInput::MiddleButton] = false;
  }

  // Handle mouse move
  if (m_pimpl->mousePosition.x() == FLT_MAX)
  {
    m_pimpl->mousePosition = mousePos;
  }
  else if (m_pimpl->mousePosition != mousePos)
  {
    Message_MouseMove *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseMove>();
    pMsg->position = mousePos;
    m_pimpl->pMsgBus->Post(pMsg);
    m_pimpl->mousePosition = mousePos;
  }

  // Handle mouse button down...
  if (is_hovered)
  {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !m_pimpl->mouseButtonDown[(int)xn::MouseInput::LeftButton])
    {
      Message_MouseButtonDown *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonDown>();
      pMsg->button = xn::MouseInput::LeftButton;
      pMsg->position = mousePos;
      m_pimpl->pMsgBus->Post(pMsg);
      m_pimpl->mouseButtonDown[(int)xn::MouseInput::LeftButton] = true;
    }

    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && !m_pimpl->mouseButtonDown[(int)xn::MouseInput::RightButton])
    {
      Message_MouseButtonDown *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonDown>();
      pMsg->button = xn::MouseInput::RightButton;
      pMsg->position = mousePos;
      m_pimpl->pMsgBus->Post(pMsg);
      m_pimpl->mouseButtonDown[(int)xn::MouseInput::RightButton] = true;
    }

    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && !m_pimpl->mouseButtonDown[(int)xn::MouseInput::MiddleButton])
    {
      Message_MouseButtonDown *pMsg = m_pimpl->pMsgBus->NewMessage<Message_MouseButtonDown>();
      pMsg->button = xn::MouseInput::MiddleButton;
      pMsg->position = mousePos;
      m_pimpl->pMsgBus->Post(pMsg);
      m_pimpl->mouseButtonDown[(int)xn::MouseInput::MiddleButton] = true;
    }
  }

  if (is_hovered && m_pimpl->scroll != 0.f)
  {
    Message_ZoomCamera *pMsg = m_pimpl->pMsgBus->NewMessage<Message_ZoomCamera>();
    pMsg->val = m_pimpl->scroll < 0.f ? 1.15f : 0.85f;
    m_pimpl->pMsgBus->Post(pMsg);
  }

  m_pimpl->scroll = 0.f;
}

void Canvas::EndFrame()
{
  ImVec2 pos = ToImGui(m_pimpl->position);
  ImGui::GetWindowDrawList()->AddImage(
    (void *)m_pimpl->pRenderer->GetTexture(),
    pos,
    ImVec2(pos.x + m_pimpl->pRenderer->GetWidth(),
           pos.y + m_pimpl->pRenderer->GetHeight()),
    ImVec2(0, 1), ImVec2(1, 0));

  ImGui::End();
}

IRenderer *Canvas::GetRenderer()
{
  return m_pimpl->pRenderer;
}

void Canvas::Handle(Message *pMsg)
{
  if (pMsg->GetType() == (uint32_t)MessageType::MouseScroll)
    m_pimpl->scroll = ((Message_MouseScroll *)(pMsg))->val;
}

xn::vec2 Canvas::GetRenderSize() const
{
  return m_pimpl->renderSize;
}