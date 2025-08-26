#include "pch.h"

#include "aoconfig.h"

#include <modules/theme/thememanager.h>
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_mounting.h"
#include "engine/system/replay_playback.h"
#include "engine/network/metadata/server_metadata.h"
#include <rolechat/filesystem/RCDir.h>
#include <rolechat/filesystem/RCFile.h>

// Copied over from Vanilla.
// As said in the comments there, this is a *super broad* definition.
// Linux is guaranteed to be case-sensitive, however, in case of Mac, it...
// depends. On Mac, it can be toggled. So, should the time ever come to that,
// manually define CASE_SENSITIVE_FILESYSTEM if you're working on a Mac that
// has, well, a case-sensitive filesystem.
#ifdef Q_OS_LINUX
#define CASE_SENSITIVE_FILESYSTEM
#endif

void AOApplication::reload_packages()
{
  // Reset cached data
  CharacterRepository::reset();
  engine::system::replays::io::resetCache();

  QVector<QString> packageNames = FS::Packages::Scan();
  QString packagesPath = FS::Paths::ApplicationPath() + "/packages/";

  QDir baseCharactersDir (FS::Paths::BasePath() + "/characters");
  if (baseCharactersDir.exists())
  {
    QVector<ActorSelectEntry> baseCharacters;
    QStringList character_folders = baseCharactersDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &character_folder : character_folders)
    {
      ActorSelectEntry packageChar;
      packageChar.name = character_folder.toStdString();
      baseCharacters.append(std::move(packageChar));
    }
    CharacterRepository::setFilteredList("base", baseCharacters);
  }

  for(QString packageName : packageNames)
  {
    QDir charactersPath (packagesPath + packageName + "/characters");
    if (charactersPath.exists())
    {
      QVector<ActorSelectEntry> packageCharacters;
      QStringList character_folders = charactersPath.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
      for (const QString &character_folder : character_folders)
      {
        ActorSelectEntry packageChar;
        packageChar.name = character_folder.toStdString();
        packageCharacters.append(std::move(packageChar));
      }
      CharacterRepository::setFilteredList(packageName, packageCharacters);
    }

    const QDir replaysDir(packagesPath + packageName + "/replays");
    if (replaysDir.exists())
    {
      const QStringList replayFolders = replaysDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
      engine::system::replays::io::cachePackage(packageName, replayFolders);
    }
  }

}

QVector<QString> AOApplication::get_all_package_and_base_paths(QString p_path)
{
  QVector<QString> found_paths;
  QVector<QString> disabledList = FS::Packages::DisabledList();
  QVector<QString> packageNames = FS::Packages::CachedNames();

  for (int i=0; i< packageNames.size(); i++)
  {
    if(!disabledList.contains(packageNames.at(i)))
    {
      QString package_path = FS::Paths::Package(packageNames.at(i))  + p_path;
      if(FS::Checks::DirectoryExists(package_path))
      {
        found_paths.append(package_path);
      }
    }
  }

  found_paths.append(FS::Paths::BasePath() + p_path);


  return found_paths;
}

QString AOApplication::get_music_path(QString p_song)
{
  fs::RCFile musicFile("sounds/music/" + p_song.toStdString());
  return get_case_sensitive_path(musicFile.findFirst());
}

QString AOApplication::get_background_path(QString p_identifier)
{
  return QString::fromStdString(fs::RCDir("background/" + p_identifier.toStdString()).findFirst());
}

QString AOApplication::get_background_dir_path(QString p_identifier)
{
  return get_case_sensitive_path(get_background_path(p_identifier));
}

/**
 * @brief Returns the 'correct' path for the file given as the parameter by
 * trying to match the case of the actual path.
 *
 * @details This function is mostly used on case-sensitive file systems, like
 * ext4, generally used on Linux. On FAT, there is no difference between
 * "file" and "FILE". On ext4, those are two different files. This results in
 * assets that are detected correctly on Windows not being detected on Linux.
 *
 * For this reason, the implementation of this function is system-dependent:
 * on case-insensitive systems, it just returns the parameter itself.
 *
 * @param p_file The path whose casing must be checked against the actual
 * directory structure.
 *
 * @return The parameter path with fixed casing.
 */

QString AOApplication::get_case_sensitive_path(const std::string &p_file)
{
  return get_case_sensitive_path(QString::fromStdString(p_file));
}

QString AOApplication::get_case_sensitive_path(QString p_file)
#ifndef CASE_SENSITIVE_FILESYSTEM
{
  return p_file.replace("//", "/");
}
#else
{
  if (QFile::exists(p_file) || p_file.isEmpty())
  {
    return p_file;
  }

  const auto l_dir_path = get_case_sensitive_path(QFileInfo(p_file).absolutePath());
  if (l_dir_path.isEmpty())
  {
    return p_file;
  }

  const QDir l_dir(l_dir_path);
  const auto l_file_list = l_dir.entryList(QDir::Files);
  const auto l_regex = QRegExp(p_file, Qt::CaseInsensitive, QRegExp::FixedString);
  for (auto &i_file : l_file_list)
  {
    const QString l_file_path = l_dir.absoluteFilePath(i_file);
    if (l_regex.exactMatch(l_file_path))
    {
      p_file = l_file_path;
      break;
    }
  }
  return p_file;
}
#endif

/**
 * @brief Returns the first case-sensitive file that is the combination of one
 * of the given root and extensions, or empty string if no combination exists.
 *
 * @details A root is matched to all given extensions in order before
 * continuing to the next root.
 *
 * @param possible_roots The potential roots the filepath could have.
 * Case-insensitive.
 * @param possible_exts The potential extensions the filepath could have.
 * Case-insensitive.
 *
 * @return The first case-sensitive root+extension path for which a file
 * exists, or an empty string, if not one does.
 */
QString AOApplication::find_asset_path(QStringList p_file_list, QStringList p_extension_list)
{
  for (const QString &i_file : qAsConst(p_file_list))
  {
    const QDir l_dir(get_case_sensitive_path(QFileInfo(i_file).absolutePath()));

    for (const QString &i_extension : qAsConst(p_extension_list))
    {
      const QString l_file_path = get_case_sensitive_path(l_dir.filePath(i_file + i_extension));
      if (FS::Checks::FileExists(l_file_path))
      {
        return l_file_path;
      }
    }
  }

  return nullptr;
}

QString AOApplication::find_asset_path(QStringList p_file_list)
{
  return find_asset_path(p_file_list, QStringList{""});
}

QString AOApplication::find_asset_path(QString p_file, QStringList p_extension_list)
{
  return find_asset_path(QStringList{p_file}, p_extension_list);
}

QString AOApplication::find_asset_path(QString p_file)
{
  return find_asset_path(QStringList{p_file});
}

/**
 * @brief Returns the first case-sensitive file in the theme folder that is
 * of the form name+extension, or empty string if it fails.
 *
 * @details The p_exts list is browsed in order. A name+extension file is
 * searched in order in the following directories before checking the next
 * extension:
 * 1. The current time of day folder in the current gamemode folder
 * 2. The current gamemode folder
 * 3. The current time of day folder
 * 4. The current theme folder.
 * The first path that is matched is the one that is returned. If no file
 * is found at all, it returns an empty string.
 *
 * @param p_name Name of the file to look for. Case-insensitive.
 * @param p_exts The potential extensions the filepath could have.
 * Case-insensitive.
 *
 * @return The first case-sensitive root+extension path that corresponds to an
 * actual file, or an empty string, if not one does.
 */
QString AOApplication::find_theme_asset_path(QString p_file, QStringList p_extension_list)
{
  QStringList themeManagerDirs = ThemeManager::get().mCurrentThemeReader.getThemeDirOrder();

  QStringList l_path_list = {};

  for(QString path : themeManagerDirs)
  {
    l_path_list.append(path + "/" + p_file);
  }

  // Only add gamemode and/or time of day if non empty.
  const QString l_gamemode = system::ConfigManager::gamemode();
  const QString l_timeofday = system::ConfigManager::timeOfDay();
  const QString l_theme_root = FS::Paths::FindDirectory("themes/" + QString::fromStdString(config::ConfigUserSettings::stringValue("theme", "default")));

  if (!l_gamemode.isEmpty())
  {
    if (!l_timeofday.isEmpty())
      l_path_list.append(l_theme_root + "/gamemodes/" + l_gamemode + "/times/" + l_timeofday + "/" + p_file);
    l_path_list.append(l_theme_root + "/gamemodes/" + l_gamemode + "/" + p_file);
  }

  if (!l_timeofday.isEmpty())
    l_path_list.append(l_theme_root + "/times/" + l_timeofday + "/" + p_file);

  l_path_list.append(l_theme_root + "/" + p_file);

  // Check if default folder exists. We do this here as it is cheaper than doing it in find_asset_path
  // (as we know there should not be capitalization or folder jumping shenanigans here.
  const QString l_default_theme_path = FS::Paths::BasePath() + "themes/default/";
  if (FS::Checks::DirectoryExists(l_default_theme_path))
    l_path_list.append(l_default_theme_path + p_file);

  return find_asset_path(l_path_list, p_extension_list);
}

QString AOApplication::find_theme_asset_path(QString p_file)
{
  return find_theme_asset_path(p_file, QStringList{""});
}

QString AOApplication::find_current_theme_path()
{
  return FS::Paths::FindDirectory("themes/" + QString::fromStdString(config::ConfigUserSettings::stringValue("theme", "default")));
}

QString AOApplication::getCurrentTheme()
{
  return QString::fromStdString(config::ConfigUserSettings::stringValue("theme", "default"));
}

