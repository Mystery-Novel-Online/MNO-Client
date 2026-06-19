#include "RenameContentDialog.h"

#include <QFormLayout>
#include <QHttpMultiPart>

#include <engine/network/api_manager.h>

#include <engine/param/json_reader.h>

RenameContentDialog::RenameContentDialog(int contentId, const QString &contentName, QWidget *parent) : QDialog(parent), m_workshopId(contentId)
{
  m_nameEdit = new QLineEdit(this);
  m_nameEdit->setText(contentName);

  m_submitButton = new QPushButton("Submit", this);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Name:", m_nameEdit);
  layout->addRow("", m_submitButton);

  connect(m_submitButton, &QPushButton::clicked, this, &RenameContentDialog::submit);

  setLayout(layout);
}

void RenameContentDialog::submit()
{
  if (m_currentReply)
    return;

  m_submitButton->setEnabled(false);

  QString newName = m_nameEdit->text().trimmed();

  if (newName.isEmpty())
  {
    QMessageBox::warning(this, "Error", "Please enter a name.");
    return;
  }

  QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  if(m_workshopId != -1)
    ApiManager::appendField(multiPart, "id", QString::number(m_workshopId));

  ApiManager::appendField(multiPart, "key", ApiManager::authorizationKey());
  ApiManager::appendField(multiPart, "name", newName);

  m_currentReply = ApiManager::instance().post("api/workshop/edit", multiPart);

  connect(m_currentReply, &QNetworkReply::finished, this, &RenameContentDialog::handleReply);
}

void RenameContentDialog::handleReply()
{
  JSONReader reader;
  if (m_currentReply->error() == QNetworkReply::NoError)
  {
    QMessageBox::information(this, "Success", "Name changed successfully.");
    accept();
  } else
  {
    reader.ReadFromString(m_currentReply->readAll());
    QMessageBox::critical(this, "Error", reader.getStringValue("error"));
    qDebug() << m_currentReply->readAll();;
    m_submitButton->setEnabled(true);
  }
  m_currentReply->deleteLater();
  m_currentReply = nullptr;
}
