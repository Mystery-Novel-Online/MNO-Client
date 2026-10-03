#include "fs_mounting.h"
#include "fs_reading.h"

static QVector<QString> s_foundPackages = {};
static QVector<QString> s_disabledPackages = {};

void FS::Packages::SetDisabled(QVector<QString> disableList)
{
  s_disabledPackages.clear();
  s_disabledPackages = disableList;
  SaveDisabled();
}

void FS::Packages::SaveDisabled()
{
  const QString iniPath = Paths::BasePath() + "packages.ini";
  QFile iniFile(iniPath);
  iniFile.open(QIODevice::WriteOnly);
  QTextStream out(&iniFile);

  iniFile.resize(0);

  for(int i=0; i< s_disabledPackages.size(); i++)
  {
    out << s_disabledPackages[i] << "\r\n";
  }

  iniFile.close();

}
