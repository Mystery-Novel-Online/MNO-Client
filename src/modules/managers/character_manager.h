#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <aoemotebutton.h>
#include <datatypes.h>

#include "dro/param/actor/actor_loader.h"



class CharacterManager
{
public:
  CharacterManager(const CharacterManager&) = delete;

  ActorData *p_SelectedCharacter = nullptr;

  ActorData *ReadCharacter(QString t_folder);
  ActorData *SwitchCharacter(QString t_folder);


  static CharacterManager& get()
  {
    return s_Instance;
  }

  void setOutfitList(QStringList t_outfits);
  void setOutfitIndex(int t_index);

  QString lastCharList = "Server Characters";
  QVector<char_type> GetCharList();
  QVector<char_type> GetCharList(QString package);
  QVector<char_type> GetLastCharList();
  QVector<char_type> GetServerCharList();

  QString GetFilteredCharaName(int id);
  QString GetServerCharaName(int id);

  void ResetPackages();
  void SetCharList(QVector<char_type> charList);
  void SetCharList(QString package, QVector<char_type> charList);

  void SetCharaTaken(int id, bool status);

  void AddToFavorites(QString chara);
  void RemoveFromFavorites(QString chara);
  void LoadFavoritesList();
  void SaveFavoritesList();

  int GetAvaliablePersona();

  QStringList GetCharacterPackages();

  QVector<char_type> mFilteredChrList;

  QVector<char_type> mServerCharacters;
  QVector<char_type> mFavoriteCharacters;

  QHash<QString, QVector<char_type>> mPackageCharacters;

  bool GetCharacterInServer(QString name);
  bool GetCharacterInServer(int filterID);
  int GetFilteredId(int Id);
  int GetFilteredId(QString name);

private:
  CharacterManager()
  {

  }
  static CharacterManager s_Instance;
  QHash<QString, bool>CharacterTaken = {};
  QStringList mCharacterPackages = {"Server Characters", "Favorites", "All"};
  QStringList mCharacterOutfits = {};

};

#endif // CHARACTERMANAGER_H
