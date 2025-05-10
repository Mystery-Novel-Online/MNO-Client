#ifndef COURTROOM_LAYOUT_H
#define COURTROOM_LAYOUT_H
#include <QString>
#include <QWidget>

namespace courtroom
{
  void cleanup();

  namespace layout
  {
    void setWidgetList(QHash<QString, QWidget *> widgetList);
    void addWidget(QString& name, QWidget* widget);
    void moveWidget(const char* name, int x, int y);
    void raiseWidget(const char* name);
    void setVisibility(const char* name, bool visible);
    void setParent(const std::string& parent, const std::string& child);
  }

  namespace ooc
  {
    void appendMessage(const char* sender, const char* message);
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
    void setScale(int value);
    void setHorizontal(int value);
    void setVertical(int value);
  }

}

#endif // COURTROOM_LAYOUT_H
