#include "layer_selection_panel.h"
#include "server_select_entry.h"

LayerSelectionPanel::LayerSelectionPanel(QWidget *parent)
    : RPWidget{"layers_panel", parent}
{
  //setDragable(true);
  resetTransform();
  setBackgroundImage("layers_panel");

  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");


  m_container = new QWidget(scrollArea);
  m_layout = new QGridLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);

  scrollArea->setWidget(m_container);

  QGridLayout *rootLayout = new QGridLayout(this);
  rootLayout->addWidget(scrollArea);
  setLayout(rootLayout);


}

void LayerSelectionPanel::clear()
{
  while (QLayoutItem* item = m_layout->takeAt(0))
  {
    if (QWidget* w = item->widget())
      w->deleteLater();

    delete item;
  }
}

void LayerSelectionPanel::addLayer(const QString &layer, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(nullptr, m_app, 0, 0);
  emote->setFixedSize(40, 40);
  emote->set_emote_number(0);
  emote->setLayerImage(layer, layer, layer, type == LayerSelection_Toggle);
  emote->show();

  const int columns = 4;

  int index = m_layout->count();
  int row = index / columns;
  int col = index % columns;

  m_layout->addWidget(emote, row, col);
}
