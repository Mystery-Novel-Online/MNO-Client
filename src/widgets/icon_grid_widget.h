#ifndef ICON_GRID_WIDGET_H
#define ICON_GRID_WIDGET_H

class IconGridWidget : public RPWidget
{
  Q_OBJECT
public:
  IconGridWidget(QWidget *parent);
  void construct();
  void refresh();

};

#endif // ICON_GRID_WIDGET_H
