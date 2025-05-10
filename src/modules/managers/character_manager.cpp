#include "character_manager.h"
#include "aoemotebutton.h"
#include "commondefs.h"
#include "dro/fs/fs_reading.h"
#include "emotion_manager.h"

#include <AOApplication.h>
#include <QCheckBox>
#include <QFile>
#include "courtroom.h"
#include "qmath.h"
#include <QTextStream>

#include <modules/theme/thememanager.h>

CharacterManager CharacterManager::s_Instance;


ActorData *CharacterManager::ReadCharacter(QString t_folder)
{
  QString l_jsonPath = AOApplication::getInstance()->get_character_path(t_folder, "char.json");
  if(FS::Checks::FileExists(l_jsonPath))
  {
    ActorData *l_returnData = new ActorDataReader();
    l_returnData->LoadActor(t_folder);
    return l_returnData;
  }

  ActorData *l_returnData = new LegacyActorReader();
  l_returnData->LoadActor(t_folder);
  return l_returnData;
}

ActorData *CharacterManager::SwitchCharacter(QString t_folder)
{
  QString l_jsonPath = AOApplication::getInstance()->get_character_path(t_folder, "char.json");

  QStringList l_OutfitNames = {"<All>"};


  if(FS::Checks::FileExists(l_jsonPath))
  {
    p_SelectedCharacter = new ActorDataReader();
    p_SelectedCharacter->LoadActor(t_folder);
    QStringList l_charaOutfits = p_SelectedCharacter->GetOutfitNames();
    l_OutfitNames.append(l_charaOutfits);
    setOutfitList(l_OutfitNames);
    return p_SelectedCharacter;
  }

  p_SelectedCharacter = new LegacyActorReader();
  p_SelectedCharacter->LoadActor(t_folder);
  setOutfitList(l_OutfitNames);
  return p_SelectedCharacter;
}

void CharacterManager::setOutfitList(QStringList t_outfits)
{
  mCharacterOutfits = t_outfits;
  
  QWidget *l_outfitSelectorWidget = ThemeManager::get().getWidget("outfit_selector");

  if (dynamic_cast<QComboBox*>(l_outfitSelectorWidget) != nullptr)
  {
    QComboBox* l_outfitSelectorCombo = dynamic_cast<QComboBox*>(l_outfitSelectorWidget);
    l_outfitSelectorCombo->clear();
    l_outfitSelectorCombo->addItems(t_outfits);
    if(l_outfitSelectorCombo->count() > 1) l_outfitSelectorCombo->setCurrentIndex(1);
  }
}

void CharacterManager::setOutfitIndex(int t_index)
{
  if(mCharacterOutfits.length() > t_index && t_index != -1)
  {
    p_SelectedCharacter->SwitchOutfit(mCharacterOutfits[t_index]);
  }
  EmotionManager::get().refreshEmoteSelection(false);
  EmotionManager::get().refreshEmotePage();
}

QVector<char_type> CharacterManager::GetCharList()
{
  return mServerCharacters;
}

QVector<char_type> CharacterManager::GetCharList(QString package)
{
  lastCharList = package;
  if(package == "Server Characters") return mServerCharacters;
  if(package == "Favorites") return mFavoriteCharacters;

  if(package == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = mPackageCharacters.constBegin(); i != mPackageCharacters.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(mPackageCharacters.contains(package))
  {
    return mPackageCharacters[package];
  }
  return QVector<char_type>();
}

QVector<char_type> CharacterManager::GetLastCharList()
{
  if(lastCharList == "Server Characters") return mServerCharacters;
  if(lastCharList == "Favorites") return mFavoriteCharacters;

  if(lastCharList == "All")
  {
    QVector<char_type> ALlCharacters = {};
    QHash<QString, QVector<char_type>>::const_iterator i;
    for (i = mPackageCharacters.constBegin(); i != mPackageCharacters.constEnd(); ++i)
    {
      ALlCharacters.append(i.value());
    }
    return ALlCharacters;
  }

  if(mPackageCharacters.contains(lastCharList))
  {
    return mPackageCharacters[lastCharList];
  }

  return mServerCharacters;
}

QVector<char_type> CharacterManager::GetServerCharList()
{
  return mServerCharacters;
  CharacterTaken = {};
}

QString CharacterManager::GetFilteredCharaName(int id)
{
  return mFilteredChrList.at(id).name;
}

QString CharacterManager::GetServerCharaName(int id)
{
  return mServerCharacters.at(id).name;
}

void CharacterManager::ResetPackages()
{
  lastCharList = "Server Characters";
  mPackageCharacters = {};
  mCharacterPackages.clear();
  mCharacterPackages.append({"Server Characters", "Favorites", "All"});
}

void CharacterManager::SetCharList(QVector<char_type> charList)
{
  mServerCharacters = charList;
}

void CharacterManager::SetCharList(QString package, QVector<char_type> charList)
{
  if(!mCharacterPackages.contains(package)) mCharacterPackages.append(package);
  mPackageCharacters[package] = charList;
}

bool CharacterManager::GetCharacterInServer(QString name)
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == name)
    {
      return true;
    }
  }

  return false;
}

bool CharacterManager::GetCharacterInServer(int filterID)
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == mFilteredChrList.at(filterID).name)
    {
      return true;
    }
  }

  return false;
}

void CharacterManager::SetCharaTaken(int id, bool status)
{
  CharacterTaken[mServerCharacters.at(id).name] = status;
}

void CharacterManager::AddToFavorites(QString chara)
{
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    if (chara == mFavoriteCharacters.at(j).name)
    {
      return;
    }
  }
  char_type ct;
  ct.name = chara;
  mFavoriteCharacters.append(ct);
  SaveFavoritesList();
}

void CharacterManager::RemoveFromFavorites(QString chara)
{
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    if (chara == mFavoriteCharacters.at(j).name)
    {
      mFavoriteCharacters.removeAt(j);
      return;
    }
  }
  SaveFavoritesList();
}

void CharacterManager::LoadFavoritesList()
{
  mFavoriteCharacters.clear();
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
      mFavoriteCharacters.append(character);
    }
  }

  file.close();

}

void CharacterManager::SaveFavoritesList()
{
  QStringList favoritesList = {};
  for (int j = 0; j < mFavoriteCharacters.size(); j++)
  {
    favoritesList.append(mFavoriteCharacters[j].name);
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

int CharacterManager::GetFilteredId(int Id)
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == mFilteredChrList.at(Id).name)
    {
      return j;
    }
  }

  return -1;
}

int CharacterManager::GetFilteredId(QString name)
{
  for (int j = 0; j < mServerCharacters.size(); j++)
  {
    if (mServerCharacters[j].name == name)
    {
      return j;
    }
  }

  return -1;
}

int CharacterManager::GetAvaliablePersona()
{
  for(int i = 0; i < 25; i++)
  {
    QString personaName = QString("Persona" + QString::number(i));
    if(CharacterTaken.contains(personaName))
    {
      if(!CharacterTaken[personaName])
      {
        for (int j = 0; j < mServerCharacters.size(); j++)
        {
          if (mServerCharacters[j].name == personaName)
          {
            return j;
          }
        }

      }
    }
  }

  return -1;
}

QStringList CharacterManager::GetCharacterPackages()
{
  return mCharacterPackages;
}
