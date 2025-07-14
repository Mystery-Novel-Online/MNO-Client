#ifndef SCENE_WIDGET_H
#define SCENE_WIDGET_H

#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include "datatypes.h"
#include "rp_button.h"
#include "dro/system/theme.h"

class SceneWidget : public QMainWindow
{
  Q_OBJECT

public:
  SceneWidget(ThemeSceneType scene, QWidget *parent = nullptr);
  ThemeSceneType getSceneType() const { return m_currentScene; }
  void reload();


  RPButton *createButton(QString name, QString image, std::function<void()> releasedFunction = nullptr);

  template <typename T, typename... Args>
  T* createWidget(const QString& themeKey, Args&&... args)
  {
    static_assert(std::is_base_of<QWidget, T>::value, "Class must be a QWidget");

    T* widget = new T(std::forward<Args>(args)..., this);
    QWidget* qWidget = static_cast<QWidget*>(widget);
    dro::system::theme::applyDimensions(qWidget, themeKey, m_currentScene);
    m_widgetThemeMap[qWidget] = themeKey;
    qWidget->show();
    return widget;
  }

private:
  ThemeSceneType m_currentScene;
  QMap<QWidget*, QString> m_widgetThemeMap = {};
};

#endif // SCENE_WIDGET_H
