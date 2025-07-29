#ifndef ROLECHAT_CONFIG_H
#define ROLECHAT_CONFIG_H

#include <QMainWindow>

namespace Ui
{
class RolechatConfig;
}

class RolechatConfig : public QMainWindow
{
  Q_OBJECT

public:
  explicit RolechatConfig(QWidget *parent = nullptr);
  ~RolechatConfig();

  QWidget* findTab(const QString& name);

private:
  void setupTabs();
  void switchCategory(QString category);

private slots:
  void on_config_save_button_clicked();

  void on_category_list_widget_currentTextChanged(const QString &currentText);

private:
  Ui::RolechatConfig *ui;

  QMap<QString, QWidget*> m_tabWidgets = {};
  QMap<QString, QStringList> m_tabCategories =
  {
    {"Content", {"Theme"}}
  };
};

#endif // ROLECHAT_CONFIG_H
