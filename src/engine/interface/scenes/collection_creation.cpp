#include "collection_creation.h"
#include <QFormLayout>
#include <QHttpMultiPart>
#include <engine/network/api_manager.h>
#include <engine/param/json_reader.h>


CollectionCreation::CollectionCreation(QWidget *parent) : QDialog{parent}
{
  setWindowTitle("Create Collection");
  setModal(true);

  w_IconLineEdit = new QLineEdit(this);
  w_IconLineEdit->setReadOnly(true);

  w_NameLineEdit = new QLineEdit(this);
  w_DescTextEdit = new QTextEdit(this);

  w_IconPushBtn = new QPushButton("Choose Icon", this);
  w_SbmtPushBtn = new QPushButton("Submit", this);

  QFormLayout *layout = new QFormLayout(this);
  layout->addRow("Icon Image:", w_IconLineEdit);
  layout->addRow("", w_IconPushBtn);
  layout->addRow("Name:", w_NameLineEdit);
  layout->addRow("Description:", w_DescTextEdit);
  layout->addRow("", w_SbmtPushBtn);
  setLayout(layout);

  connect(w_IconPushBtn, &QPushButton::clicked, this, &CollectionCreation::iconSelection);
  connect(w_SbmtPushBtn, &QPushButton::clicked, this, &CollectionCreation::submitForm);
}

void CollectionCreation::init()
{
  QString uploadKey = ApiManager::authorizationKey();
  if(uploadKey.trimmed().isEmpty() || uploadKey.trimmed() == "PUT_KEY_HERE")
  {
    QMessageBox::information(nullptr, "Warning", "You are currently not logged in.");
    config::ConfigUserSettings::save();
    return;
  }
  CollectionCreation *prompt = new CollectionCreation(nullptr);
  prompt->show();
}

void CollectionCreation::iconSelection()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Collection Icon", "", "PNG Files (*.png)");
  if (!file.isEmpty()) w_IconLineEdit->setText(file);
}

void CollectionCreation::submitForm()
{
  if (w_IconLineEdit->text().isEmpty()) {
    QMessageBox::warning(this, "Error", "Please select a preview file.");
    return;
  }

  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  if(!w_IconLineEdit->text().trimmed().isEmpty() && w_IconLineEdit->text() != "<No Change>")
  {
    if (!ApiManager::appendFile(multiPart, "icon", w_IconLineEdit->text())) {
      QMessageBox::warning(this, "Error", "Unable to open file.");
      delete multiPart;
      return;
    }
  }

  ApiManager::appendField(multiPart, "key", ApiManager::authorizationKey());
  ApiManager::appendField(multiPart, "name", w_NameLineEdit->text());
  ApiManager::appendField(multiPart, "description", w_DescTextEdit->toPlainText());
  ApiManager::appendField(multiPart, "tags", "untagged");

  m_currentReply = ApiManager::instance().post(QString("api/workshop/collection/create"), multiPart);

  connect(m_currentReply, &QNetworkReply::finished, this, &CollectionCreation::handleReply);
  connect(m_currentReply, &QNetworkReply::uploadProgress, this, &CollectionCreation::updateProgress);
}

void CollectionCreation::handleReply()
{
  JSONReader reader;
  if (m_currentReply->error() == QNetworkReply::NoError) {
    QMessageBox::information(this, "Success", "Collection created!");
    reader.ReadFromString(m_currentReply->readAll());
  } else {
    reader.ReadFromString(m_currentReply->readAll());
    QMessageBox::critical(this, "Error", reader.getStringValue("error"));
  }
  m_currentReply->deleteLater();
}

void CollectionCreation::updateProgress(qint64 bytesSent, qint64 bytesTotal)
{

}
