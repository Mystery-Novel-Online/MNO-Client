#include "TransferOwnershipDialog.h"

#include <QFormLayout>
#include <QHttpMultiPart>

#include "engine/network/api_manager.h"
#include "engine/param/json_reader.h"

TransferOwnershipDialog::TransferOwnershipDialog(const WorkshopContentEntry &content, QWidget *parent)
  : QDialog(parent)
  , m_contentId(content.id)
{
  m_userValue = new QLineEdit(this);
  m_userValue->setText("");

  m_submitButton = new QPushButton("Submit", this);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Discord ID:", m_userValue);
  layout->addRow("", m_submitButton);

  connect(m_submitButton, &QPushButton::clicked, this, &TransferOwnershipDialog::submit);

  setLayout(layout);
}

void TransferOwnershipDialog::submit()
{
  if(m_currentReply || m_contentId <= -1) {
    return;
  }

  m_submitButton->setEnabled(false);

  QString discordUserId = m_userValue->text().trimmed();

  if(discordUserId.count() >= 19 || discordUserId.count() <= 16)
  {
    QMessageBox::warning(this, "Error", "You have not provided a valid discord account to transfer to.");
    return;
  }

  QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  ApiManager::appendField(multiPart, "id", QString::number(m_contentId));
  ApiManager::appendField(multiPart, "key", ApiManager::authorizationKey());
  ApiManager::appendField(multiPart, "user_target", discordUserId);

  m_currentReply = ApiManager::instance().post("api/workshop/transfer", multiPart);
  connect(m_currentReply, &QNetworkReply::finished, this, &TransferOwnershipDialog::handleReply);
}

void TransferOwnershipDialog::handleReply()
{
  JSONReader reader;
  if(m_currentReply->error() == QNetworkReply::NoError) {
    QMessageBox::information(this, "Success", "Content ownership transfered successfully.");
    accept();
  }
  else {
    reader.ReadFromString(m_currentReply->readAll());
    QMessageBox::critical(this, "Error", reader.getStringValue("error"));
    m_submitButton->setEnabled(true);
  }
  m_currentReply->deleteLater();
  m_currentReply = nullptr;
}
