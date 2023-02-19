
#include "xnCommon.h"
#include "XornAppMessages.h"
#include "MessageBus.h"
#include "Input.h"

#include "imgui.h"
#include "Canvas.h"
#include "Common.h"
#include <Logger.h>

using namespace xn;

static uint32_t GetModState()
{
  uint32_t modState = 0;
  if (ImGui::GetIO().KeyMods & ImGuiModFlags_Ctrl) modState |= MK_ctrl;
  if (ImGui::GetIO().KeyMods & ImGuiModFlags_Shift) modState |= MK_shift;
  if (ImGui::GetIO().KeyMods & ImGuiModFlags_Alt) modState |= MK_alt;
  return modState;
}

class Canvas::PIMPL 
{
public:

  PIMPL(std::string const &name, MessageBus *pMsgBus)
    : name(name)
    , pMsgBus(pMsgBus)
    , position(0.f, 0.f)
    , windowSize(100.f, 100.f)
    , renderSize(100.f, 100.f)
    , scroll(0.f)
    , mouseButtonDown{}
    , oldImageMousePos(FLT_MAX, FLT_MAX)
  {}

  ~PIMPL()
  {

  }

  void HandleMouseUp(MouseInput button, vec2 const &mousePos)
  {
    Message_MouseButtonUp *pMsg = pMsgBus->NewMessage<Message_MouseButtonUp>();
    pMsg->button = button;
    pMsg->position = mousePos;
    pMsg->modState = GetModState();
    pMsgBus->Post(pMsg);
    mouseButtonDown[(int)button] = false;
  }

  void HandleMouseDown(MouseInput button, vec2 const &mousePos)
  {
    Message_MouseButtonDown *pMsg = pMsgBus->NewMessage<Message_MouseButtonDown>();
    pMsg->button = button;
    pMsg->position = mousePos;
    pMsg->modState = GetModState();
    pMsgBus->Post(pMsg);
    mouseButtonDown[(int)button] = true;
  }

  void HandleMouseMove(vec2 const &newImageMousePos)
  {
    if (oldImageMousePos.x() == FLT_MAX)
      oldImageMousePos = newImageMousePos;

    if (newImageMousePos == oldImageMousePos)
      return;

    vec2 newViewMousePos = newImageMousePos;
    Message_MouseMove *pMsg = pMsgBus->NewMessage<Message_MouseMove>();
    pMsg->position = newViewMousePos;
    pMsg->modState = GetModState();
    pMsgBus->Post(pMsg);
    oldImageMousePos = newImageMousePos;
  }

  mat33 GetMatrix_Screen_View() const
  {
    float w = renderSize.x();
    float h = renderSize.y();
    xn::mat33 t, s;
    t.Translation(vec2(-w, -h) / 2.f);
    s.Scaling(vec2(2.f / w, -2.f / h));

    return (t * s);
  }

  static float const s_minCanvasSize;

  std::string name;
  MessageBus *pMsgBus;
  xn::vec2 windowSize;
  xn::vec2 renderSize;
  xn::vec2 position;
  float scroll;
  bool mouseButtonDown[(int)MouseInput::COUNT];
  vec2 oldImageMousePos;
};

float const Canvas::PIMPL::s_minCanvasSize = 50.f;

Canvas::Canvas(std::string const &name, MessageBus *pMsgBus)
  : m_pimpl(new PIMPL(name, pMsgBus))
{

}

Canvas::~Canvas()
{
  delete m_pimpl;
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
  
  // This will catch our interactions
  ImGui::InvisibleButton(std::string(std::string("canvas##") + m_pimpl->name).c_str(), canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
  const bool is_hovered = ImGui::IsItemHovered(); // Hovered
  const bool is_active = ImGui::IsItemActive();   // Held

  vec2 mousePos(io.MousePos.x, io.MousePos.y);

  // Handle mouse button up...
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_pimpl->mouseButtonDown[(int)MouseInput::LeftButton])
    m_pimpl->HandleMouseUp(MouseInput::LeftButton, mousePos);

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Right) && m_pimpl->mouseButtonDown[(int)MouseInput::RightButton])
    m_pimpl->HandleMouseUp(MouseInput::RightButton, mousePos);

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle) && m_pimpl->mouseButtonDown[(int)MouseInput::MiddleButton])
    m_pimpl->HandleMouseUp(MouseInput::MiddleButton, mousePos);

  // Handle mouse move
  m_pimpl->HandleMouseMove(mousePos);

  // Handle mouse button down...
  if (is_hovered)
  {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !m_pimpl->mouseButtonDown[(int)MouseInput::LeftButton])
      m_pimpl->HandleMouseDown(MouseInput::LeftButton, mousePos);

    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && !m_pimpl->mouseButtonDown[(int)MouseInput::RightButton])
      m_pimpl->HandleMouseDown(MouseInput::RightButton, mousePos);

    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && !m_pimpl->mouseButtonDown[(int)MouseInput::MiddleButton])
      m_pimpl->HandleMouseDown(MouseInput::MiddleButton, mousePos);
  }

  // Zoom
  if (is_hovered && m_pimpl->scroll != 0.f)
  {
    Message_ZoomCamera *pMsg = m_pimpl->pMsgBus->NewMessage<Message_ZoomCamera>();
    pMsg->val = m_pimpl->scroll < 0.f ? 1.15f : 0.85f;
    m_pimpl->pMsgBus->Post(pMsg);
  }

  // Undo
  if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && ((ImGui::GetIO().KeyMods & ImGuiModFlags_Ctrl) > 0))
  {
    Message_Undo *pMsg = m_pimpl->pMsgBus->NewMessage<Message_Undo>();
    m_pimpl->pMsgBus->Post(pMsg);
  }

  // Redo
  if (ImGui::IsKeyPressed(ImGuiKey_Y, false) && ((ImGui::GetIO().KeyMods & ImGuiModFlags_Ctrl) > 0))
  {
    Message_Redo *pMsg = m_pimpl->pMsgBus->NewMessage<Message_Redo>();
    m_pimpl->pMsgBus->Post(pMsg);
  }

  m_pimpl->scroll = 0.f;
}

void Canvas::BlitImage(void *pImage, uint32_t w, uint32_t h)
{
  ImVec2 pos = ToImGui(m_pimpl->position);
  ImGui::GetWindowDrawList()->AddImage(
    pImage,
    pos,
    ImVec2(pos.x + w, pos.y + h),
    ImVec2(0, 1), ImVec2(1, 0));
}

void Canvas::EndFrame()
{
  ImGui::End();
}

void Canvas::Handle(Message *pMsg)
{
  if (pMsg->GetType() == (uint32_t)MessageType::MouseScroll)
    m_pimpl->scroll = ((Message_MouseScroll *)(pMsg))->val;
}

xn::vec2 Canvas::GetRenderRegionSize() const
{
  return m_pimpl->renderSize;
}

mat33 Canvas::GetMatrix_Screen_View() const
{
  return m_pimpl->GetMatrix_Screen_View();
}