#ifndef CONFIG_TAB_MOUNTING_H
#define CONFIG_TAB_MOUNTING_H

#include <QWidget>

namespace Ui
{
class ConfigTabMounting;
}

class ConfigTabMounting : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabMounting(QWidget *parent = nullptr);
  ~ConfigTabMounting();

private slots:
  void mountStateChanged(QListWidgetItem *item);
  void addMountClicked();
  void removeMountClicked();

private:
  void updateList();
  Ui::ConfigTabMounting *ui;
};

#endif // CONFIG_TAB_MOUNTING_H
