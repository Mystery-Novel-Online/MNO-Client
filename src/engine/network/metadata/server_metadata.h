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

    static void setServerList(const QVector<ActorSelectEntry>& list);
    static void setFilteredList(const QString& name, const QVector<ActorSelectEntry>& list);

    static bool characterExists(const QString& name);
    static bool filteredCharacterExists(int filterIndex);

    static void clearFiltered();
    static void addFiltered(const ActorSelectEntry& character);
    static ActorSelectEntry filteredCharacter(int index);

    static QString characterNameFiltered(int index);
    static QString characterNameServer(int index);

    static int findAvailablePersona();

    static const QVector<ActorSelectEntry>& serverList();
    static QVector<ActorSelectEntry> filteredList(const QString& packageName);
    static QVector<ActorSelectEntry> currentList();
    static QVector<ActorSelectEntry> resetClaims();

    static int networkedIdFromName(const QString& name);
    static int networkedIdFromFiltered(int filteredId);

    static QStringList filterList();
    static int serverListLength();

  private:
    static inline QString lastUsedFilter = "Server Characters";

    static inline const QStringList defaultFilters = { "Server Characters", "Favorites", "Recently Used", "All" };
    static inline QVector<ActorSelectEntry> serverCharacters;
    static inline QVector<ActorSelectEntry> filteredCharacters;
    static inline QVector<ActorSelectEntry> favoriteCharacters;
    static inline QVector<ActorSelectEntry> recentCharacters;

    static inline QHash<QString, QVector<ActorSelectEntry>> repository;
    static inline QHash<QString, bool> claimedCharacters;
  };
}

#endif // SERVER_METADATA_H
