#include "ConfigTabAccessibility.h"
#include "ui_ConfigTabAccessibility.h"

ConfigTabAccessibility::ConfigTabAccessibility(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabAccessibility)
{
  ui->setupUi(this);
  ui->backgroundDimmingSlider->setValue(config::ConfigUserSettings::intergerValue("background_contrast"));
}

ConfigTabAccessibility::~ConfigTabAccessibility()
{
  delete ui;
}

void ConfigTabAccessibility::on_horizontalSlider_valueChanged(int value)
{
  config::ConfigUserSettings::setValue("background_contrast", ui->backgroundDimmingSlider->value());
}


void ConfigTabAccessibility::on_backgroundDimmingSlider_valueChanged(int value)
{
  config::ConfigUserSettings::setValue("background_contrast", ui->backgroundDimmingSlider->value());
}

