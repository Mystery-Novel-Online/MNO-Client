#include "aoapplication.h"

#include "aoconfig.h"
#include "commondefs.h"
#include "courtroom.h"
#include "drpather.h"
#include "file_functions.h"
#include "modules/managers/character_manager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>

#include <modules/theme/thememanager.h>

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
  CharacterManager::get().ResetPackages();
  package_names = {};

  QString packages_path = DRPather::get_application_path() + "/packages/";
  QDir packages_directory(packages_path);

  QList<QFileInfo> packages_fileinfo= packages_directory.entryInfoList();


  for (int i=0; i< packages_fileinfo.size(); i++)
  {
    if (packages_fileinfo.at(i).isDir())
    {
      QString packageBaseName = packages_fileinfo.at(i).baseName();
      if(!packageBaseName.isEmpty())
      {
        package_names.append(packageBaseName);

        QDir package_dir(packages_fileinfo.at(i).absoluteFilePath() + "/characters");
        if (package_dir.exists())
        {
          QVector<char_type> packageCharacters;
          QStringList character_folders = package_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
          for (const QString &character_folder : character_folders)
          {
            char_type packageChar;
            packageChar.name = character_folder;
            packageCharacters.append(std::move(packageChar));
          }
          CharacterManager::get().SetCharList(packageBaseName, packageCharacters);
        }

      }
    }
  }

  read_disabled_packages_ini();
}


void AOApplication::save_disabled_packages_ini()
{
  const QString l_ini_path = get_base_path() + BASE_PACKAGES_INI;
  QFile l_packages_ini(l_ini_path);
  l_packages_ini.open(QIODevice::WriteOnly);
  QTextStream out(&l_packages_ini);

  l_packages_ini.resize(0);

  for (int i=0; i< m_disabled_packages.size(); i++)
  {
    out << m_disabled_packages[i] << "\r\n";
  }

  l_packages_ini.close();


}
void AOApplication::read_disabled_packages_ini()
{
  m_disabled_packages = {};

  const QString l_ini_path = get_base_path() + BASE_PACKAGES_INI;
  QFile l_packages_ini(l_ini_path);
  if (l_packages_ini.open(QFile::ReadOnly))
  {
    QTextStream in(&l_packages_ini);
    while (!in.atEnd())
    {
      QString l_line = in.readLine().trimmed();
      if(package_names.contains(l_line)) m_disabled_packages.append(l_line);
    }
  }
}

QString AOApplication::get_base_path()
{
  return DRPather::get_application_path() + "/base/";
}

QString AOApplication::get_package_path(QString p_package)
{
  return DRPather::get_application_path() + "/packages/" + p_package + "/";
}

QString AOApplication::get_package_or_base_path(QString p_path)
{

  for (int i=0; i< package_names.size(); i++)
  {
    if(!m_disabled_packages.contains(package_names.at(i)))
    {
      QString package_path = get_package_path(package_names.at(i))  + p_path;
      if(dir_exists(package_path))
      {
        return package_path;
      }
    }

  }

  return get_base_path() + p_path;
}

QString AOApplication::get_package_or_base_file(QString p_filepath)
{
  for (int i=0; i< package_names.size(); i++)
  {
    if(!m_disabled_packages.contains(package_names.at(i)))
    {
      QString package_path = get_package_path(package_names.at(i))  + p_filepath;
      if(file_exists(package_path))
      {
        return package_path;
      }
    }
  }

  return get_base_path() + p_filepath;
}



QString AOApplication::get_base_file_path(QString p_file)
{
  return get_package_or_base_path(p_file);
}

QVector<QString> AOApplication::get_all_package_and_base_paths(QString p_path)
{
  QVector<QString> found_paths;

  for (int i=0; i< package_names.size(); i++)
  {
    if(!m_disabled_packages.contains(package_names.at(i)))
    {
      QString package_path = get_package_path(package_names.at(i))  + p_path;
      if(dir_exists(package_path))
      {
        found_paths.append(package_path);
      }
    }
  }

  found_paths.append(get_base_path() + p_path);


  return found_paths;
}



QString AOApplication::get_character_folder_path(QString p_chr)
{
  return get_package_or_base_path("characters/" + p_chr);
}

QString AOApplication::get_character_path(QString p_chr, QString p_file)
{
  return get_character_folder_path(p_chr) + "/" + p_file;
}

QString AOApplication::get_music_folder_path()
{
  const QString l_path = get_base_path() + "sounds/music/";
  return get_case_sensitive_path(l_path);
}

QString AOApplication::get_music_path(QString p_song)
{
  return get_case_sensitive_path(get_package_or_base_file("sounds/music/" + p_song));
}

QString AOApplication::get_background_path(QString p_identifier)
{
  return get_package_or_base_path("background/" + p_identifier);
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
      if (file_exists(l_file_path))
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
  const QString l_gamemode =
      ao_config->is_manual_gamemode_selection_enabled() ? ao_config->manual_gamemode() : ao_config->gamemode();
  const QString l_timeofday =
      ao_config->is_manual_timeofday_selection_enabled() ? ao_config->manual_timeofday() : ao_config->timeofday();
  const QString l_theme_root = get_package_or_base_path("themes/" + ao_config->theme());

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
  const QString l_default_theme_path = get_base_path() + "themes/default/";
  if (dir_exists(l_default_theme_path))
    l_path_list.append(l_default_theme_path + p_file);

  return find_asset_path(l_path_list, p_extension_list);
}

QString AOApplication::find_theme_asset_path(QString p_file)
{
  return find_theme_asset_path(p_file, QStringList{""});
}

QString AOApplication::find_current_theme_path()
{
  return get_package_or_base_path("themes/" + ao_config->theme());
}

QString AOApplication::getCurrentTheme()
{
  return ao_config->theme();
}

QString AOApplication::getCurrentGamemode()
{
  return ao_config->is_manual_gamemode_selection_enabled() ? ao_config->manual_gamemode() : ao_config->gamemode();
}

QString AOApplication::getCurrentTime()
{
  return ao_config->is_manual_timeofday_selection_enabled() ? ao_config->manual_timeofday() : ao_config->timeofday();
}

