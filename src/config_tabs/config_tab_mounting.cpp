#include "config_tab_mounting.h"
#include "engine/system/user_database.h"
#include "ui_config_tab_mounting.h"

ConfigTabMounting::ConfigTabMounting(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabMounting)
{
  ui->setupUi(this);
  updateList();
  connect(ui->addMount, &QAbstractButton::clicked, this, &ConfigTabMounting::addMountClicked);
  connect(ui->removeMount, &QAbstractButton::clicked, this, &ConfigTabMounting::removeMountClicked);
  connect(ui->mountedDriveList, &QListWidget::itemChanged, this, &ConfigTabMounting::mountStateChanged);
}

ConfigTabMounting::~ConfigTabMounting()
{
  delete ui;
}

void ConfigTabMounting::mountStateChanged(QListWidgetItem *item)
{
  GetDB().toggleMount(item->text().toStdString(), item->checkState() == Qt::Checked);
  updateList();
}

void ConfigTabMounting::addMountClicked()
{

  QString dir = QFileDialog::getExistingDirectory(
      this,
      "Select Folder to Mount",
      QDir::homePath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
      );

  if (dir.isEmpty())
    return;

  GetDB().toggleMount(dir.toStdString(), true);


  updateList();
}

void ConfigTabMounting::removeMountClicked()
{
  QListWidgetItem* item = ui->mountedDriveList->currentItem();

  if (item)
    GetDB().removeMount(item->text().toStdString());

  updateList();
}

void ConfigTabMounting::updateList()
{
  ui->mountedDriveList->blockSignals(true);
  ui->mountedDriveList->clear();

  for(auto mount : GetDB().mountedDirectories())
  {
    QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(mount.directory),  ui->mountedDriveList);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(mount.active ? Qt::Checked : Qt::Unchecked);
  }
  ui->mountedDriveList->blockSignals(false);
}
