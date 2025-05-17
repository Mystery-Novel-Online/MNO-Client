#ifndef COURTROOM_LAYOUT_H
#define COURTROOM_LAYOUT_H
#include <QString>
#include <QWidget>

namespace courtroom
{
  void cleanup();
  void reload();

  namespace layout
  {
    void setWidgetList(QHash<QString, QWidget *> widgetList);
    void addWidget(QString& name, QWidget* widget);
    void moveWidget(const char* name, int x, int y);
    void raiseWidget(const char* name);
    void setVisibility(const char* name, bool visible);
    void setParent(const std::string& parent, const std::string& child);
    void deleteTab();
  }

  namespace ic
  {
    void focusMessageBox();
    std::string getMessageBoxContents();
    void setMessageBox(const std::string& text);
    void appendMessageBox(const std::string& text);
  }

  namespace tabs
  {
    void deleteToggle(const QString& name);
  }

  namespace ooc
  {
    std::string getInputFieldContents();
    std::string getDisplayName();
    void appendMessage(const char* sender, const char* message);
  }

  namespace choice
  {
    //void notifyPair(const char* sender, const char* message);
  }

  namespace stickers
  {
    void create(const std::string& name, const std::string& image, int x, int y, int width, int height);
  }

  namespace buttons
  {
    void create(const std::string& name, int x, int y, int width, int height);
  }

  namespace sliders
  {
    void create(const std::string& name, int x, int y, int width, int height, int min, int max);
    int  getValue(const std::string& name);
    void setValue(const std::string& name, int value);
    void setScale(int value);
    void setHorizontal(int value);
    void setVertical(int value);
  }

  namespace viewport
  {
    void screenshot();
  }

}

#endif // COURTROOM_LAYOUT_H
