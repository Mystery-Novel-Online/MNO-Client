#ifndef SERVER_METADATA_H
#define SERVER_METADATA_H

#include "datatypes.h"

namespace ServerMetadata
{
  void SetFeatureList(QStringList features);
  bool FeatureSupported(QString featureName);
}

namespace dro::network::metadata::character::lists
{
  void reset();
  void addFavorite(const QString& folder);
  void removeFavorite(const QString& folder);

  void loadFavorites();
  void saveFavorites();

  void setServerList(QVector<char_type> list);
  void setFilteredList(const QString& package, QVector<char_type> list);

  void setCharacterAvailability(int id, bool status);

  int findAvailablePersona();

  bool characterCheck(const QString& name);
  bool characterCheck(int filterID);
  bool characterTaken(const QString& name);

  QStringList characterFilters();

  QVector<char_type> serverList();
  QVector<char_type> filteredList(QString package);
  QVector<char_type> currentList();
  QVector<char_type> resetClaimed();

  char_type filteredCharacter(int id);

  int lengthServerList();

  void clearFlitered();
  void addFlitered(const char_type& character);

  QString characterNameFiltered(int id);
  QString characterNameServer(int id);

  int characterServerId(int Id);
  int characterServerId(QString name);
}

#endif // SERVER_METADATA_H
