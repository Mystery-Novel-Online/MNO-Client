#ifndef REPLAY_PLAYBACK_H
#define REPLAY_PLAYBACK_H

class ReplayWindow;
class RPViewport;
class QStringList;
class QString;

namespace dro::system::replays
{
  void assignWindow(ReplayWindow *window);
  void assignViewport(RPViewport *viewport);

  namespace recording
  {
    void start();

    void musicChange(QString music);
    void backgroundChange(QString background);
    void messageCharacter();
    void messageSystem(QString name, QString message);
    void splashAnimation();
    void weatherChange();
    void gamemodeChange();
    void hourChange();
    void todChange();

    void save();
  }

  namespace playback
  {
    void loadFile(QString name);
    void load(const QString &name, const QString &package, const QString &category);
    void progress();
    void setTimestamp(int index);
    void stop();
  }

  namespace io
  {
    void resetCache();
    void cachePackage(QString package, QStringList categories);
    QStringList packageNames();
    QStringList packageCategories(QString package);
    QStringList packageContents(QString package, QString category);

  }
}

#endif // REPLAY_PLAYBACK_H
