#ifndef THEMESCENE_H
#define THEMESCENE_H
#include <QHash>
#include "datatypes.h"


class ThemeScene
{
public:
  ThemeScene();

  WidgetThemeData *getWidgetData(QString t_name);
  widgetFontStruct *getWidgetFont(QString t_name);
  QVector2D getWidgetSpacing(QString t_name);
  void setDummyTransform(QString t_name);
  void setWidgetTransform(QString t_name, pos_size_type t_transform);
  void setWidgetSpacing(QString t_name, QVector2D t_spacing);
  void setWidgetFont(QString t_name, widgetFontStruct *t_font);


  //Chatlog Sub Fonts
  // - Contains
  bool containsChatlogBold(QString t_type);
  bool containsChatlogColor(QString t_type);
  // - Get
  bool getChatlogBold(QString t_type);
  QString getChatlogColor(QString t_type);
  // - Set
  void setChatlogBold(QString t_name, bool t_isBold);
  void setChatlogColor(QString t_name, QString t_color);


private:
  QHash<QString, WidgetThemeData*> m_Widgets = {};
  QHash<QString, bool> m_ChatlogBold = {};
  QHash<QString, QString> m_ChatlogColor = {};
};

#endif // THEMESCENE_H
