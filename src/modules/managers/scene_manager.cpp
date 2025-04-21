#include "scene_manager.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "aoapplication.h"
#include "file_functions.h"
#include "modules/background/background_reader.h"
#include "modules/background/legacy_background_reader.h"

SceneManager SceneManager::s_Instance;

void SceneManager::execLoadPlayerBackground(QString t_backgroundName)
{
  mBackgroundName = t_backgroundName;
  const QString l_backgroundJSONPath = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "background.json");
  if(file_exists(l_backgroundJSONPath))
  {
    pCurrentBackground = new BackgroundReader();
  }
  else
  {
    pCurrentBackground = new LegacyBackgroundReader();
  }

  pCurrentBackground->execLoadBackground(t_backgroundName);
}

int SceneManager::getBackgroundPosition()
{
  if(pCurrentBackground == nullptr) return 0;
  return pCurrentBackground->getVerticalPosition(0);
}

QString SceneManager::getBackgroundPath(QString t_position)
{
  if(pCurrentBackground == nullptr) return "";
  QString l_filename = pCurrentBackground->getBackgroundFilename(t_position);
  return AOApplication::getInstance()->get_background_sprite_path(mBackgroundName, l_filename);
}

QString SceneManager::getForegroundPath(QString t_position)
{
  if(pCurrentBackground == nullptr) return "";
  QString l_filename = pCurrentBackground->getForegroundFilename(t_position);
  return AOApplication::getInstance()->get_background_sprite_path(mBackgroundName, l_filename);
}

DRBackgroundSettings SceneManager::getBackgroundSettings()
{
  if(pCurrentBackground == nullptr) return DRBackgroundSettings();
  return pCurrentBackground->getSettings();
}

AOLabel *SceneManager::CreateTransition(QWidget *parents, AOApplication *ao_app, DRGraphicsView *viewport)
{
  pUiTransition = new AOLabel(parents, ao_app);
  pViewport = viewport;
  return pUiTransition;
}

AOLabel *SceneManager::GetTransition()
{
  return pUiTransition;
}

void SceneManager::RenderTransition()
{
  QImage image(pViewport->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  pViewport->scene()->render(&painter);
  pUiTransition->setPixmap(QPixmap::fromImage(image));
}

void SceneManager::AnimateTransition()
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  pUiTransition->setGraphicsEffect(eff);

  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(mFadeDuration);
  a->setStartValue(1);
  a->setEndValue(0);
  a->setEasingCurve(QEasingCurve::OutBack);
  a->start(QPropertyAnimation::DeleteWhenStopped);
}

void SceneManager::setFadeDuration(int duration)
{
  mFadeDuration = duration;
}

void SceneManager::clearPlayerDataList()
{
  mPlayerDataList.clear();
}

