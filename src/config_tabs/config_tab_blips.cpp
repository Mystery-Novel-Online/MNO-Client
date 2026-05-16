#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

config_tab_blips::config_tab_blips(QWidget *parent) : QWidget(parent), ui(new Ui::config_tab_blips)
{
  ui->setupUi(this);
}

config_tab_blips::~config_tab_blips()
{
  delete ui;
}
