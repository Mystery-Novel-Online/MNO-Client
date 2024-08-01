#include "game_manager.h"
#include "replay_manager.h"
#include "modules/managers/pathing_manager.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <aoapplication.h>
#include <QDir>
#include <modules/json/replay_reader.h>

ReplayManager ReplayManager::s_Instance;

QStringList ReplayManager::getReplayList(QString t_package, QString t_category)
{
  QString t_path = "replays/";
  if(!t_category.trimmed().isEmpty()) t_path += (t_category + "/");
  return PathingManager::get().searchDirectoryContentsSpecific(t_path, "json", t_package, false);
}

QString ReplayManager::getReplayPath(QString t_package, QString t_category, QString t_name)
{
  QString t_path = "";
  if(t_package.trimmed().isEmpty())
  {
    t_path = PathingManager::get().getBasePath() + "replays/";
  }
  else
  {
    t_path = PathingManager::get().getPackagePath(t_package) + "replays/";
  }

  if(!t_category.trimmed().isEmpty()) t_path += (t_category + "/");
  t_path += t_name;
  t_path += ".json";

  return t_path;
}

QString ReplayManager::getReplayImagePath(QString t_package, QString t_category, QString t_name)
{
  QString t_path = "";
  if(t_package.trimmed().isEmpty())
  {
    t_path = PathingManager::get().getBasePath() + "replays/";
  }
  else
  {
    t_path = PathingManager::get().getPackagePath(t_package) + "replays/";
  }

  if(!t_category.trimmed().isEmpty()) t_path += (t_category + "/");
  t_path += t_name;
  t_path += ".png";

  return t_path;
}

void ReplayManager::RecordingStart()
{
  QString l_FileName = QDateTime::currentDateTime().toString("yyyy-MM-dd (hh.mm.ss.z)'.json'");
  m_FilePathOutput =  AOApplication::getInstance()->get_base_path() + "replays/" + l_FileName;
  m_TimerRecorder.start();
}

void ReplayManager::RecordChangeMusic(QString t_music)
{
  ReplayOperation lNewOperation = ReplayOperation("bgm");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["track"] = t_music;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordChangeBackground(QString t_bgn)
{
  ReplayOperation lNewOperation = ReplayOperation("bg");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["name"] = t_bgn;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordMessageIC(ICMessageData *m_Message)
{
  ReplayOperation lNewOperation = ReplayOperation("msg");

  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();

  lNewOperation.mVariables["pre"] = m_Message->m_PreAnimation;
  lNewOperation.mVariables["char"] = m_Message->m_CharacterFolder;
  lNewOperation.mVariables["outfit"] = m_Message->m_CharacterOutfit;
  lNewOperation.mVariables["emote"] = m_Message->m_CharacterEmotion;
  lNewOperation.mVariables["msg"] = m_Message->m_MessageContents;
  lNewOperation.mVariables["pos"] = m_Message->m_AreaPosition;
  lNewOperation.mVariables["sound"] = m_Message->m_SFXName;
  lNewOperation.mVariables["color"] = QString::number(m_Message->m_TextColor);
  lNewOperation.mVariables["showname"] = m_Message->m_ShowName;
  lNewOperation.mVariables["video"] = m_Message->m_VideoName;
  lNewOperation.mVariables["hide"] = QString::number(m_Message->m_HideCharacter);
  lNewOperation.mVariables["flip"] = QString::number(m_Message->m_IsFlipped);
  lNewOperation.mVariables["effect"] = m_Message->m_EffectData.mName;
  lNewOperation.mVariables["shout"] = m_Message->m_ShoutName;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordMessageOOC(QString t_name, QString t_message)
{
  ReplayOperation lNewOperation = ReplayOperation("ooc");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["name"] = t_name;
  lNewOperation.mVariables["msg"] = t_message;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordPlaySplash(QString t_splash)
{
  ReplayOperation lNewOperation = ReplayOperation("wtce");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["name"] = t_splash;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordChangeWeather(QString t_weather)
{
  ReplayOperation lNewOperation = ReplayOperation("weather");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["name"] = t_weather;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordChangeGamemode(QString t_gamemode)
{
  //TO-DO: Implement
}

void ReplayManager::RecordChangeHour(QString t_hour)
{
  ReplayOperation lNewOperation = ReplayOperation("clock");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["hour"] = t_hour;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordChangeTOD(QString t_tod)
{
  ReplayOperation lNewOperation = ReplayOperation("TOD");
  lNewOperation.mTimestamp = m_TimerRecorder.elapsed();
  lNewOperation.mVariables["value"] = t_tod;
  m_ReplayOperationsRecorded.append(lNewOperation);
  RecordingSave();
}

void ReplayManager::RecordingSave()
{
  QJsonObject lReplayJson;

  QJsonArray lReplayOperations;

  for(ReplayOperation rOperation : m_ReplayOperationsRecorded)
  {
    QJsonObject rNewOperations;
    rNewOperations["op"] = rOperation.mOperation;
    rNewOperations["time"] = rOperation.mTimestamp;

    QMap<QString, QString>::const_iterator i;
    for (i = rOperation.mVariables.constBegin(); i != rOperation.mVariables.constEnd(); ++i)
    {
      rNewOperations[i.key()] = i.value();
    }

    lReplayOperations.append(rNewOperations);
  }

  lReplayJson["script"] = lReplayOperations;


  QJsonDocument lOutputJson(lReplayJson);

  QFile file(m_FilePathOutput);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << "Failed to open file for writing.";
    return;
  }

  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << lOutputJson.toJson();

  file.close();
}

void ReplayManager::PlaybackLoadFile(QString t_name, ReplayScene *p_scene)
{
  m_PlaybackPositionIndex = 0;
  p_SceneReplay = p_scene;
  ReplayReader l_reader = ReplayReader(t_name);
  m_ReplayOperationsPlayback = l_reader.getOperations();

  p_scene->SetupReplayMetadata(m_ReplayOperationsPlayback.count());
  return;
}

void ReplayManager::PlaybackProgressManual()
{
  if(m_ReplayOperationsPlayback.count() > (m_PlaybackPositionIndex + 1))
  {
    m_PlaybackPositionIndex += 1;
    QString mOp = m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mOperation;

    if(mOp == "msg")
    {
      p_SceneReplay->setMsgOperation(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables);
    }

    if(mOp == "weather")
    {
      p_SceneReplay->setWeather(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables["name"]);
    }

    if(mOp == "bg")
    {
      p_SceneReplay->setBackground(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables["name"]);
    }

    if(mOp == "wtce")
    {
      p_SceneReplay->playWTCE(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables["name"]);
      if(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables["name"] == "RoomTransition") mOp = "IGNORE";
    }

    if(mOp == "bgm")
    {
      p_SceneReplay->playSong(m_ReplayOperationsPlayback[m_PlaybackPositionIndex].mVariables["track"]);
    }

    if(mOp != "msg" && mOp != "wtce") PlaybackProgressManual();
  }

}

void ReplayManager::PlaybackProgressSlider(int t_index)
{
  m_PlaybackPositionIndex = t_index;

  int l_position = t_index;
  bool m_BGFound = false;
  bool m_BGMFound = false;
  bool m_MSGFound = false;

  while(l_position != 0)
  {
    QString l_currentOp = m_ReplayOperationsPlayback[l_position].mOperation;
    if(l_currentOp == "bgm" && !m_BGMFound)
    {
      p_SceneReplay->playSong(m_ReplayOperationsPlayback[l_position].mVariables["track"]);
      m_BGMFound = true;
    }
    if(l_currentOp == "bg" && !m_BGFound)
    {
      p_SceneReplay->setBackground(m_ReplayOperationsPlayback[l_position].mVariables["name"]);
      m_BGFound = true;
    }
    if(l_currentOp == "msg" && !m_MSGFound)
    {
      p_SceneReplay->setMsgOperation(m_ReplayOperationsPlayback[l_position].mVariables);
      m_MSGFound = true;
    }

    l_position -= 1;
    if(m_BGFound && m_BGMFound && m_MSGFound) l_position = 0;
  }

}

void ReplayManager::ResetPackagesCache()
{
  m_PackagesContaining.clear();
  m_PackageCategories.clear();
  m_PackagesContaining.append("Local Recordings");
}

void ReplayManager::CachePackageReplays(QString t_package, QVector<QString> t_tags)
{
  if(!m_PackagesContaining.contains(t_package))
  {
    m_PackagesContaining.append(t_package);
    m_PackageCategories[t_package] = t_tags;
  }
}

QStringList ReplayManager::getPackageNames()
{
  return m_PackagesContaining.toList();
}

QVector<QString> ReplayManager::getPackageCategoryList(QString t_package)
{
  return m_PackageCategories[t_package];
}
