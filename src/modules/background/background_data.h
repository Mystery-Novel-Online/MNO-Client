#ifndef BACKGROUNDDATA_H
#define BACKGROUNDDATA_H

#include "datatypes.h"

class BackgroundData
{
public:
  BackgroundData();

  void setPosition(QString t_position, DRBackgroundPosition t_data);
  QString getBackgroundFilename(QString t_position);
  QString getForegroundFilename(QString t_position);
  virtual int getVerticalPosition(double t_height) = 0;
  void setSettings(DRBackgroundSettings t_settings);
  DRBackgroundSettings getSettings();


  virtual void execLoadBackground(QString t_backgroundName) = 0;

private:
  DRBackgroundSettings mSettings;
  QMap<QString, DRBackgroundPosition> mPositions = {};
};

#endif // BACKGROUNDDATA_H
