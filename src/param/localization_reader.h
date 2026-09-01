#ifndef LOCALIZATIONREADER_H
#define LOCALIZATIONREADER_H

#include "json_reader.h"



class LocalizationReader : public JSONReader
{
public:
  LocalizationReader(QString t_languageCode);
  bool getIsLoaded();
  QString getLanguageName();
  bool containsLocalizationValue(QString t_value);
  QString getLocalizationValue(QString t_value);
  QString getCode();
  QString getCredit();

private:
  QMap<QString, QString> mValues = { };
  QString mName = "";
  QString mCredit = "";
  QString mLangCode = "en";
  bool mIsLoaded = false;
};

#endif // LOCALIZATIONREADER_H
