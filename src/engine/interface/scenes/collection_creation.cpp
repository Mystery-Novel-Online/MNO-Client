#include "collection_creation.h"
#include <QFormLayout>
#include <engine/network/api_manager.h>


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
  layout->addRow("Preview Image:", w_IconLineEdit);
  layout->addRow("", w_IconPushBtn);
  layout->addRow("Name:", w_NameLineEdit);
  layout->addRow("Description:", w_DescTextEdit);
  layout->addRow("", w_SbmtPushBtn);
  setLayout(layout);

  //connect(m_chooseButton, &QPushButton::clicked, this, &WorkshopUploader::chooseFile);
  //connect(m_imageButton, &QPushButton::clicked, this, &WorkshopUploader::choosePreviewFile);
  //connect(m_submitButton, &QPushButton::clicked, this, &WorkshopUploader::submitForm);
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
