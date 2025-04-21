#include "version.h"

#include <QFile>
#include <QImageReader>
#include <QString>
#include <qprocess.h>

#include <bass/bass.h>

#include "datatypes.h"
#include "drpather.h"
#include "file_functions.h"

int get_release_version()
{
  return 1;
}

int get_major_version()
{
  return 7;
}

int get_minor_version()
{
  return 0;
}

VersionNumber get_version_number()
{
  return VersionNumber(get_release_version(), get_major_version(), get_minor_version());
}

QString get_post_version()
{
  return "a01";
}

QString get_version_string()
{
  QString l_version = get_version_number().to_string();

  const QString l_post = get_post_version();
  if (!l_post.isEmpty())
  {
    l_version += l_post;
  }

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

QString get_about_message()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");
  const QString git_branch = get_resource_file_text(":/res/git/git_branch.txt");
  const QString git_hash = get_resource_file_text(":/res/git/git_hash.txt");

  QString msg = QString("<h2>Danganronpa Online</h2>"
                        "version: %1"
                        "<p><b>Source code:</b> "
                        "<a href='https://github.com/Chrezm/DRO-Client'>"
                        "https://github.com/Chrezm/DRO-Client</a>"
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

void launch_updater_check(bool is_beta, bool isSilent)
{
  qDebug() << "Launching updater...";
  QString program = "./updater.exe";
  QString new_program = "./updater.exe.new";
  if(file_exists(new_program))
  {
    QFile::remove(program);
    QFile::rename(new_program, program);
  }

  if(!file_exists(program)) return;

  QStringList arguments;
  arguments << "--hide" << "--check";

  if(isSilent) arguments << "--silent";

  if(is_beta) arguments << "--beta";
  else arguments << "--stable";

  QProcess *myProcess = new QProcess();
  myProcess->startDetached(program, arguments);
}

bool check_updater_is_beta()
{
  QString version = "stable";
  QString version_file_path = DRPather::get_application_path() + "/base/version";
  if(file_exists(version_file_path))
  {
    QFile inputFile(version_file_path);
    if (inputFile.open(QIODevice::ReadOnly))
    {
      QTextStream in(&inputFile);
      if(!in.atEnd())
      {
        version = in.readLine();
      }
      inputFile.close();
    }
  }

  return version.toLower() == "beta";
}

