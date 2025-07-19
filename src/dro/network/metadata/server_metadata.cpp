#include "server_metadata.h"
#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "dro/fs/fs_reading.h"
#include "commondefs.h"
#include <QDebug>

QStringList s_featureList = {};

QString s_lastUsedFilter = "Server Characters";
QVector<char_type> s_characterListFiltered;
QVector<char_type> s_characterListServer;
QVector<char_type> s_characterListFavorites;

QHash<QString, QVector<char_type>> s_characterListRepository;

QHash<QString, bool> s_characterClaimList = {};
QStringList s_characterFilterList = {"Server Characters", "Favorites", "All"};


void ServerMetadata::SetFeatureList(QStringList features)
{
  s_featureList.clear();
  s_featureList = features;
}

bool ServerMetadata::FeatureSupported(QString featureName)
{
  return s_featureList.contains(featureName);
}

void dro::network::metadata::character::lists::reset()
{
  s_lastUsedFilter = "Server Characters";
  s_characterListRepository = {};
  s_characterFilterList.clear();
  s_characterFilterList.append({"Server Characters", "Favorites", "All"});
}

void dro::network::metadata::character::lists::addFavorite(const QString &folder)
{
  for (int j = 0; j < s_characterListFavorites.size(); j++)
  {
    if (folder == s_characterListFavorites.at(j).name)
    {
      return;
    }
  }
  char_type ct;
  ct.name = folder;
  s_characterListFavorites.append(ct);
  saveFavorites();
}

void dro::network::metadata::character::lists::removeFavorite(const QString &folder)
{
  for (int j = 0; j < s_characterListFavorites.size(); j++)
  {
    if (folder == s_characterListFavorites.at(j).name)
    {
      s_characterListFavorites.removeAt(j);
      return;
    }
  }
  saveFavorites();
}

void dro::network::metadata::character::lists::loadFavorites()
{
  s_characterListFavorites.clear();
  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Failed to open file for reading";
    return;
  }

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QString characterName = in.readLine().trimmed();

    if (!characterName.isEmpty())
    {
      char_type character;
      character.name = characterName;
      s_characterListFavorites.append(character);
    }
  }

  file.close();
}

void dro::network::metadata::character::lists::saveFavorites()
{
  QStringList favoritesList = {};
  for (int j = 0; j < s_characterListFavorites.size(); j++)
  {
    favoritesList.append(s_characterListFavorites[j].name);
  }

  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&file);
    for (const QString &character : favoritesList)
    {
      out << character << "\n";
    }
    file.close();
  }
  else
  {
    qDebug() << "Save Favorites Failed";
  }
}

void dro::network::metadata::character::lists::setServerList(QVector<char_type> list)
{
  s_characterListServer = list;
}

void dro::network::metadata::character::lists::setFilteredList(const QString &package, QVector<char_type> list)
{
  if(!s_characterFilterList.contains(package)) s_characterFilterList.append(package);
  s_characterListRepository[package] = list;
}

void dro::network::metadata::character::lists::setCharacterAvailability(int id, bool status)
{
  s_characterClaimList[s_characterListServer.at(id).name] = status;
}

int dro::network::metadata::character::lists::findAvailablePersona()
{
  for(int i = 0; i <= 25; i++)
  {
    QString personaName = QString("Persona" + QString::number(i));
    if(s_characterClaimList.contains(personaName))
    {
      if(!s_characterClaimList[personaName])
      {
        for (int j = 0; j < s_characterListServer.size(); j++)
        {
          if (s_characterListServer[j].name == personaName)
          {
            return j;
          }
        }

      }
    }
  }

  return -1;
}

bool dro::network::metadata::character::lists::characterCheck(const QString &name)
{
  for (int j = 0; j < s_characterListServer.size(); j++)
  {
    if (s_characterListServer[j].name == name)
    {
      return true;
    }
  }

  return false;
}

bool dro::network::metadata::character::lists::characterCheck(int filterID)
{
  for (int j = 0; j < s_characterListServer.size(); j++)
  {
    if (s_characterListServer[j].name == s_characterListFiltered.at(filterID).name)
    {
      return true;
    }
  }

  return false;
}

QStringList dro::network::metadata::character::lists::characterFilters()
{
  return s_characterFilterList;
}

QVector<char_type> dro::network::metadata::character::lists::serverList()
{
  return s_characterListServer;
}

QVector<char_type> dro::network::metadata::character::lists::filteredList(QString package)
{
  s_lastUsedFilter = package;
  if(package == "Server Characters") return s_characterListServer;
  if(package == "Favorites") return s_characterListFavorites;

  if(package == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = s_characterListRepository.constBegin(); i != s_characterListRepository.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(s_characterListRepository.contains(package))
  {
    return s_characterListRepository[package];
  }
  return QVector<char_type>();
}

QVector<char_type> dro::network::metadata::character::lists::currentList()
{
  if(s_lastUsedFilter == "Server Characters") return s_characterListServer;
  if(s_lastUsedFilter == "Favorites") return s_characterListFavorites;

  if(s_lastUsedFilter == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = s_characterListRepository.constBegin(); i != s_characterListRepository.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(s_characterListRepository.contains(s_lastUsedFilter))
  {
    return s_characterListRepository[s_lastUsedFilter];
  }

  return s_characterListServer;
}

QVector<char_type> dro::network::metadata::character::lists::resetClaimed()
{
  s_characterClaimList = {};
  return s_characterListServer;
}

QString dro::network::metadata::character::lists::characterNameFiltered(int id)
{
  return s_characterListFiltered.at(id).name;
}

QString dro::network::metadata::character::lists::characterNameServer(int id)
{
  return s_characterListServer.at(id).name;
}

int dro::network::metadata::character::lists::characterServerId(int Id)
{
  for (int j = 0; j < s_characterListServer.size(); j++)
  {
    if (s_characterListServer[j].name == s_characterListFiltered.at(Id).name)
    {
      return j;
    }
  }

  return -1;
}

int dro::network::metadata::character::lists::characterServerId(QString name)
{
  for (int j = 0; j < s_characterListServer.size(); j++)
  {
    if (s_characterListServer[j].name == name)
    {
      return j;
    }
  }

  return -1;
}

void dro::network::metadata::character::lists::clearFlitered()
{
  s_characterListFiltered.clear();
}

void dro::network::metadata::character::lists::addFlitered(const char_type &character)
{
  s_characterListFiltered.append(character);
}

char_type dro::network::metadata::character::lists::filteredCharacter(int id)
{
  if(id > s_characterListFiltered.count()) return {};
  s_characterListFiltered.at(id);
}

int dro::network::metadata::character::lists::lengthServerList()
{
  return s_characterListServer.length();
}

bool dro::network::metadata::character::lists::characterTaken(const QString &name)
{
  if(!s_characterClaimList.contains(name)) return false;
  return s_characterClaimList[name];
}
