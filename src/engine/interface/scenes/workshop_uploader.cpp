#include "workshop_uploader.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QHttpPart>
#include <engine/param/json_reader.h>
#include <engine/fs/fs_characters.h>
#include <engine/fs/fs_reading.h>
#include <engine/network/api_manager.h>

WorkshopUploader::WorkshopUploader(QWidget *parent, bool edit, int editTarget) : QDialog{parent}, m_currentReply(nullptr), m_isEdit(edit), m_editTarget(editTarget)
{

  setWindowTitle(m_isEdit ? "Edit Character" : "Upload Character");
  setModal(true);

  m_filePath = new QLineEdit(this);
  m_filePath->setReadOnly(true);

  m_previewPath = new QLineEdit(this);
  m_previewPath->setReadOnly(true);


  m_artist = new QLineEdit(this);
  m_description = new QTextEdit(this);

  m_chooseButton = new QPushButton("Choose Zip", this);
  m_imageButton = new QPushButton("Choose Preview", this);
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
    m_previewPath->setText("<No Change>");
  }

  QFormLayout *layout = new QFormLayout(this);
  layout->addRow("Zip File:", m_filePath);
  layout->addRow("", m_chooseButton);
  layout->addRow("Preview Image:", m_previewPath);
  layout->addRow("", m_imageButton);
  layout->addRow("Artist:", m_artist);
  layout->addRow("Description:", m_description);
  layout->addRow("", m_private);
  layout->addRow("", m_submitButton);
  layout->addRow("Progress:", m_progress);

  setLayout(layout);

  m_network = new QNetworkAccessManager(this);

  connect(m_chooseButton, &QPushButton::clicked, this, &WorkshopUploader::chooseFile);
  connect(m_imageButton, &QPushButton::clicked, this, &WorkshopUploader::choosePreviewFile);

  connect(m_submitButton, &QPushButton::clicked, this, &WorkshopUploader::submitForm);
  connect(m_network, &QNetworkAccessManager::finished, this, &WorkshopUploader::handleReply);
}

void WorkshopUploader::StartUpload()
{
  QString uploadKey = ApiManager::authorizationKey();
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
  QString uploadKey = ApiManager::authorizationKey();
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

void WorkshopUploader::choosePreviewFile()
{
  QString file = QFileDialog::getOpenFileName(this, "Select PNG File", "", "Image Files (*.png)");
  if (!file.isEmpty()) {
    m_previewPath->setText(file);
  }
}

void WorkshopUploader::submitForm()
{
  if (m_filePath->text().isEmpty() && !m_isEdit) {
    QMessageBox::warning(this, "Error", "Please select a zip file.");
    return;
  }

  if (m_previewPath->text().isEmpty() && !m_isEdit) {
    QMessageBox::warning(this, "Error", "Please select a preview file.");
    return;
  }


  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  if(!m_filePath->text().trimmed().isEmpty() && m_filePath->text() != "<No Change>")
  {
    if (!ApiManager::appendFile(multiPart, "zipfile", m_filePath->text())) {
      QMessageBox::warning(this, "Error", "Unable to open file.");
      delete multiPart;
      return;
    }
  }

  if(!m_previewPath->text().trimmed().isEmpty() && m_previewPath->text() != "<No Change>")
  {
    if (!ApiManager::appendFile(multiPart, "preview", m_previewPath->text())) {
      QMessageBox::warning(this, "Error", "Unable to open file.");
      delete multiPart;
      return;
    }
  }


  ApiManager::appendField(multiPart, "key", ApiManager::authorizationKey());

  if(m_editTarget != -1)
    ApiManager::appendField(multiPart, "id", QString::number(m_editTarget));

  ApiManager::appendField(multiPart, "artist", m_artist->text());
  ApiManager::appendField(multiPart, "description", m_description->toPlainText());
  ApiManager::appendField(multiPart, "tags", "untagged");
  ApiManager::appendField(multiPart, "is_private", QString::number(m_private->checkState() == Qt::Checked));

  m_currentReply = ApiManager::instance().post(QString(m_isEdit ? "api/workshop/edit" : "api/workshop/upload"), multiPart);

  m_progress->setVisible(true);
  m_progress->setValue(0);

  connect(m_currentReply, &QNetworkReply::finished, this, &WorkshopUploader::handleReply);
  connect(m_currentReply, &QNetworkReply::uploadProgress, this, &WorkshopUploader::updateProgress);
}

void WorkshopUploader::handleReply()
{
  m_progress->setVisible(false);

  JSONReader reader;
  if (m_currentReply->error() == QNetworkReply::NoError) {
    QMessageBox::information(this, "Success", "Upload completed! It should be approved soon.");

    reader.ReadFromString(m_currentReply->readAll());

    const QString characterFolder = reader.getStringValue("folder_name");
    if(FS::Checks::CharacterExists(characterFolder.toStdString().c_str()))
    {
      QString contentFilePath = fs::characters::getFilePath(characterFolder, "CONTENT.txt");
      QFile contentFile(contentFilePath);
      if (contentFile.open(QIODevice::WriteOnly | QIODevice::Text))
      {
        QTextStream out(&contentFile);
        out << ApiManager::repoUrl(reader.getIntValue("character_id"));
        contentFile.close();
      }
    }
  } else {
    reader.ReadFromString(m_currentReply->readAll());
    QMessageBox::critical(this, "Error", reader.getStringValue("error"));
    qDebug() << m_currentReply->readAll();;
  }
  m_currentReply->deleteLater();
}

void WorkshopUploader::updateProgress(qint64 bytesSent, qint64 bytesTotal)
{
  if (bytesTotal > 0) {
    int percent = static_cast<int>((bytesSent * 100) / bytesTotal);
    m_progress->setValue(percent);
  }
}
