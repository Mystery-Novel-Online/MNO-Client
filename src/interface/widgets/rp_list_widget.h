#ifndef RP_LIST_WIDGET_H
#define RP_LIST_WIDGET_H

#include <QListWidget>
#include <QObject>
#include <QWidget>

class RPListWidget : public QListWidget
{
  Q_OBJECT
public:
  RPListWidget(QWidget* parent);
  void clearSelection();
  void selectDefault();
  void selectText(const QString& value = "");
  void filterList(const QString& filter);

  void assignColors(const QColor& unselected, const QColor& selected, const QColor& header);
  void setItemColor(QListWidgetItem *item);
private:
  QColor m_colorUnselected;
  QColor m_colorSelected;
  QColor m_colorHeader;

};

#endif // RP_LIST_WIDGET_H
