#include "health_bar.h"


HealthBar::HealthBar(QString name, AOApplication *app, QWidget *parent) : AOImageDisplay{parent, app}, m_App(app) , m_Name(name)
{
  if(name == "defense") m_HealthIndex = 1;
  if(name == "prosecution") m_HealthIndex = 2;
}

void HealthBar::SetValue(int value)
{
  if (value < 0 || value > 10) return;
  m_Value = value;
  set_theme_image(m_Name + "bar" + QString::number(m_Value) + ".png");
}

void HealthBar::OnAddClicked()
{
  int newValue = m_Value + 1;
  if (newValue <= 10) m_App->send_server_packet(DRPacket("HP", {QString::number(m_HealthIndex), QString::number(newValue)}));
}

void HealthBar::OnSubtractClicked()
{
  int newValue = m_Value - 1;
  if (newValue >= 0) m_App->send_server_packet(DRPacket("HP", {QString::number(m_HealthIndex), QString::number(newValue)}));
}
