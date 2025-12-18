#include "layer_selection_panel.h"
#include "server_select_entry.h"
#include "engine/param/actor_repository.h"
#include "engine/interface/courtroom_layout.h"

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
  m_VariantSwitches.clear();
  baseImage.clear();
  while (QLayoutItem* item = m_layout->takeAt(0))
  {
    if (QWidget* w = item->widget())
      w->deleteLater();

    delete item;
  }
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &toggle, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(m_container, m_app, 0, 0);
  LayerSelectionData data = {layer, toggle, "", emote, type};
  emote->setFixedSize(40, 40);
  emote->set_emote_number(m_layers.count());
  m_layers.append(data);

  emote->setLayerImage(toggle, toggle, toggle, type == LayerSelection_Toggle);
  emote->show();



  const int columns = (width() - 22) / 40;

  int index = m_layout->count();
  int row = index / columns;
  int col = index % columns;

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  m_layout->addWidget(emote, row, col);
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &variation, bool state, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(m_container, m_app, 0, 0);
  LayerSelectionData data = {layer, "", variation, emote, type};
  emote->setFixedSize(40, 40);
  emote->set_emote_number(m_layers.count());
  m_layers.append(data);

  QString name = layer + "_" + variation;
  emote->setLayerImage(name, name, name, state);
  emote->show();

  const int columns = (width() - 22) / 40;

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
  QString VariantName = data.layerName + "_" + data.variation;
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

  case LayerSelection_Variation:
    for(int i = 0; i < m_layers.count(); i++)
    {
      QString name = m_layers.at(i).layerName + "_" + m_layers.at(i).variation;
      if(m_layers.at(i).layerName == data.layerName)
      {
        m_layers.at(i).button->setLayerImage(name, name, name, false);
      }
    }
    m_VariantSwitches[data.layerName] = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  case LayerSelectionType_VariationBase:
    for(int i = 0; i < m_layers.count(); i++)
    {
      QString name = m_layers.at(i).layerName + "_" + m_layers.at(i).variation;
      if(m_layers.at(i).layerName == data.layerName)
      {
        m_layers.at(i).button->setLayerImage(name, name, name, false);
      }
    }
    baseImage = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  default:
    break;
  }
  m_layers[layerId] = data;
  courtroom::ic::focusMessageBox();
}

