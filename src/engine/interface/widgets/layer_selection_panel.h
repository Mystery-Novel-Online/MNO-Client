#ifndef LAYER_SELECTION_PANEL_H
#define LAYER_SELECTION_PANEL_H

class LayerSelectionPanel : public RPWidget
{
  Q_OBJECT
public:
  LayerSelectionPanel(QWidget *parent = nullptr);

private:
  QWidget *m_container;
  QVBoxLayout *m_layout;
};

#endif // LAYER_SELECTION_PANEL_H
