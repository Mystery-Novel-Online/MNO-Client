#ifndef LAYER_SELECTION_PANEL_H
#define LAYER_SELECTION_PANEL_H

enum LayerSelectionType
{
  LayerSelection_Toggle,
  LayerSelection_ToggleDisabled,
  LayerSelection_Variation
};
class LayerSelectionPanel : public RPWidget
{
  Q_OBJECT
public:
  LayerSelectionPanel(QWidget *parent = nullptr);
  void clear();
  void addLayer(const QString& layer, LayerSelectionType type);

private:
  QWidget *m_container;
  QGridLayout *m_layout;
};

#endif // LAYER_SELECTION_PANEL_H
