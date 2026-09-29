#include "ConfigTabAccessibility.h"
#include "ui_ConfigTabAccessibility.h"

ConfigTabAccessibility::ConfigTabAccessibility(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabAccessibility)
{
  ui->setupUi(this);
}

ConfigTabAccessibility::~ConfigTabAccessibility()
{
  delete ui;
}
