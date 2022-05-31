#ifndef IMGUICONTEXT_H
#define IMGUICONTEXT_H

#include "xnUIContext.h"
#include "xnGeometry.h"

struct GLFWwindow;

class ImGuiUIContext : public xn::UIContext
{
public:

  ImGuiUIContext(GLFWwindow *pWindow, bool installCallbacks, char const *pVersion);
  ~ImGuiUIContext();

  void NewFrame() override;
  void Compose() override;
  void Draw() override;

  bool BeginWindow(const char *name, bool *p_open = nullptr, uint32_t flags = 0) override;
  void EndWindow() override;

  Dg::Vector2<float> GetCursorPos() override;

  void OpenPopup(const char *name, uint32_t flags = 0) override;
  bool BeginPopup(const char *name, uint32_t flags = 0) override;
  void EndPopup() override;
  void PushTextWrapPos(float) override;
  void PopTextWrapPos() override;

  void Separator() override;
  bool Button(const char *label, const Dg::Vector2<float> &size_arg = Dg::Vector2<float>(0.f, 0.f)) override;
  void Text(const char *fmt, ...) override;
  bool Checkbox(char const *pLabel, bool *pVal) override;

  bool SliderFloat(const char *label, float *v, float v_min, float v_max, const char *format = "%.3f", uint32_t flags = 0) override;
  bool SliderInt(const char *label, int *v, int v_min, int v_max, const char *format = "%d", uint32_t flags = 0) override;

};

#endif