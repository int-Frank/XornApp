#include "ImGuiUIContext.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static int ToImGuiFlags(xn::UIFlags const *pFlags)
{
  if (pFlags == nullptr)
    return 0;
  return 0;
}

ImGuiUIContext::ImGuiUIContext(GLFWwindow *pWindow, bool installCallbacks, char const *pVersion)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(pWindow, installCallbacks);
  ImGui_ImplOpenGL3_Init(pVersion);
}

ImGuiUIContext::~ImGuiUIContext()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiUIContext::NewFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiUIContext::Compose()
{
  ImGui::Render();
}

void ImGuiUIContext::Draw()
{
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiUIContext::BeginWindow(const char *pName, bool *pOpen, xn::UIFlags const *pFlags)
{
  ImGui::Begin(pName, pOpen, ToImGuiFlags(pFlags));

  //ImColor btnClr(30.f / 255.f, 48.f / 255.f, 74.f / 255.f, 1.f);
  //if (pFlags != nullptr && pFlags->QueryFlag(xn::UIFlag::IsActive))
  //  btnClr = ImColor(4.f / 255.f, 255.f / 255.f, 224.f / 255.f, 1.f);
  //
  //float width = ImGui::GetWindowWidth();
  //ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)btnClr);
  //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)btnClr);
  //ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)btnClr);
  //ImGui::Button(("##btn" + std::string(pName)).c_str(), ImVec2(width, 5.f));
  //ImGui::PopStyleColor(3);

  return ImGui::IsWindowFocused();
}

void ImGuiUIContext::EndWindow()
{
  ImGui::End();
}

Dg::Vector2<float> ImGuiUIContext::GetCursorPos()
{
  ImVec2 pos = ImGui::GetCursorPos();
  return xn::vec2(pos.x, pos.y);
}

void ImGuiUIContext::OpenPopup(const char *pName, uint32_t flags)
{
  ImGui::OpenPopup(pName, flags);
}

bool ImGuiUIContext::BeginPopup(const char *pName, uint32_t flags)
{
  return ImGui::BeginPopup(pName, flags);
}

void ImGuiUIContext::EndPopup()
{
  ImGui::EndPopup();
}

void ImGuiUIContext::PushTextWrapPos(float v)
{
  ImGui::PushTextWrapPos(v);
}

void ImGuiUIContext::PopTextWrapPos()
{
  ImGui::PopTextWrapPos();
}

void ImGuiUIContext::Separator()
{
  ImGui::Separator();
}

bool ImGuiUIContext::Button(const char *pLabel, Dg::Vector2<float> const &size)
{
  return ImGui::Button(pLabel, ImVec2(size.x(), size.y()));
}

void ImGuiUIContext::Text(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  ImGui::TextV(fmt, args);
  va_end(args);
}

bool ImGuiUIContext::SliderFloat(const char *pLabel, float *pV, float vMin, float vMax, const char *format, xn::UIFlags const *pFlags)
{
  return ImGui::SliderFloat(pLabel, pV, vMin, vMax, format, ImGuiSliderFlags(ToImGuiFlags(pFlags)));
}

bool ImGuiUIContext::SliderInt(const char *label, int *v, int v_min, int v_max, const char *format, xn::UIFlags const *pFlags)
{
  return ImGui::SliderInt(label, v, v_min, v_max, format, ToImGuiFlags(pFlags));
}

bool ImGuiUIContext::Checkbox(char const *pLabel, bool *pVal)
{
  return ImGui::Checkbox(pLabel, pVal);
}