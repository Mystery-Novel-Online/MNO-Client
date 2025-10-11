#include "workshop_uploader.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QHttpPart>
#include <engine/param/json_reader.h>
#include <engine/fs/fs_characters.h>
#include <engine/fs/fs_reading.h>

WorkshopUploader::WorkshopUploader(QWidget *parent, bool edit, int editTarget) : QDialog{parent}, m_currentReply(nullptr), m_isEdit(edit), m_editTarget(editTarget)
{

  setWindowTitle(m_isEdit ? "Edit Character" : "Upload Character");
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

  m_private = new QCheckBox(this);
  m_private->setText("Make Upload Private");

  if(edit)
  {
    m_filePath->setText("<No Change>");
    m_description->setText("<No Change>");
    m_artist->setText("<No Change>");
  }

  QFormLayout *layout = new QFormLayout(this);
  layout->addRow("Zip File:", m_filePath);
  layout->addRow("", m_chooseButton);
  layout->addRow("Artist:", m_artist);
  layout->addRow("Description:", m_description);
  layout->addRow("", m_private);
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
    QMessageBox::information(nullptr, "Warning", "You currently are not authenticated to upload");
    config::ConfigUserSettings::save();
    return;
  }
  WorkshopUploader *prompt = new WorkshopUploader(nullptr);
  prompt->show();
}

void WorkshopUploader::StartEdit(int id)
{
  QString uploadKey = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE"));
  if(uploadKey.trimmed().isEmpty() || uploadKey.trimmed() == "PUT_KEY_HERE")
  {
    QMessageBox::information(nullptr, "Warning", "You currently are not authenticated to upload");
    config::ConfigUserSettings::save();
    return;
  }
  WorkshopUploader *prompt = new WorkshopUploader(nullptr, true, id);
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
  if (m_filePath->text().isEmpty() && !m_isEdit) {
    QMessageBox::warning(this, "Error", "Please select a zip file.");
    return;
  }

  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  if(!m_filePath->text().trimmed().isEmpty() && m_filePath->text() != "<No Change>")
  {
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
  }


  auto addField = [&](const QString &name, const QString &value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + name + "\""));
    part.setBody(value.toUtf8());
    multiPart->append(part);
  };

  addField("key", QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE")));

  if(m_editTarget != -1)
    addField("id", QString::number(m_editTarget));

  if(!m_artist->text().trimmed().isEmpty() && m_artist->text() != "<No Change>")
    addField("artist", m_artist->text());

  if(!m_description->toPlainText().trimmed().isEmpty() && m_description->toPlainText() != "<No Change>")
    addField("description", m_description->toPlainText());

  addField("tags", "untagged");
  addField("is_private", QString::number(m_private->checkState() == Qt::Checked));

  QUrl url(QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + QString(m_isEdit ? "api/workshop/edit" : "api/workshop/upload"));
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

    JSONReader reader;
    reader.ReadFromString(reply->readAll());

    const QString characterFolder = reader.getStringValue("folder_name");
    if(FS::Checks::CharacterExists(characterFolder.toStdString().c_str()))
    {
      QString repoUrl = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/" + QString::number(reader.getIntValue("character_id")) + "/repo";
      QString contentFilePath = fs::characters::getFilePath(characterFolder, "CONTENT.txt");
      QFile contentFile(contentFilePath);
      if (contentFile.open(QIODevice::WriteOnly | QIODevice::Text))
      {
        QTextStream out(&contentFile);
        out << repoUrl;
        contentFile.close();
      }
    }
  } else {
    QMessageBox::critical(this, "Error", reply->errorString());
    qDebug() << reply->readAll();;
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
