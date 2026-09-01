#ifndef WORKSHOPPACKAGE_H
#define WORKSHOPPACKAGE_H

class WorkshopPackage
{
public:
  static bool extract(const QByteArray& data, const QMap<QString, QString>& files);
};

#endif // WORKSHOPPACKAGE_H
