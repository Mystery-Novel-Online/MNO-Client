#include "config_tab_callwords.h"
#include "ui_config_tab_callwords.h"

ConfigTabCallwords::ConfigTabCallwords(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabCallwords)
{
  ui->setupUi(this);
}

ConfigTabCallwords::~ConfigTabCallwords()
{
  delete ui;
}
