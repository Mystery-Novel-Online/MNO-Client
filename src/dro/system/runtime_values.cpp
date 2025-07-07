#include "runtime_values.h"

#include <QMap>
#include <QRegularExpressionMatch>

QMap<QString, QString> s_runtimeValues = {};

void dro::runtime::values::storeValue(const QString &name, const QString &value)
{
  if(s_runtimeValues.contains(name) && !name.startsWith("prior_"))
  {
    s_runtimeValues["prior_" + name] = s_runtimeValues[name];
  }
  s_runtimeValues[name] = value;
}

QString dro::runtime::values::resolveVariables(const QString &input)
{
  QString result = input;
  QRegularExpression re(R"(\{([^}]+)\})");
  QRegularExpressionMatchIterator i = re.globalMatch(input);

  while (i.hasNext())
  {
    QRegularExpressionMatch match = i.next();
    QString varName = match.captured(1);
    result.replace(match.captured(0), s_runtimeValues[varName]);
  }

  return result;
}
