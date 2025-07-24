#ifndef BACKGROUNDDATA_H
#define BACKGROUNDDATA_H



class BackgroundData
{
public:
  BackgroundData();

  void setPosition(QString t_position, DRBackgroundPosition t_data);
  QString getBackgroundFilename(QString t_position);
  QString getForegroundFilename(QString t_position);
  void setSettings(DRBackgroundSettings t_settings);
  DRBackgroundSettings getSettings();


  virtual void execLoadBackground(QString t_backgroundName) = 0;

private:
  DRBackgroundSettings mSettings;
  QMap<QString, DRBackgroundPosition> mPositions = {};
};

#endif // BACKGROUNDDATA_H
