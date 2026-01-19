#include "layer_selection_panel.h"
#include "drtheme.h"
#include "server_select_entry.h"
#include "engine/param/actor_repository.h"
#include "engine/interface/courtroom_layout.h"

constexpr int ButtonSize   = 40;
constexpr int LayoutMargin = 22;

LayerSelectionPanel::LayerSelectionPanel(QWidget *parent)
    : RPWidget{"layers_panel", parent}
{
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

void LayerSelectionPanel::clearGlobals()
{
  m_GlobalVariants.clear();
}

void LayerSelectionPanel::addButtonToGrid(QWidget *button)
{
  QPoint f_spacing = AOApplication::getInstance()->current_theme->get_widget_settings_spacing("layers_panel", "courtroom", "layers_panel_spacing");
  m_layout->setHorizontalSpacing(f_spacing.x());
  m_layout->setVerticalSpacing(f_spacing.y());

  int maxColumns = (width() - LayoutMargin - f_spacing.x()) / ButtonSize;
  int row = m_layout->count() / maxColumns;
  int col = m_layout->count() % maxColumns;

  button->setFixedSize(ButtonSize, ButtonSize);
  button->show();
  m_layout->addWidget(button, row, col);
}

void LayerSelectionPanel::disableLayerVariants(const QString &layerName)
{
  for (LayerSelectionData& layer : m_layers)
  {
    if (layer.layerName != layerName)
      continue;

    const QString name = layer.layerName + "_" + layer.variation;
    layer.button->setLayerImage(name, name, name, false);
  }
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &toggle, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(m_container, m_app, 0, 0);
  emote->setLayerImage(toggle, toggle, toggle, type == LayerSelection_Toggle);
  emote->set_emote_number(m_layers.count());

  LayerSelectionData data = {layer, toggle, "", emote, type};
  m_layers.append(data);

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  addButtonToGrid(emote);
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &variation, bool state, LayerSelectionType type)
{
  AOEmoteButton *emote = new AOEmoteButton(m_container, m_app, 0, 0);
  emote->set_emote_number(m_layers.count());

  QString name = layer + "_" + variation;

  if(m_GlobalVariants.contains(layer))
    state = m_GlobalVariants[layer] == variation;

  emote->setLayerImage(name, name, name, state);

  LayerSelectionData data = {layer, "", variation, emote, type};
  m_layers.append(data);

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  addButtonToGrid(emote);
}

void LayerSelectionPanel::layerClicked(int layerId)
{
  if(layerId > m_layers.count()) return;
  LayerSelectionData& data = m_layers[layerId];
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
    disableLayerVariants(data.layerName);
    m_VariantSwitches[data.layerName] = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  case LayerSelection_VariationGlobal:
  case LayerSelection_VariationGlobalBase:
    disableLayerVariants(data.layerName);
    m_GlobalVariants[data.layerName] = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  case LayerSelectionType_VariationBase:
    disableLayerVariants(data.layerName);
    baseImage = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  default:
    break;
  }
  courtroom::ic::focusMessageBox();
}

