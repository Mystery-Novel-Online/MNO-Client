#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

#include <iostream>
#include <rolechat-lib/src/rolechat/audio/blip_config.h>

#include <rolechat/filesystem/RCDir.h>

config_tab_blips::config_tab_blips(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabBlips)
{
  ui->setupUi(this);
  rolechat::fs::RCDir blipDirectory("sounds/blips/");
  for(const auto& subDir : blipDirectory.subDirectories())
  {
    ui->blipSet->addItem(QString::fromStdString(subDir));
  }
  BlipConfig blip("danganronpa");
}

config_tab_blips::~config_tab_blips()
{
  delete ui;
}
