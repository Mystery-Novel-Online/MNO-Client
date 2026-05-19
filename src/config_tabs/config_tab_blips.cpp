#include "config_tab_blips.h"
#include "ui_config_tab_blips.h"

#include <iostream>

#include <rolechat/filesystem/RCDir.h>
#include "engine/system/audio.h"
config_tab_blips::config_tab_blips(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabBlips)
{

  m_allowCharacters = config::ConfigUserSettings::booleanValue("blip_override_characters", true);
  m_allowTheme = config::ConfigUserSettings::booleanValue("blip_override_themes", true);
  m_useOverrides = config::ConfigUserSettings::booleanValue("blip_override_custom", false);

  int configBlipRate = config::ConfigUserSettings::intergerValue("blip_custom_bliprate", 3000);
  m_useBlanks = config::ConfigUserSettings::booleanValue("blip_custom_blanks", false);

  ui->setupUi(this);
  rolechat::fs::RCDir blipDirectory("sounds/blips/");

  const QString qCurrentBlipSet = QString::fromStdString(config::ConfigUserSettings::stringValue("blip_set", "Danganronpa"));

  ui->blipOverride->setChecked(m_useOverrides);

  ui->overrideRuleTheme->setChecked(m_allowTheme);
  ui->overrideRuleCharacter->setChecked(m_allowCharacters);

  ui->blipRate->setMinimum(1);
  ui->blipRate->setMaximum(999999999);
  ui->blipRate->setValue(configBlipRate);
  m_blipRate = configBlipRate;

  ui->blipBlanks->setChecked(m_useBlanks);

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
  auto blip = activeBlip();

  if(blip)
    return QString::fromStdString("sounds/blips/" + blip->get().name() + "/" + blip->get().soundFile(gender.toStdString()));

  return "sounds/general/sfx-blip" + gender + ".wav";
}

bool config_tab_blips::useBlanks()
{
  if(m_useOverrides)
    return m_useBlanks;

  auto blip = activeBlip();
  if (blip)
    return blip->get().blanksAllowed();

  return false;
}

int config_tab_blips::blipRate()
{
  if(m_useOverrides)
    return m_blipRate;

  auto blip = activeBlip();
  if (blip)
    return blip->get().blipRate();

  return 99999;
}

void config_tab_blips::setCharacterBlip(const std::string &set)
{
  if(set.empty())
  {
    m_characterBlip.reset();
    return;
  }

  if(m_characterBlip.has_value())
  {
    if(m_characterBlip->name() == set)
      return;
    m_characterBlip.reset();
  }

  m_characterBlip.emplace(BlipConfig(set));

  if(!m_characterBlip->valid())
    m_characterBlip.reset();
}

void config_tab_blips::setTagBlip(const std::string &set)
{
  if(set.empty())
  {
    m_tagBlip.reset();
    return;
  }

  if(m_tagBlip.has_value())
  {
    if(m_tagBlip->name() == set)
      return;
    m_tagBlip.reset();
  }

  m_tagBlip.emplace(BlipConfig(set));

  if(!m_tagBlip->valid())
    m_tagBlip.reset();
}

void config_tab_blips::playStartingSfx()
{
  auto blip = activeBlip();
  if (!blip)
    return;

  auto startingSound = blip->get().startingSfx();
  if(startingSound.empty())
    return;

  audio::effect::PlayGlobal(startingSound);
}

void config_tab_blips::playEndingSfx()
{
  auto blip = activeBlip();
  if (!blip)
    return;

  auto endingSound = blip->get().endingSfx();
  if(endingSound.empty())
    return;

  audio::blip::SetSound(endingSound);
  audio::blip::Tick();
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

void config_tab_blips::on_overrideRuleCharacter_stateChanged(int arg1)
{
  m_allowCharacters = ui->overrideRuleCharacter->isChecked();
  config::ConfigUserSettings::setValue("characters_override_blip", m_allowCharacters);
}


void config_tab_blips::on_overrideRuleTheme_stateChanged(int arg1)
{
  m_allowCharacters = ui->overrideRuleTheme->isChecked();
  config::ConfigUserSettings::setValue("blip_override_themes", m_allowCharacters);
}


void config_tab_blips::on_blipOverride_toggled(bool arg1)
{
  m_useOverrides = arg1;
  config::ConfigUserSettings::setValue("blip_override_custom", m_useOverrides);
}


void config_tab_blips::on_blipRate_valueChanged(int arg1)
{
  m_blipRate = arg1;
  config::ConfigUserSettings::setValue("blip_custom_bliprate", arg1);
}


void config_tab_blips::on_blipBlanks_stateChanged(int arg1)
{
  m_useBlanks = ui->blipBlanks->isChecked();
  config::ConfigUserSettings::setValue("blip_custom_blanks", m_useBlanks);
}

std::optional<std::reference_wrapper<const BlipConfig> > config_tab_blips::activeBlip() const
{
  if(m_tagBlip)
    return *m_tagBlip;

  if (m_allowCharacters && m_characterBlip.has_value())
    return *m_characterBlip;

  if (m_currentBlip.has_value())
    return *m_currentBlip;

  return std::nullopt;
}

