#ifndef LAYER_SELECTION_PANEL_H
#define LAYER_SELECTION_PANEL_H

enum LayerSelectionType
{
  LayerSelection_Toggle,
  LayerSelection_ToggleDisabled,
  LayerSelection_Variation
};

struct LayerSelectionData
{
  QString layerName = "";
  QString toggleName = "";
  QString variation = "";

  AOEmoteButton* button = nullptr;
  LayerSelectionType type;
};

class LayerSelectionPanel : public RPWidget
{
  Q_OBJECT
public:
  LayerSelectionPanel(QWidget *parent = nullptr);
  void clear();
  void addLayer(const QString& layer, const QString& toggle, LayerSelectionType type);
  void addLayer(const QString& layer, const QString& variation, bool state, LayerSelectionType type);

  QString getVariant(const QString& layerName, const QString& fallback) { if(!m_VariantSwitches.contains(layerName)) return fallback; return m_VariantSwitches[layerName];};

public slots:
  void layerClicked(int layerId);

private:
  QWidget *m_container;
  QGridLayout *m_layout;

  QList<LayerSelectionData> m_layers = {};
  QMap<QString, QString> m_VariantSwitches = {};
};

#endif // LAYER_SELECTION_PANEL_H
