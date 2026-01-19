#ifndef LAYER_SELECTION_PANEL_H
#define LAYER_SELECTION_PANEL_H

enum LayerSelectionType
{
  LayerSelection_Toggle,
  LayerSelection_ToggleDisabled,
  LayerSelection_Variation,
  LayerSelectionType_VariationBase,
  LayerSelection_VariationGlobal,
  LayerSelection_VariationGlobalBase
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
  void clearGlobals();
  void addLayer(const QString& layer, const QString& toggle, LayerSelectionType type);
  void addLayer(const QString& layer, const QString& variation, bool state, LayerSelectionType type);

  QString getVariant(const QString& layerName, const QString& fallback) { if(m_GlobalVariants.contains(layerName)) return m_GlobalVariants[layerName]; if(m_VariantSwitches.contains(layerName)) return m_VariantSwitches[layerName]; return fallback; };
  QString getBaseVariant(const QString& fallback) { if(m_GlobalVariants.contains("base_image")) return m_GlobalVariants["base_image"]; if(baseImage.trimmed().isEmpty()) return fallback; return baseImage;};

public slots:
  void layerClicked(int layerId);

private:
  QWidget *m_container;
  QGridLayout *m_layout;

  QList<LayerSelectionData> m_layers = {};
  QString baseImage = "";
  QMap<QString, QString> m_VariantSwitches = {};
  QMap<QString, QString> m_GlobalVariants = {};
};

#endif // LAYER_SELECTION_PANEL_H
