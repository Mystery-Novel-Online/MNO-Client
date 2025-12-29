#include "scene_manager.h"
#include "pch.h"

#include "engine/fs/fs_reading.h"
#include "engine/param/background/legacy_background_reader.h"
#include <modules/theme/thememanager.h>
#include "rolechat/background/JsonBackgroundData.h"

SceneManager SceneManager::s_Instance;

void SceneManager::execLoadPlayerBackground(QString t_backgroundName, QString variant)
{
  mBackgroundName = t_backgroundName;
  const QString l_backgroundJSONPath = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "background.json");
  if(FS::Checks::FileExists(l_backgroundJSONPath))
  {
    pCurrentBackground = new JsonBackgroundData();
    pCurrentBackground->loadBackground(l_backgroundJSONPath.toStdString());
    pCurrentBackground->setVariant(variant.toStdString());
  }
  else
  {
    pCurrentBackground = new LegacyBackgroundReader();
    pCurrentBackground->loadBackground(t_backgroundName.toStdString());
  }

}

QString SceneManager::getBackgroundPath(QString t_position)
{
  if(pCurrentBackground == nullptr) return "";
  QString l_filename = QString::fromStdString(pCurrentBackground->backgroundFilename(t_position.toStdString()));
  return AOApplication::getInstance()->get_background_sprite_path(mBackgroundName, l_filename);
}

QString SceneManager::getForegroundPath(QString t_position)
{
  if(pCurrentBackground == nullptr) return "";
  QString l_filename = QString::fromStdString(pCurrentBackground->foregroundFilename(t_position.toStdString()));
  return AOApplication::getInstance()->get_background_sprite_path(mBackgroundName, l_filename);
}

DRBackgroundSettings SceneManager::getBackgroundSettings()
{
  return DRBackgroundSettings();
}

RPLabel *SceneManager::CreateTransition(QWidget *parents, AOApplication *ao_app, DRGraphicsView *viewport)
{
  pUiTransition = new RPLabel(parents, ao_app);
  pViewport = viewport;
  return pUiTransition;
}

RPLabel *SceneManager::GetTransition()
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
