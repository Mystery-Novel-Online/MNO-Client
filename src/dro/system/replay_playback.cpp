#include "replay_playback.h"
#include "pch.h"

#include "dro/interface/scenes/replay_window.h"
#include "dro/interface/widgets/viewports/rp_viewport.h"

#include "dro/network/metadata/message_metadata.h"
#include "dro/param/replay_reader.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/audio.h"
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

// Saving Limits
static int s_limitEarliestMessage = 0;
static int s_limitLatestMessage = 0;
static int s_limitMessageCount = 0;

//Auto Variables
static bool s_autoModeSingle = false;
static int s_nextAutoUpdate = 0;



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

      s_limitEarliestMessage = 0;
      s_limitLatestMessage = 0;
      s_limitMessageCount = 0;
    }

    void save()
    {
      if((s_limitLatestMessage - s_limitEarliestMessage) < 240000 ) return;
      if(s_limitMessageCount < 5 ) return;

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
      using namespace dro::network::metadata;
      const MessageMetadata message = message::recentMessage();
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;

      if(s_limitEarliestMessage == 0) s_limitEarliestMessage = timestampElapsed;
      s_limitLatestMessage = timestampElapsed;
      if(message.textContent.trimmed().length() > 3) s_limitMessageCount += 1;


      ReplayOperation lNewOperation = {"msg", timestampElapsed, {}};

      lNewOperation.variables["pre"] = message.characterPre;
      lNewOperation.variables["char"] = message.characterFolder;
      lNewOperation.variables["emote"] = message.characterEmote;
      lNewOperation.variables["outfit"] = message.characterOutfit;
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

      if(message::pair::isActive())
      {
        lNewOperation.variables["pair_char"] = message::pair::getCharacter();
        lNewOperation.variables["pair_emote"] = message::pair::getEmote();
        lNewOperation.variables["pair_outfit"] = message::pair::getOutfit();
        lNewOperation.variables["pair_sequence"] = message::pair::getAnimation();
        lNewOperation.variables["pair_layers"] = message::pair::getLayers();
        lNewOperation.variables["pair_horizontal"] =  QString::number(message::pair::horizontalOffset());
        lNewOperation.variables["pair_vertical"] =  QString::number(message::pair::verticalOffset());
        lNewOperation.variables["pair_scale"] =  QString::number(message::pair::scaleValue());
        lNewOperation.variables["pair_leader"] =  QString::number(message::pair::isLeader());
        lNewOperation.variables["pair_flipped"] =  QString::number(message::pair::isFlipped());
        lNewOperation.variables["pair_visible"] =  QString::number(message::pair::isVisible());
      }

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

    void weatherChange(QString name, QString environment)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation weatherOperation = {"weather", timestampElapsed, {}};

      weatherOperation.variables["name"] = name;
      weatherOperation.variables["environment"] = environment;
      s_recordingOperations.append(weatherOperation);
      save();
    }

    void splashAnimation(const QString &splash)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation splashOperation = {"wtce", timestampElapsed, {}};
      splashOperation.variables["name"] = splash;

      s_recordingOperations.append(splashOperation);
      save();
    }

    void gamemodeChange(const QString &mode)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation modeOperation = {"gamemode", timestampElapsed, {}};
      modeOperation.variables["name"] = mode;

      s_recordingOperations.append(modeOperation);
      save();
    }

    void hourChange(const QString &hour)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation hourOperation = {"clock", timestampElapsed, {}};
      hourOperation.variables["hour"] = hour;

      s_recordingOperations.append(hourOperation);
      save();
    }

    void todChange(const QString &timeOfDay)
    {
      const int timestampElapsed = RuntimeLoop::uptime() - s_recordingStartTime;
      ReplayOperation periodOperation = {"period", timestampElapsed, {}};
      periodOperation.variables["value"] = timeOfDay;

      s_recordingOperations.append(periodOperation);
      save();
    }

  }

  namespace playback
  {
    using namespace dro::network::metadata;
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

    void autoUpdate(const int &uptime)
    {
      if(s_nextAutoUpdate == 0) return;
      if(uptime < s_nextAutoUpdate) return;
      s_nextAutoUpdate = 0;
      if(s_autoModeSingle)
        progressSingle();
      else
        progress();
    }

    void progressSingle()
    {
      if(!s_replayViewport) return;
      if(!s_replayWindow) return;

      if(s_playbackOperations.count() == s_playbackTimestamp + 1) return;
      s_autoModeSingle = true;
      const ReplayOperation &currentOperation = s_playbackOperations[s_playbackTimestamp];
      s_playbackTimestamp += 1;
      const ReplayOperation &nextOperation = s_playbackOperations[s_playbackTimestamp];

      if(nextOperation.operation == "msg")
      {
        message::incomingMessage(s_playbackOperations[s_playbackTimestamp]);
        s_replayViewport->loadCurrentMessage();
      }

      if(nextOperation.operation == "bgm")
        audio::bgm::Play(s_playbackOperations[s_playbackTimestamp].variables["track"].toStdString());

      if(nextOperation.operation == "bg")
        s_replayViewport->loadBackground(s_playbackOperations[s_playbackTimestamp].variables["name"]);

      s_replayWindow->setScrubberPosition(s_playbackTimestamp);

      s_nextAutoUpdate = RuntimeLoop::uptime() + (nextOperation.timestamp - currentOperation.timestamp);
    }

    void setNextUpdate(int nextUpdate)
    {
      s_autoModeSingle = false;
      if(nextUpdate == 0)
        s_nextAutoUpdate = 0;
      else
        s_nextAutoUpdate = nextUpdate + RuntimeLoop::uptime();
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
