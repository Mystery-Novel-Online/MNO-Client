#include "rp_slider.h"

RPSlider::RPSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, &QSlider::customContextMenuRequested, this, [parent, this](const QPoint &pos)
  {
    bool ok;
    int val = QInputDialog::getInt(parent, "Set Value", "Value:", value(), minimum(), maximum(), 1, &ok);
    if (ok) setValue(val);
  });
}
