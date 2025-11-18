#include "layer_selection_panel.h"
#include "server_select_entry.h"

LayerSelectionPanel::LayerSelectionPanel(QWidget *parent)
    : RPWidget{"layers_panel", parent}
{
  setDragable(true);
  resetTransform();
  setBackgroundImage("layers_panel");

  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");


  m_container = new QWidget(scrollArea);
  m_layout = new QVBoxLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);

  scrollArea->setWidget(m_container);

  QVBoxLayout *rootLayout = new QVBoxLayout(this);
  rootLayout->addWidget(scrollArea);
  setLayout(rootLayout);

  ServerSelectEntry *entry = new ServerSelectEntry("WOW");
  entry->setId(m_layout->count());
  m_layout->addWidget(entry);

}
