#include "LayerSelectionPanel.h"
#include "drtheme.h"
#include "engine/param/actor_repository.h"
#include "engine/interface/courtroom_layout.h"

constexpr int ButtonSize   = 40;
constexpr int LayoutMargin = 22;

LayerSelectionPanel::LayerSelectionPanel(QWidget *parent)
    : RPWidget{"layers_panel", parent} {
  resetTransform();
  setBackgroundImage("layers_panel");

  m_iconScrollArea = new QScrollArea(this);
  m_iconScrollArea->setWidgetResizable(true);
  m_iconScrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");

  m_container = new QWidget(m_iconScrollArea);
  m_layout = new QGridLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);

  m_iconScrollArea->setWidget(m_container);

  QGridLayout *rootLayout = new QGridLayout(this);
  rootLayout->addWidget(m_iconScrollArea);
  setLayout(rootLayout);
}

void LayerSelectionPanel::clear() {
  m_layers.clear();
  m_VariantSwitches.clear();
  m_baseImageOverride.clear();
  while(QLayoutItem* item = m_layout->takeAt(0))
  {
    if(QWidget* w = item->widget()) {
      w->deleteLater();
    }
    delete item;
  }
}

void LayerSelectionPanel::clearGlobals() {
  m_GlobalVariants.clear();
}

void LayerSelectionPanel::addButtonToGrid(QWidget *button) {
  if(width() <= 0) {
    return;
  }

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

void LayerSelectionPanel::disableLayerVariants(const QString &layerName) {
  for(LayerSelectionData& layer : m_layers) {
    if(layer.layerName != layerName) {
      continue;
    }

    const QString name = layer.layerName + "_" + layer.variation;
    layer.button->setLayerImage(name, name, name, false);
  }
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &toggle, LayerSelectionType type) {
  AOEmoteButton *emote = new AOEmoteButton(m_container, m_app, 0, 0);
  emote->setLayerImage(toggle, toggle, toggle, type == LayerSelectionType::Toggle);
  emote->set_emote_number(m_layers.count());

  LayerSelectionData data = {layer, toggle, "", emote, type};
  m_layers.append(data);

  connect(emote, &AOEmoteButton::emote_clicked, this, &LayerSelectionPanel::layerClicked);
  addButtonToGrid(emote);
}

void LayerSelectionPanel::addLayer(const QString &layer, const QString &variation, bool state, LayerSelectionType type) {
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

QString LayerSelectionPanel::getVariant(const QString &layerName, const QString &fallback) {
  if(m_GlobalVariants.contains(layerName)) {
    return m_GlobalVariants[layerName];
  }
  if(m_VariantSwitches.contains(layerName)) {
    return m_VariantSwitches[layerName];
  }
  return fallback;
}

QString LayerSelectionPanel::getBaseVariant(const QString &fallback) {
  if(m_GlobalVariants.contains("base_image")) {
    return m_GlobalVariants["base_image"];
  }
  if(m_baseImageOverride.trimmed().isEmpty()) {
    return fallback;
  }
  return m_baseImageOverride;
}

void LayerSelectionPanel::moveToCursor(const QPoint& position) {
  QPoint calculatedPosition(position.x() - (width() / 2), position.y() - (height() / 2));
  if(calculatedPosition.x() < 0) {
    calculatedPosition.setX(0);
  }
  if(calculatedPosition.y() < 0) {
    calculatedPosition.setY(0);
  }
  move(calculatedPosition.x(), calculatedPosition.y());
  raise();
  show();
}

void LayerSelectionPanel::toggleShortcutMode(bool state) {
  m_shortcutMode = state;
  if(state) {
    // TODO: Create a little border around the window so it looks nice when called up
    m_iconScrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");
  }
  else {
    m_iconScrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");
  }
}

void LayerSelectionPanel::layerClicked(int layerId) {
  if(layerId > m_layers.count()) {
    return;
  }

  LayerSelectionData& data = m_layers[layerId];
  QString VariantName = data.layerName + "_" + data.variation;

  if(QApplication::keyboardModifiers() & Qt::AltModifier) {
    AOApplication::getInstance()->m_courtroom->ui_ic_chat_message_field->addTag(CueType::Layer, { data.layerName, data.variation });
    emit interactionFinished();
    return;
  }

  switch(data.type) {
  case LayerSelectionType::Toggle:
    data.type = LayerSelectionType::ToggleDisabled;
    data.button->setLayerImage(data.toggleName, data.toggleName, data.toggleName, false);
    engine::actor::user::toggleLayer(data.toggleName.toStdString(), false);
    break;

  case LayerSelectionType::ToggleDisabled:
    data.type = LayerSelectionType::Toggle;
    data.button->setLayerImage(data.toggleName, data.toggleName, data.toggleName, true);
    engine::actor::user::toggleLayer(data.toggleName.toStdString(), true);
    break;

  case LayerSelectionType::Variation:
    disableLayerVariants(data.layerName);
    m_VariantSwitches[data.layerName] = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  case LayerSelectionType::VariationGlobal:
  case LayerSelectionType::VariationGlobalBase:
    disableLayerVariants(data.layerName);
    m_GlobalVariants[data.layerName] = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  case LayerSelectionType::VariationBase:
    disableLayerVariants(data.layerName);
    m_baseImageOverride = data.variation;
    data.button->setLayerImage(VariantName, VariantName, VariantName, true);
    break;

  default:
    break;
  }
  emit interactionFinished();
}


bool LayerSelectionPanel::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::Leave:
    if(m_shortcutMode)
      hide();
    break;

  default:
    break;
  }

  return RPWidget::event(event);
}

