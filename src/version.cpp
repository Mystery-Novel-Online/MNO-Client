#include "version.h"
#include "pch.h"

int get_release_version()
{
  return 1;
}

int get_major_version()
{
  return 9;
}

int get_minor_version()
{
  return 0;
}

VersionNumber get_version_number(int baseVersion)
{
  int releaseBase = 0;
  int majorBase = 0;
  int minorBase = 0;

  const QString baseVersionString = QString::number(baseVersion);
  if(baseVersionString.length() >= 6 && !baseVersionString.startsWith("-"))
  {
    releaseBase = QString(baseVersionString.at(3)).toInt();
    majorBase = QString(baseVersionString.at(1)).toInt();
    minorBase = QString(baseVersionString.at(5)).toInt();
  }

  return VersionNumber(get_release_version() + releaseBase, get_major_version() + majorBase, get_minor_version() + minorBase);
}

QString get_post_version()
{
  return "b02";
}

QString get_version_string(int baseVersion)
{
  QString l_version = get_version_number(baseVersion).to_string() + get_post_version();
  return l_version;
}

QString get_resource_file_text(QString filename)
{
  QString data;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
  {
    return "";
  }
  data = file.readAll();
  file.close();
  return data;
}

QString build_about_message()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");
  const QString git_branch = get_resource_file_text(":/res/git/git_branch.txt");
  const QString git_hash = get_resource_file_text(":/res/git/git_hash.txt");

  QString msg = QString("<h2>Mystery Novel Online</h2>"
                        "version: %1"
                        "<p><b>Source code:</b> "
                        "<a href='https://github.com/Mystery-Novel-Online/MNO-Client'>"
                        "https://github.com/Mystery-Novel-Online/MNO-Client</a>"
                        "<p><b>Development:</b><br>"
                        "Cerapter, Elf, Iuvee, Tricky Leifa, Keightiie"
                        "<p>Based on Attorney Online 2:<br>"
                        "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
                        "https://github.com/AttorneyOnline/AO2-Client</a>"
                        "<p>Running on Qt version %2 with the BASS %3 audio engine.<br>"
                        "APNG plugin loaded: %4"
                        "<p>Built on %5 %6<br>")
                    .arg(get_version_string())
                    .arg(QLatin1String(QT_VERSION_STR))
                    .arg(QLatin1String(BASSVERSIONTEXT))
                    .arg(hasApng ? "Yes" : "No")
                    .arg(QLatin1String(__DATE__))
                    .arg(QLatin1String(__TIME__));

  if (git_branch.isEmpty())
    msg += QString("No git branch information available.<br>");
  else
    msg += QString("Built from git branch %1<br>").arg(git_branch);

  if (git_hash.isEmpty())
    msg += QString("No git hash information available.");
  else
    msg += QString("Hash of the latest commit: %1").arg(git_hash);

  return msg;
}

