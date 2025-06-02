#include "replay_playback.h"
#include "dro/interface/scenes/replay_window.h"
#include "dro/interface/widgets/viewports/rp_viewport.h"
#include "dro/network/metadata/message_metadata.h"
#include "dro/param/replay_reader.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/audio.h"
#include "datatypes.h"
#include "dro/system/runtime_loop.h"

static ReplayWindow *s_replayWindow = nullptr;
static RPViewport *s_replayViewport = nullptr;

static int s_playbackTimestamp = 0;
static QVector<ReplayOperation> s_playbackOperations = {};

static QStringList s_replayPackages = {};
static QHash<QString, QStringList> s_packageCategories = {};

static QString s_outputPath = "";
static bool s_recordingActive = true;
static int s_recordingStartTime = 0;
static QVector<ReplayOperation> s_recordingOperations = {};

namespace dro::system::replays
{
  void assignWindow(ReplayWindow *window)
  {
    s_replayWindow = window;
  };

  void assignViewport(RPViewport *viewport)
  {
    s_replayViewport = viewport;
  }


  namespace recording
  {

    void start()
    {
      QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd (hh.mm.ss.z)'.json'");
      s_outputPath =  FS::Paths::BasePath() + "replays/" + fileName;
      s_recordingStartTime = RuntimeLoop::uptime();
      s_recordingActive = true;
      s_recordingOperations.clear();
    }

    void save()
    {
      QJsonObject replayJson;

      QJsonArray replayOperations;

      for(ReplayOperation operation : s_recordingOperations)
      {
        QJsonObject rNewOperations;
        rNewOperations["op"] = operation.operation;
        rNewOperations["time"] = operation.timestamp;

        QMap<QString, QString>::const_iterator i;
        for (i = operation.variables.constBegin(); i != operation.variables.constEnd(); ++i)
          rNewOperations[i.key()] = i.value();

        replayOperations.append(rNewOperations);
      }

      replayJson["script"] = replayOperations;


      QJsonDocument lOutputJson(replayJson);

      QFile file(s_outputPath);

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

    void messageCharacter()
    {
      const MessageMetadata message = dro::network::metadata::message::recentMessage();
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation lNewOperation = {"msg", timestampElapsed, {}};

      lNewOperation.variables["pre"] = message.characterPre;
      lNewOperation.variables["char"] = message.characterFolder;
      lNewOperation.variables["emote"] = message.characterEmote;
      lNewOperation.variables["msg"] = message.textContent;
      lNewOperation.variables["pos"] = message.backgroundPosition;
      lNewOperation.variables["sound"] = message.sfxName;
      lNewOperation.variables["color"] = QString::number(message.modifiers.TextColor);
      lNewOperation.variables["showname"] = message.userShowname;
      lNewOperation.variables["video"] = message.characterVideo;
      lNewOperation.variables["hide"] = QString::number(message.modifiers.Hidden);
      lNewOperation.variables["flip"] = QString::number(message.modifiers.Flipped);
      lNewOperation.variables["effect"] = message.effect.name;
      lNewOperation.variables["shout"] = message.characterShout;

      if(!message.characterSequence.isEmpty())
        lNewOperation.variables["sequence"] = message.characterSequence;

      if(!message.characterLayers.isEmpty())
        lNewOperation.variables["layers"] = message.characterLayers;

      if(message.offsetHorizontal != 500) lNewOperation.variables["offset_h"] = QString::number(message.offsetHorizontal);
      if(message.offsetVertical != 0) lNewOperation.variables["offset_v"] = QString::number(message.offsetVertical);
      if(message.offsetScale != 1000) lNewOperation.variables["offset_s"] = QString::number(message.offsetScale);

      s_recordingOperations.append(lNewOperation);
      save();
    }

    void messageSystem(QString name, QString message)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation lNewOperation = {"ooc", timestampElapsed, {}};

      lNewOperation.variables["name"] = name;
      lNewOperation.variables["msg"] = message;
      s_recordingOperations.append(lNewOperation);
      save();
    }

    void musicChange(QString music)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation lNewOperation = {"bgm", timestampElapsed, {}};

      lNewOperation.variables["track"] = music;
      s_recordingOperations.append(lNewOperation);
      save();
    }

    void backgroundChange(QString background)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation lNewOperation = {"bg", timestampElapsed, {}};

      lNewOperation.variables["name"] = background;
      s_recordingOperations.append(lNewOperation);
      save();
    }

  }

  namespace playback
  {

    void loadFile(QString name)
    {
      s_playbackTimestamp = 0;
      ReplayReader(name, s_playbackOperations);
      if(!s_replayWindow) return;
      s_replayWindow->setScrubberData(s_playbackOperations.count());

    }

    void progress()
    {
      if(!s_replayViewport) return;
      if(!s_replayWindow) return;

      if(s_playbackOperations.count() > (s_playbackTimestamp + 1))
      {
        s_playbackTimestamp += 1;
        QString operation = s_playbackOperations[s_playbackTimestamp].operation;

        if(operation == "msg")
        {
          dro::network::metadata::message::incomingMessage(s_playbackOperations[s_playbackTimestamp]);
          s_replayViewport->loadCurrentMessage();
        }

        if(operation == "bgm")
          audio::bgm::Play(s_playbackOperations[s_playbackTimestamp].variables["track"].toStdString());

        if(operation == "bg")
          s_replayViewport->loadBackground(s_playbackOperations[s_playbackTimestamp].variables["name"]);

        //if(mOp == "weather")
        //  p_SceneReplay->setWeather(s_playbackOperations[s_playbackTimestamp].variables["name"]);

        //if(mOp == "wtce")
        //  p_SceneReplay->playWTCE(s_playbackOperations[s_playbackTimestamp].variables["name"]);
        //  if(s_playbackOperations[s_playbackTimestamp].variables["name"] == "RoomTransition") mOp = "IGNORE";


        if(operation != "msg" && operation != "wtce") progress();
      }

      s_replayWindow->setScrubberPosition(s_playbackTimestamp);
    }

    void load(const QString &name, const QString &package, const QString &category)
    {
      if(!s_replayWindow) return;
      s_playbackTimestamp = 0;

      QString filePath = package.isEmpty() ? "base/replays/" : "packages/" + package + "/replays/";

      if(!category.isEmpty()) filePath += category + "/";

      filePath.append(name + ".json");

      ReplayReader(filePath, s_playbackOperations);
      s_replayWindow->setScrubberData(s_playbackOperations.count());
      progress();
    }

    void setTimestamp(int index)
    {
      s_playbackTimestamp = index;

      int l_position = index;
      bool m_BGFound = false;
      bool m_BGMFound = false;
      bool m_MSGFound = false;

      while(l_position != 0)
      {
        const QString &l_currentOp = s_playbackOperations[l_position].operation;
        if(l_currentOp == "bgm" && !m_BGMFound)
        {
          audio::bgm::Play(s_playbackOperations[l_position].variables["track"].toStdString());
          m_BGMFound = true;
        }
        if(l_currentOp == "bg" && !m_BGFound)
        {
          s_replayViewport->loadBackground(s_playbackOperations[l_position].variables["name"]);
          m_BGFound = true;
        }
        if(l_currentOp == "msg" && !m_MSGFound)
        {
          dro::network::metadata::message::incomingMessage(s_playbackOperations[l_position]);
          s_replayViewport->loadCurrentMessage();
          m_MSGFound = true;
        }

        l_position -= 1;
        if(m_BGFound && m_BGMFound && m_MSGFound) l_position = 0;
      }
    }

  }

  namespace io
  {

    void resetCache()
    {
      s_packageCategories.clear();
      s_replayPackages.clear();
      s_replayPackages.append("Local Recordings");
    }

    QStringList packageNames()
    {
      return s_replayPackages;
    }

    QStringList packageCategories(QString package)
    {
      return s_packageCategories[package];
    }

    void cachePackage(QString package, QStringList categories)
    {
      if(!s_replayPackages.contains(package))
      {
        s_replayPackages.append(package);
        s_packageCategories[package] = categories;
      }
    }

    QStringList packageContents(QString package, QString category)
    {
      QString path = "replays/";
      if(package.isEmpty())
      {
        if(!category.trimmed().isEmpty()) path += (category + "/");
        return FS::Paths::GetFileList(path, false, "json", false);
      }
      else
      {
        if(!category.trimmed().isEmpty()) path += (category + "/");
        return FS::Paths::GetFileList(path, package, "json", false);
      }

    }

  }

}
