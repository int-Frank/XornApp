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

  bool BeginWindow(const char *name, bool *p_open = nullptr, xn::UIFlags const *pFlags = nullptr) override;
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
  bool InputText(const char *label, char *buf, size_t buf_size) override;
  bool Combo(const char *pLabel, int *pCurrentItem, const char *const pItems[], int itemCount, int displayItemsCount = -1) override;
  bool RadioButton(const char *pLabel, int *pValue, int buttonHandle) override;
  void SameLine() override;

  bool SliderFloat(const char *label, float *v, float v_min, float v_max, const char *format = "%.3f", xn::UIFlags const *pFlags = nullptr) override;
  bool SliderInt(const char *label, int *v, int v_min, int v_max, const char *format = "%d", xn::UIFlags const *pFlags = nullptr) override;
  bool InputFloat(const char *label, float *v, float step, float step_fast, const char *format = "%.3f", xn::UIFlags const *pFlags = nullptr) override;
};

#endif