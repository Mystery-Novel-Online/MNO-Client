#include "workshop_uploader.h"
#include "engine/system/user_database.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QHttpPart>
#include <engine/param/json_reader.h>
#include <engine/fs/fs_characters.h>
#include <engine/fs/fs_reading.h>
#include <engine/network/api_manager.h>
#include <QDialogButtonBox>
#include <QHeaderView>

WorkshopUploader::WorkshopUploader(QWidget *parent, bool edit, int editTarget, const WorkshopContentEntry entry) : QDialog{parent}, m_currentReply(nullptr), m_isEdit(edit), m_editTarget(editTarget)
{
  m_workshopEntry = entry;

  setWindowTitle(m_isEdit ? "Edit Character" : "Upload Character");
  setModal(true);

  m_filePath = new QLineEdit(this);
  m_filePath->setReadOnly(true);

  m_previewPath = new QLineEdit(this);
  m_previewPath->setReadOnly(true);


  m_description = new QTextEdit(this);

  m_tagTable = new UploaderTagTable(this);

  m_chooseButton = new QPushButton("Choose Zip", this);
  m_imageButton = new QPushButton("Choose Preview", this);
  m_submitButton = new QPushButton("Submit", this);

  m_addTag = new QPushButton("Add Tag", this);

  m_collectionList = new QComboBox(this);
  if(edit)
  {
    m_collectionList->addItem("<No Change>");
  }

  m_collectionList->addItem("No Collection");

  for(const QString& name : ApiManager::instance().collections())
  {
    m_collectionList->addItem(name);
  }

  m_progress = new QProgressBar(this);
  m_progress->setRange(0, 100);
  m_progress->setValue(0);
  m_progress->setVisible(true);

  m_private = new QCheckBox(this);
  m_private->setText("Make Upload Private");

  if(edit)
  {
    m_filePath->setText("<No Change>");
    m_description->setText(entry.description);
    m_previewPath->setText("<No Change>");
  }

  m_chooseButton->setMinimumWidth(100);
  m_imageButton->setMinimumWidth(100);
  QFormLayout *layout = new QFormLayout();

  if(entry.content_type != "collection")
  {
    layout->addRow(m_chooseButton, m_filePath);
  }
  else
  {
    m_chooseButton->hide();
    m_filePath->hide();
  }
  layout->addRow(m_imageButton, m_previewPath);

  if(entry.content_type != "collection")
  {
    layout->addRow("Collection:", m_collectionList);
  }
  else
  {
    m_collectionList->hide();
  }

  layout->addRow("Description:", m_description);


  if(entry.content_type != "collection")
  {
    layout->addRow("", m_private);
  }
  else
  {
    m_private->hide();
  }

  layout->addRow(m_submitButton, m_progress);

  QFormLayout *tagLayout = new QFormLayout();

  QHBoxLayout *overallLayout = new QHBoxLayout();
  overallLayout->addLayout(layout);
  overallLayout->addLayout(tagLayout);

  setLayout(overallLayout);

  connect(m_chooseButton, &QPushButton::clicked, this, &WorkshopUploader::chooseFile);
  connect(m_imageButton, &QPushButton::clicked, this, &WorkshopUploader::choosePreviewFile);

  connect(m_submitButton, &QPushButton::clicked, this, &WorkshopUploader::submitForm);
  connect(m_addTag, &QPushButton::clicked, this, &WorkshopUploader::addTagClicked);

  bool allowSystem = ApiManager::instance().userPermission() > APIPerms_Auto;

  if(entry.content_type != "collection")
  {

    tagLayout->addRow("", m_addTag);
    tagLayout->addRow("Tags:", m_tagTable);

    if(entry.tagMap.isEmpty() && !m_isEdit)
    {
      m_tagTable->addTag("Artist", "Unknown", true);
      m_tagTable->addTag("Franchise", "Unknown", true);
    }
    else
    {
      bool artistAdded = false;
      bool franchiseAdded = false;


      for (const QPair<QString, QString>& pair : entry.tagMap)
      {
        QString key = pair.first;
        QString value = pair.second;
        bool notOptionalTag = false;
        if(key == "Artist" && !artistAdded)
        {
          artistAdded = true;
          notOptionalTag = true;
        }
        if(key == "Franchise" && !franchiseAdded)
        {
          franchiseAdded = true;
          notOptionalTag = true;
        }

        if(key == "System" && !allowSystem)
          notOptionalTag = true;

        if(key == "Gender" && !allowSystem)
          notOptionalTag = true;

        m_tagTable->addTag(key, value, notOptionalTag);
      }
    }
  }
  else
  {
    m_addTag->hide();
    m_tagTable->hide();
  }
}

void WorkshopUploader::StartUpload()
{
  bool loginStatus = ApiManager::loggedIn();
  if(!loginStatus)
  {
    QMessageBox::information(nullptr, "Warning", "You currently are not authenticated to upload");
    return;
  }

  WorkshopUploader *prompt = new WorkshopUploader(nullptr);
  prompt->show();
}

void WorkshopUploader::StartEdit(int id, const WorkshopContentEntry entry)
{
  bool loginStatus = ApiManager::loggedIn();
  if(!loginStatus)
  {
    QMessageBox::information(nullptr, "Warning", "You currently are not authenticated to upload");
    return;
  }

  WorkshopUploader *prompt = new WorkshopUploader(nullptr, true, id, entry);
  prompt->show();
}

void WorkshopUploader::chooseFile()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Zip File", "", "Zip Files (*.zip)");
  if (!file.isEmpty()) {
    m_filePath->setText(file);
  }
}

void WorkshopUploader::addTagClicked()
{
  QDialog dialog(this);
  dialog.setWindowTitle("Add Tag");

  auto *layout = new QVBoxLayout(&dialog);

  auto *categoryLabel = new QLabel("Category:", &dialog);
  auto *categoryCombo = new QComboBox(&dialog);

  int i = 1;

  bool allowSystem = ApiManager::instance().userPermission() > APIPerms_Auto;

  for(std::string cat : ApiManager::instance().contentCategories())
  {
    if(cat != "System" && cat != "Gender")
      categoryCombo->addItem(QString::fromStdString(cat), i);
    else if (allowSystem)
      categoryCombo->addItem(QString::fromStdString(cat), i);
    i++;
  };

  auto *tagLabel = new QLabel("Tag:", &dialog);
  auto *tagEdit = new QLineEdit(&dialog);
  tagEdit->setPlaceholderText("Enter tag name...");

  auto *buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal,
      &dialog
      );

  layout->addWidget(categoryLabel);
  layout->addWidget(categoryCombo);
  layout->addWidget(tagLabel);
  layout->addWidget(tagEdit);
  layout->addWidget(buttons);

  QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted)
  {
    int categoryId = categoryCombo->currentData().toInt();
    QString categoryName = categoryCombo->currentText();
    QString tagName = tagEdit->text().trimmed();

    if (tagName.isEmpty())
      return;

    m_tagTable->addTag(categoryName, tagName);
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

  //ApiManager::appendField(multiPart, "artist", m_artist->text());

  ApiManager::appendField(multiPart, "description", m_description->toPlainText());
  ApiManager::appendField(multiPart, "is_private", QString::number(m_private->checkState() == Qt::Checked));

  ApiManager::appendField(multiPart, "collection", m_collectionList->currentText());


  QJsonArray arr;

  for (int row = 0; row < m_tagTable->rowCount(); ++row)
  {
    auto categoryItem = m_tagTable->item(row, 0);
    auto tagItem      = m_tagTable->item(row, 1);

    if (!categoryItem || !tagItem)
      continue;

    QString category = categoryItem->text().trimmed();
    QString tag      = tagItem->text().trimmed();

    if (category.isEmpty() || tag.isEmpty())
      continue;


    QJsonObject obj;
    obj["category"] = category;
    obj["tag"] = tag;
    arr.append(obj);

  }

  QJsonDocument doc(arr);

  ApiManager::appendField(multiPart, "tags", doc.toJson(QJsonDocument::Compact));

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

    const QString qFolderName = reader.getStringValue("folder_name");
    const QString qGUID = reader.getStringValue("guid");
    const int qContentId = reader.getIntValue("content_id");
    const int qLastUpdated = reader.getIntValue("last_updated");

    if(FS::Checks::CharacterExists(qFolderName.toStdString().c_str()))
    {
      GetDB().cacheContentData(qGUID.toStdString(), qFolderName.toStdString(), qLastUpdated, qContentId);
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
