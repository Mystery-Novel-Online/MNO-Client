#ifndef LAYERSELECTIONPANEL_H
#define LAYERSELECTIONPANEL_H

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

  void disableLayerVariants(const QString& layerName);

  void addLayer(const QString& layer, const QString& toggle, LayerSelectionType type);
  void addLayer(const QString& layer, const QString& variation, bool state, LayerSelectionType type);

  QString getVariant(const QString& layerName, const QString& fallback);;
  QString getBaseVariant(const QString& fallback);;

private:
  void addButtonToGrid(QWidget* button);

public slots:
  void layerClicked(int layerId);

private:
  QWidget *m_container;
  QGridLayout *m_layout;

  QList<LayerSelectionData> m_layers = {};
  QMap<QString, QString> m_VariantSwitches = {};
  QMap<QString, QString> m_GlobalVariants = {};

  QString m_baseImageOverride = "";
};

#endif // LAYERSELECTIONPANEL_H
