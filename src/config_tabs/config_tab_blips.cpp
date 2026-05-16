#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

config_tab_blips::config_tab_blips(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabBlips)
{
  ui->setupUi(this);
  ui->blipSet->addItem("<Legacy Blips>");
}

config_tab_blips::~config_tab_blips()
{
  delete ui;
}
