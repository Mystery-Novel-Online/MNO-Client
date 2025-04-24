#ifndef COURTROOM_LAYOUT_H
#define COURTROOM_LAYOUT_H
#include <QString>
#include <QWidget>

namespace Layout::Courtroom
{

  void SetWidgetList(QHash<QString, QWidget *> widgetList);
  void AddToWidgetList(QString name, QWidget* widget);

  void CreateSticker(const char *name, const char *image, int axisX, int axisY, int sizeX, int sizeY);
  void MoveWidget(const char* name, int axisX, int axisY);
  void ToggleWidgetVisibility(const char* name, bool visibleState);
  void RaiseWidget(const char* name);


  void AppendToOOC(const char* name, const char* message);
}

#endif // COURTROOM_LAYOUT_H
