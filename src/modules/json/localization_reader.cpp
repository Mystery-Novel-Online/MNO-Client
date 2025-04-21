#include "localization_reader.h"
#include "file_functions.h"
#include "aoapplication.h"



LocalizationReader::LocalizationReader(QString t_languageCode)
{
  mLangCode = t_languageCode;
  QString l_localizationPath = AOApplication::getInstance()->get_base_path() + "localization/" + t_languageCode + ".json";
  if(file_exists(l_localizationPath))
  {
    ReadFromFile(l_localizationPath);
    mIsLoaded = true;

    mName = getStringValue("name");
    mCredit = getStringValue("translators");

    QJsonArray l_translationsArray = getArrayValue("translations");
    for(QJsonValueRef r_translationData : l_translationsArray)
    {
      SetTargetObject(r_translationData.toObject());
      mValues[getStringValue("id")] = getStringValue("text");
    }
  }
}

bool LocalizationReader::getIsLoaded()
{
  return mIsLoaded;
}

QString LocalizationReader::getLanguageName()
{
  return mName;
}

bool LocalizationReader::containsLocalizationValue(QString t_value)
{
  return mValues.contains(t_value);
}

QString LocalizationReader::getLocalizationValue(QString t_value)
{
  return mValues[t_value];
}

QString LocalizationReader::getCode()
{
  return mLangCode;
}

QString LocalizationReader::getCredit()
{
  return mCredit;
}
