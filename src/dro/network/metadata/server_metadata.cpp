#include "server_metadata.h"
#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "dro/fs/fs_reading.h"
#include "commondefs.h"
#include <QDebug>

using namespace dro::network::metadata;

void ServerInformation::setFeatureList(QStringList features)
{
  featureList.clear();
  featureList = features;
}

bool ServerInformation::featureSupported(const QString &name)
{
  return featureList.contains(name);
}

void CharacterRepository::reset()
{
  lastUsedFilter = defaultFilters[0];
  repository.clear();
}

void CharacterRepository::addFavorite(const QString &folder)
{
  if (std::any_of(favoriteCharacters.begin(), favoriteCharacters.end(), [&](const char_type& c){ return c.name == folder; }))
    return;
  favoriteCharacters.append({folder});
  saveFavorites();
}

void CharacterRepository::removeFavorite(const QString &folder)
{
  auto it = std::remove_if(favoriteCharacters.begin(), favoriteCharacters.end(), [&](const char_type& c){ return c.name == folder; });
  if (it != favoriteCharacters.end()) {
    favoriteCharacters.erase(it, favoriteCharacters.end());
    saveFavorites();
  }
}

void CharacterRepository::loadFavorites()
{
  favoriteCharacters.clear();

  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open favorites file.";
    return;
  }

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (!line.isEmpty())
      favoriteCharacters.append({line});
  }
}

void CharacterRepository::saveFavorites()
{
  QFile file(FS::Paths::FindFile(SAVE_FAVORITES, false));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to save favorites.";
    return;
  }

  QTextStream out(&file);
  for (const auto& character : favoriteCharacters)
    out << character.name << '\n';
}

void CharacterRepository::setCharacterAvailability(int index, bool available)
{
  if (index >= 0 && index < serverCharacters.size())
    claimedCharacters[serverCharacters.at(index).name] = available;
}

bool CharacterRepository::isCharacterAvailable(const QString &name)
{
    return claimedCharacters.value(name, false);
}

void CharacterRepository::setServerList(const QVector<char_type> &list)
{
  serverCharacters = list;
}

void CharacterRepository::setFilteredList(const QString &name, const QVector<char_type> &list)
{
  if (!defaultFilters.contains(name))
    repository[name] = list;
}

bool CharacterRepository::characterExists(const QString &name)
{
  return std::any_of(serverCharacters.begin(), serverCharacters.end(), [&](const char_type& c) { return c.name == name; });
}

bool CharacterRepository::filteredCharacterExists(int filterIndex)
{
  if (filterIndex < 0 || filterIndex >= filteredCharacters.size())
    return false;

  return characterExists(filteredCharacters.at(filterIndex).name);
}

void CharacterRepository::clearFiltered()
{
  filteredCharacters.clear();
}

void CharacterRepository::addFiltered(const char_type &character)
{
  filteredCharacters.append(character);
}

char_type CharacterRepository::filteredCharacter(int index)
{
  if(index > filteredCharacters.count()) return {};
  return filteredCharacters.at(index);
}

QString CharacterRepository::characterNameFiltered(int index)
{
  return (index >= 0 && index < filteredCharacters.size()) ? filteredCharacters.at(index).name : "";
}

QString CharacterRepository::characterNameServer(int index)
{
  return (index >= 0 && index < serverCharacters.size()) ? serverCharacters.at(index).name : "";
}

int CharacterRepository::findAvailablePersona()
{
  for (int i = 0; i <= 25; ++i) {
    QString personaName = QString("Persona%1").arg(i);
    if (!claimedCharacters.value(personaName, false))
    {
      auto it = std::find_if(serverCharacters.begin(), serverCharacters.end(), [&](const char_type& c) { return c.name == personaName; });
      if (it != serverCharacters.end()) return std::distance(serverCharacters.begin(), it);
    }
  }
  return -1;
}

const QVector<char_type> &CharacterRepository::serverList()
{
  return serverCharacters;
}

QVector<char_type> CharacterRepository::filteredList(const QString &packageName)
{
  lastUsedFilter = packageName;

  if (packageName == "Server Characters") return serverCharacters;
  if (packageName == "Favorites") return favoriteCharacters;

  if (packageName == "All") {
    QVector<char_type> allCharacters;
    for (const auto& list : repository)
      allCharacters.append(list);
    return allCharacters;
  }

  return repository.value(packageName);
}

QVector<char_type> CharacterRepository::currentList()
{
  return filteredList(lastUsedFilter);
}

QVector<char_type> CharacterRepository::resetClaims()
{
  claimedCharacters.clear();
  return serverCharacters;
}

int CharacterRepository::networkedIdFromName(const QString &name)
{
  for (int i = 0; i < serverCharacters.size(); ++i) {
    if (serverCharacters[i].name == name)
      return i;
  }
  return -1;
}

int CharacterRepository::networkedIdFromFiltered(int filteredId)
{
  if (filteredId < 0 || filteredId >= filteredCharacters.size()) return -1;
  const QString& name = filteredCharacters.at(filteredId).name;
  return networkedIdFromName(name);
}

QStringList CharacterRepository::filterList()
{
  QStringList filters = defaultFilters;
  QStringList customKeys = repository.keys();
  std::sort(customKeys.begin(), customKeys.end(), [](const QString& a, const QString& b) { return QString::localeAwareCompare(a, b) < 0; });
  filters.append(customKeys);
  return filters;
}

int CharacterRepository::serverListLength()
{
  return serverCharacters.size();
}
