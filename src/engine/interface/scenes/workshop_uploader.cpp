#include "workshop_uploader.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QHttpPart>

WorkshopUploader::WorkshopUploader(QWidget *parent) : QDialog{parent}, m_currentReply(nullptr)
{
  setWindowTitle("Upload Zip File");
  setModal(true);

  m_filePath = new QLineEdit(this);
  m_filePath->setReadOnly(true);

  m_artist = new QLineEdit(this);
  m_description = new QTextEdit(this);

  m_chooseButton = new QPushButton("Choose Zip", this);
  m_submitButton = new QPushButton("Submit", this);

  m_progress = new QProgressBar(this);
  m_progress->setRange(0, 100);
  m_progress->setValue(0);
  m_progress->setVisible(false);

  QFormLayout *layout = new QFormLayout(this);
  layout->addRow("Zip File:", m_filePath);
  layout->addRow("", m_chooseButton);
  layout->addRow("Artist:", m_artist);
  layout->addRow("Description:", m_description);
  layout->addRow("", m_submitButton);
  layout->addRow("Progress:", m_progress);

  setLayout(layout);

  m_network = new QNetworkAccessManager(this);

  connect(m_chooseButton, &QPushButton::clicked, this, &WorkshopUploader::chooseFile);
  connect(m_submitButton, &QPushButton::clicked, this, &WorkshopUploader::submitForm);
  connect(m_network, &QNetworkAccessManager::finished, this, &WorkshopUploader::handleReply);
}

void WorkshopUploader::StartUpload()
{
  QString uploadKey = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE"));
  if(uploadKey.trimmed().isEmpty() || uploadKey.trimmed() == "PUT_KEY_HERE")
  {
    QMessageBox::information(nullptr, "Warning", "In order to upload directly to the workshop, you will need a key. Please request one from Winter to do so.");
    config::ConfigUserSettings::save();
    return;
  }
  WorkshopUploader *prompt = new WorkshopUploader(nullptr);
  prompt->show();
}

void WorkshopUploader::chooseFile()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Zip File", "", "Zip Files (*.zip)");
  if (!file.isEmpty()) {
    m_filePath->setText(file);
  }
}

void WorkshopUploader::submitForm()
{
  if (m_filePath->text().isEmpty()) {
    QMessageBox::warning(this, "Error", "Please select a zip file.");
    return;
  }

  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  QFile *file = new QFile(m_filePath->text());
  if (!file->open(QIODevice::ReadOnly)) {
    QMessageBox::warning(this, "Error", "Unable to open file.");
    delete file;
    delete multiPart;
    return;
  }

  QHttpPart filePart;
  filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QVariant("form-data; name=\"zipfile\"; filename=\"" + QFileInfo(*file).fileName() + "\""));
  filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/zip"));
  filePart.setBodyDevice(file);
  file->setParent(multiPart);
  multiPart->append(filePart);

  auto addField = [&](const QString &name, const QString &value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + name + "\""));
    part.setBody(value.toUtf8());
    multiPart->append(part);
  };

  addField("key", QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE")));
  addField("artist", m_artist->text());
  addField("description", m_description->toPlainText());
  addField("tags", "untagged");

  QUrl url(QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/upload");
  QNetworkRequest request(url);

  m_currentReply = m_network->post(request, multiPart);
  multiPart->setParent(m_currentReply);

  m_progress->setVisible(true);
  m_progress->setValue(0);

  connect(m_currentReply, &QNetworkReply::uploadProgress, this, &WorkshopUploader::updateProgress);
}

void WorkshopUploader::handleReply(QNetworkReply *reply)
{
  m_progress->setVisible(false);

  if (reply->error() == QNetworkReply::NoError) {
    QMessageBox::information(this, "Success", "Upload completed! It should be approved soon.");
  } else {
    QMessageBox::critical(this, "Error", reply->errorString());
  }
  reply->deleteLater();
}

void WorkshopUploader::updateProgress(qint64 bytesSent, qint64 bytesTotal)
{
  if (bytesTotal > 0) {
    int percent = static_cast<int>((bytesSent * 100) / bytesTotal);
    m_progress->setValue(percent);
  }
}
