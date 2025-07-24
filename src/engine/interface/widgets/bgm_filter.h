#ifndef BGM_FILTER_H
#define BGM_FILTER_H

#include <QComboBox>
#include <QObject>
#include <QWidget>

class BGMFilter : public QComboBox
{
  Q_OBJECT
public:
  BGMFilter(QWidget* parent);

  void ReloadCategories();
  QStringList GetMusicList();

};

#endif // BGM_FILTER_H
