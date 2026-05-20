#include "config_tab_discord.h"
#include "ui_config_tab_discord.h"

ConfigTabDiscord::ConfigTabDiscord(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabDiscord)
{
  ui->setupUi(this);
}

ConfigTabDiscord::~ConfigTabDiscord()
{
  delete ui;
}
