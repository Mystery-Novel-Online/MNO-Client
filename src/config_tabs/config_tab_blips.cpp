#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

#include <iostream>

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

void config_tab_blips::on_blipSet_currentIndexChanged(int index)
{
  if(m_currentBlip.has_value())
    m_currentBlip.reset();

  m_currentBlip.emplace(BlipConfig(ui->blipSet->currentText().toStdString()));

  if(!m_currentBlip->valid())
    m_currentBlip.reset();
}

