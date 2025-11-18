#include "layer_selection_panel.h"
#include "server_select_entry.h"
#include "engine/param/actor_repository.h"

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
  m_layers.clear();
  while (QLayoutItem* item = m_layout->takeAt(0))
  {
    if (QWidget* w = item->widget())
      w->deleteLater();

    delete item;
  }
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &toggle, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(nullptr, m_app, 0, 0);
  LayerSelectionData data = {layer, toggle, "", emote, type};
  emote->setFixedSize(40, 40);
  emote->set_emote_number(m_layers.count());
  m_layers.append(data);

  emote->setLayerImage(toggle, toggle, toggle, type == LayerSelection_Toggle);
  emote->show();

  const int columns = 4;

  int index = m_layout->count();
  int row = index / columns;
  int col = index % columns;

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  m_layout->addWidget(emote, row, col);
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &variation, bool state, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(nullptr, m_app, 0, 0);
  LayerSelectionData data = {layer, "", variation, emote, type};
  emote->setFixedSize(40, 40);
  emote->set_emote_number(m_layers.count());
  m_layers.append(data);
  emote->setLayerImage(layer, layer, layer, state);
  emote->show();

  const int columns = 4;

  int index = m_layout->count();
  int row = index / columns;
  int col = index % columns;

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  m_layout->addWidget(emote, row, col);

}

void LayerSelectionPanel::layerClicked(int layerId)
{
  if(layerId > m_layers.count()) return;
  LayerSelectionData data = m_layers.at(layerId);
  switch(data.type)
  {
  case LayerSelection_Toggle:
    data.type = LayerSelection_ToggleDisabled;
    data.button->setLayerImage(data.toggleName, data.toggleName, data.toggleName, false);
    engine::actor::user::toggleLayer(data.toggleName.toStdString(), false);
    break;

  case LayerSelection_ToggleDisabled:
    data.type = LayerSelection_Toggle;
    data.button->setLayerImage(data.toggleName, data.toggleName, data.toggleName, true);
    engine::actor::user::toggleLayer(data.toggleName.toStdString(), true);
    break;

  default:
    break;
  }
  m_layers[layerId] = data;
}

