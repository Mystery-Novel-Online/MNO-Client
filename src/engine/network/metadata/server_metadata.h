#ifndef SERVER_METADATA_H
#define SERVER_METADATA_H



namespace engine::network::metadata
{
  class VNServerInformation
  {
  public:
    static void setFeatureList(QStringList features);
    static bool featureSupported(const QString& name);

  private:
    static inline QStringList featureList = {};
  };

  class CharacterRepository
  {
  public:
    static void reset();

    static void addFavorite(const QString& folder);
    static void removeFavorite(const QString& folder);

    static void loadFavorites();
    static void saveFavorites();

    static void setCharacterAvailability(int index, bool available);
    static bool isCharacterAvailable(const QString& name);

    static void setServerList(const QVector<char_type>& list);
    static void setFilteredList(const QString& name, const QVector<char_type>& list);

    static bool characterExists(const QString& name);
    static bool filteredCharacterExists(int filterIndex);

    static void clearFiltered();
    static void addFiltered(const char_type& character);
    static char_type filteredCharacter(int index);

    static QString characterNameFiltered(int index);
    static QString characterNameServer(int index);

    static int findAvailablePersona();

    static const QVector<char_type>& serverList();
    static QVector<char_type> filteredList(const QString& packageName);
    static QVector<char_type> currentList();
    static QVector<char_type> resetClaims();

    static int networkedIdFromName(const QString& name);
    static int networkedIdFromFiltered(int filteredId);

    static QStringList filterList();
    static int serverListLength();

  private:
    static inline QString lastUsedFilter = "Server Characters";

    static inline const QStringList defaultFilters = { "Server Characters", "Favorites", "All" };
    static inline QVector<char_type> serverCharacters;
    static inline QVector<char_type> filteredCharacters;
    static inline QVector<char_type> favoriteCharacters;

    static inline QHash<QString, QVector<char_type>> repository;
    static inline QHash<QString, bool> claimedCharacters;
  };
}

#endif // SERVER_METADATA_H
