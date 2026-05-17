#ifndef CONFIG_TAB_CALLWORDS_H
#define CONFIG_TAB_CALLWORDS_H

#include <QWidget>

enum class WordMatchMode
{
  ContainsAnywhere = 0,
  WholeWordOnly
};

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

  void addWord(const QString& word, WordMatchMode mode);
  bool matchWord(const QString& message, const QString& word, WordMatchMode mode);

  bool messageCheck(const QString& message, QString& matchedWord);

private slots:
  void on_callwordAdd_released();

private:
  Ui::ConfigTabCallwords *ui;
};

#endif // CONFIG_TAB_CALLWORDS_H
