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
    void resizeWidget(const char* name, int width, int height);
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
    void cleanupToggles();
    void deleteToggle(const QString& name);
  }

  namespace ooc
  {
    std::string getInputFieldContents();
    void setInputFieldContents(const std::string& text);
    std::string getDisplayName();
    void setDisplayName(const std::string& text);
    void appendMessage(const std::string& sender, const std::string& message);
  }

  namespace areas
  {
    void switchName(const std::string& name);
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

  namespace lineedit
  {
    void create(const std::string& name, const std::string& css, int x, int y, int width, int height);
    std::string getValue(const std::string& name);
    void setValue(const std::string& name, const std::string& value);
  }

  namespace hovercontroller
  {
    void create(const std::string& name, int x, int y, int width, int height);
    void addWidget(const std::string& name, const std::string& child);
  }

  namespace textedit
  {
    void create(const std::string& name, int x, int y, int width, int height);
    void setText(const std::string& name, const std::string& text);
    void setFrame(const std::string& name, const bool state);
  }

  namespace combobox
  {
    void create(const std::string& name, const std::string& css, int x, int y, int width, int height);
    void addItem(const std::string& name, const std::string& value);
    void removeItem(const std::string& name, const std::string& value);
    void clearItems(const std::string& name);
  }

  namespace sliders
  {
    void create(const std::string& name, int x, int y, int width, int height, int min, int max);
    void createVertical(const std::string& name, int x, int y, int width, int height, int min, int max);
    int  getValue(const std::string& name);
    void setValue(const std::string& name, int value);
    void setScale(int value);
    void setHorizontal(int value);
    void setVertical(int value);
  }

  namespace lists
  {
    void setAnimations(const QStringList& animations);
    std::string getAnimation();
  }

  namespace viewport
  {
    void update();
    void screenshot();
    void clearInteractions();
    void addInteraction(const std::string& name, const std::string& description, int x, int y, int width, int height);
    QPixmap getScreenshot();
  }

}

#endif // COURTROOM_LAYOUT_H
