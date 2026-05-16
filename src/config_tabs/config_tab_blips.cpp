#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

#include <iostream>

#include <rolechat/filesystem/RCDir.h>

config_tab_blips::config_tab_blips(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabBlips)
{
  ui->setupUi(this);
  rolechat::fs::RCDir blipDirectory("sounds/blips/");

  const QString qCurrentBlipSet = QString::fromStdString(config::ConfigUserSettings::stringValue("blip_set", "default"));

  std::optional<int> lBlipIndex;

  for(const auto& subDir : blipDirectory.subDirectories())
  {
    const QString qBlipName = QString::fromStdString(subDir);
    if (qBlipName == qCurrentBlipSet)
      lBlipIndex = ui->blipSet->count();

    ui->blipSet->addItem(qBlipName);
  }

  if (lBlipIndex.has_value())
    ui->blipSet->setCurrentIndex(*lBlipIndex);
}

config_tab_blips::~config_tab_blips()
{
  delete ui;
}

QString config_tab_blips::getBlipSound(const QString &gender)
{
  if(m_currentBlip.has_value())
    return QString::fromStdString("sounds/blips/" + m_currentBlip->name() + "/" + m_currentBlip->soundFile(gender.toStdString()));

  return "sounds/general/sfx-blip" + gender + ".wav";
}

void config_tab_blips::on_blipSet_currentIndexChanged(int index)
{
  if(m_currentBlip.has_value())
    m_currentBlip.reset();

  m_currentBlip.emplace(BlipConfig(ui->blipSet->currentText().toStdString()));

  if(!m_currentBlip->valid())
    m_currentBlip.reset();

  config::ConfigUserSettings::setString("blip_set", ui->blipSet->currentText().toStdString());
}

