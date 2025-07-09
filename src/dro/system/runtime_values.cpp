#include "runtime_values.h"

#include <QMap>
#include <QRegularExpressionMatch>

QMap<QString, QString> s_runtimeValues = {};

void dro::runtime::values::storeValue(const std::string& name, const std::string& value)
{
  QString qName = QString::fromStdString(name);
  QString qValue = QString::fromStdString(value);
  if(s_runtimeValues.contains(qName) && !qName.startsWith("prior_"))
  {
    s_runtimeValues["prior_" + qName] = s_runtimeValues[qName];
  }
  s_runtimeValues[qName] = qValue;
}

std::string dro::runtime::values::resolveVariables(const std::string& input)
{
  QString qInput = QString::fromStdString(input);

  QString result = qInput;
  QRegularExpression re(R"(\{([^}]+)\})");
  QRegularExpressionMatchIterator i = re.globalMatch(qInput);

  while (i.hasNext())
  {
    QRegularExpressionMatch match = i.next();
    QString varName = match.captured(1);
    result.replace(match.captured(0), s_runtimeValues[varName]);
  }

  return result.toStdString();
}
