#include "rp_list_widget.h"

RPListWidget::RPListWidget(QWidget *parent) : QListWidget(parent)
{

}

void RPListWidget::clearSelection()
{
  setCurrentRow(-1);
}

void RPListWidget::selectDefault()
{
  if (count() == 0)
    return;
  setCurrentRow(0);
}

void RPListWidget::selectText(const QString &value)
{
  if (count() == 0) return;
  if(value.trimmed().isEmpty())
  {
    setCurrentRow(0);
    return;
  }
  for(QListWidgetItem *item : findItems(value, Qt::MatchExactly))
  {
    setCurrentItem(item);
  };
}

void RPListWidget::filterList(const QString &filter)
{
  const QString finalFilter = filter.simplified();
  for (int i = 0; i < count(); i++)
  {
    QListWidgetItem *i_item = item(i);
    i_item->setHidden(!finalFilter.isEmpty() && !i_item->text().contains(finalFilter, Qt::CaseInsensitive));
  }
}

void RPListWidget::assignColors(const QColor &unselected, const QColor &selected, const QColor &header)
{
  m_colorUnselected = unselected;
  m_colorSelected = selected;
  m_colorHeader = header;
}

void RPListWidget::setItemColor(QListWidgetItem *item)
{
  if (item == nullptr) return;

  QColor l_color = m_colorUnselected;

  if(item->text().startsWith("[") && item->text().endsWith("]"))
    l_color = m_colorHeader;
  else if (item == currentItem())
    l_color = m_colorSelected;

  item->setBackground(l_color);
}
