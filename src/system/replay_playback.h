#ifndef REPLAY_PLAYBACK_H
#define REPLAY_PLAYBACK_H

class ReplayWindow;
class RPViewport;
class QStringList;
class QString;

namespace engine::system::replays
{
  void assignWindow(ReplayWindow *window);
  void assignViewport(RPViewport *viewport);

  namespace recording
  {
    void start();

    void musicChange(const QString& music);
    void backgroundChange(const QString& background, const QString& variant = "default");
    void messageCharacter();
    void messageSystem(const QString& name, const QString& message);
    void weatherChange(const QString& name, const QString& environment);
    void splashAnimation(const QString &splash);
    void gamemodeChange(const QString &mode);
    void hourChange(const QString &hour);
    void todChange(const QString &timeOfDay);

    void save();
  }

  namespace playback
  {
    void loadFile(const QString& name);
    void load(const QString &name, const QString &package, const QString &category);
    void setNextUpdate(int nextUpdate);
    void autoUpdate(const int &uptime);
    void progress();
    void progressSingle();
    void setTimestamp(int index);
    void stop();
  }

  namespace io
  {
    void resetCache();
    void cachePackage(const QString& package, QStringList categories);
    QStringList packageNames();
    QStringList packageCategories(const QString& package);
    QStringList packageContents(const QString& package, const QString& category);

  }
}

#endif // REPLAY_PLAYBACK_H
