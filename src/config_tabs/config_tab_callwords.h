#ifndef CONFIG_TAB_CALLWORDS_H
#define CONFIG_TAB_CALLWORDS_H

#include <QWidget>

namespace Ui
{
class ConfigTabCallwords;
}

class ConfigTabCallwords : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabCallwords(QWidget *parent = nullptr);
  ~ConfigTabCallwords();

private:
  Ui::ConfigTabCallwords *ui;
};

#endif // CONFIG_TAB_CALLWORDS_H
