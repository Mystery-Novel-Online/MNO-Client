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

WorkshopUploader::WorkshopUploader(QWidget *parent, bool edit, int editTarget, QMap<QString, QString> tagMap) : QDialog{parent}, m_currentReply(nullptr), m_isEdit(edit), m_editTarget(editTarget)
{

  setWindowTitle(m_isEdit ? "Edit Character" : "Upload Character");
  setModal(true);

  m_filePath = new QLineEdit(this);
  m_filePath->setReadOnly(true);

  m_previewPath = new QLineEdit(this);
  m_previewPath->setReadOnly(true);


  m_artist = new QLineEdit(this);
  m_description = new QTextEdit(this);

  m_tagTable = new QTableWidget(this);
  m_tagTable->setColumnCount(3);

  QStringList headers;
  headers << "Category" << "Tag" << "";
  m_tagTable->setHorizontalHeaderLabels(headers);

  m_tagTable->horizontalHeader()->setStretchLastSection(false);
  m_tagTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_tagTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_tagTable->setColumnWidth(2, 80);

  m_tagTable->setEditTriggers(
      QAbstractItemView::DoubleClicked |
      QAbstractItemView::EditKeyPressed
      );

  connect(m_tagTable, &QTableWidget::itemChanged, this, [=](QTableWidgetItem* item)
          {
            if (item->column() != 1) return;
            item->setText(item->text().trimmed());
          });

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
  layout->addRow("", m_addTag);
  layout->addRow("Tags:", m_tagTable);
  layout->addRow("Collection:", m_collectionList);
  layout->addRow("", m_private);
  layout->addRow("", m_submitButton);
  layout->addRow("Progress:", m_progress);

  setLayout(layout);

  connect(m_chooseButton, &QPushButton::clicked, this, &WorkshopUploader::chooseFile);
  connect(m_imageButton, &QPushButton::clicked, this, &WorkshopUploader::choosePreviewFile);

  connect(m_submitButton, &QPushButton::clicked, this, &WorkshopUploader::submitForm);
  connect(m_addTag, &QPushButton::clicked, this, &WorkshopUploader::addTagClicked);

  if(tagMap.isEmpty() && !m_isEdit)
  {
    addTag(2, "Artist", "Unknown", true);
    addTag(2, "Franchise", "Unkown", true);
  }
  else
  {
    bool artistAdded = false;
    bool franchiseAdded = false;


    for (auto it = tagMap.begin(); it != tagMap.end(); ++it)
    {
      QString key = it.key();
      QString value = it.value();
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
      addTag(-1, key, value, notOptionalTag);
    }
  }

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

void WorkshopUploader::addTag(int categoryId, const QString &categoryName, const QString &tagName, bool forcedTag)
{
  int row = m_tagTable->rowCount();
  m_tagTable->insertRow(row);

  QTableWidgetItem* categoryItem = new QTableWidgetItem(categoryName);
  categoryItem->setFlags(categoryItem->flags() & ~Qt::ItemIsEditable);
  m_tagTable->setItem(row, 0, categoryItem);

  QTableWidgetItem* tagItem = new QTableWidgetItem(tagName.trimmed());
  tagItem->setFlags(tagItem->flags() | Qt::ItemIsEditable);
  m_tagTable->setItem(row, 1, tagItem);

  QTableWidgetItem* dummyItem = new QTableWidgetItem();
  dummyItem->setFlags(Qt::NoItemFlags);  // fully disabled
  m_tagTable->setItem(row, 2, dummyItem);

  if (!forcedTag)
  {
    QPushButton* removeBtn = new QPushButton("-", this);

    connect(removeBtn, &QPushButton::clicked, this, [this, removeBtn]() {
              for (int i = 0; i < m_tagTable->rowCount(); i++)
              {
                if (m_tagTable->cellWidget(i, 2) == removeBtn)
                {
                  m_tagTable->removeRow(i);
                  break;
                }
              }
            });

    m_tagTable->setCellWidget(row, 2, removeBtn);
  }
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
  for(std::string cat : ApiManager::instance().contentCategories())
  {
    if(cat != "System")
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

    addTag(categoryId, categoryName, tagName, false);
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

  ApiManager::appendField(multiPart, "collection", m_collectionList->currentText());

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
