#ifndef MODAL_H
#define MODAL_H

#include <string>
#include <vector>

class ProgramData;
class App;

class Modal
{
public:

  Modal(std::string const &name);
  virtual ~Modal() {}

  // returns false if should close
  bool Show(App *pApp);

protected:

  virtual void _Show(App *pApp) = 0;
  void Close();

private:

  std::string m_name;
  bool m_first;
  bool m_stayOpen;
};

class Modal_SavePrompt : public Modal
{
public:

  Modal_SavePrompt();

private:
  void _Show(App *pApp) override;
};

class Modal_SaveAs : public Modal
{
public:

  Modal_SaveAs();

private:
  void _Show(App *pApp) override;
};

class Modal_Overwrite : public Modal
{
public:

  Modal_Overwrite(std::string const &filePath);

private:
  void _Show(App *pApp) override;
  std::string m_filePath;
};

class Modal_Open : public Modal
{
public:

  Modal_Open();

private:
  void _Show(App *pApp) override;
};

class Modal_Import : public Modal
{
public:

  Modal_Import();

private:
  void _Show(App *pApp) override;
};

class Modal_NewProject : public Modal
{
public:

  Modal_NewProject();

private:
  void _Show(App *pApp) override;
};

class Modal_Export : public Modal
{
public:

  Modal_Export();

private:
  void _Show(App *pApp) override;
};

class Modal_Alert : public Modal
{
public:

  Modal_Alert(std::string const &msg);

private:
  void _Show(App *pApp) override;
  std::string m_msg;
};

#endif