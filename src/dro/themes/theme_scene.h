#ifndef THEMESCENE_H
#define THEMESCENE_H

class ThemeScene
{
public:
  ThemeScene();

  QStringList getWidgetNames()
  {
    QStringList l_returnValue = {};
    for (auto it = m_Widgets.begin(); it != m_Widgets.end(); ++it) l_returnValue << it.key();

    return l_returnValue;
  }

  WidgetThemeData *getWidgetData(QString t_name);
  widgetFontStruct *getWidgetFont(QString t_name);
  QVector2D getWidgetSpacing(QString t_name);
  void setDummyTransform(QString t_name);
  void setWidgetTransform(QString t_name, pos_size_type t_transform);
  void setWidgetRotation(QString t_name, double t_rotation);
  void setWidgetSpacing(QString t_name, QVector2D t_spacing);
  void setWidgetFont(QString t_name, widgetFontStruct *t_font);


  //Chatlog Sub Fonts
  // - Contains
  bool containsWidget(QString t_name) { return m_Widgets.contains(t_name);}
  bool containsChatlogBold(QString t_type) { return m_ChatlogBold.contains(t_type);}
  bool containsChatlogColor(QString t_type) { return m_ChatlogColor.contains(t_type); }



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
