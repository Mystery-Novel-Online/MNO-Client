#include "workshop_tags.h"

WorkshopTags::WorkshopTags(QWidget *parent) : QWidget(parent)
{
  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setFrameShape(QFrame::NoFrame);

  m_container = new QWidget();
  m_layout = new QHBoxLayout(m_container);

  m_layout->setContentsMargins(6, 6, 6, 6);
  m_layout->setSpacing(7);
  m_layout->setAlignment(Qt::AlignLeft);

  m_scrollArea->setWidget(m_container);

  QHBoxLayout* root = new QHBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->addWidget(m_scrollArea);
  setLayout(root);

  m_scrollArea->setStyleSheet(R"(
  QScrollBar:horizontal {
      height: 4px;
      background: transparent;
      margin: 0px;
  }

  QScrollBar::handle:horizontal {
      background: #3a3a3a;
      border-radius: 2px;
      min-width: 30px;
  }

  QScrollBar::handle:horizontal:hover {
      background: #5a5a5a;
  }

  QScrollBar::add-line:horizontal,
  QScrollBar::sub-line:horizontal {
      width: 0px;
      background: none;
  }

  QScrollBar::add-page:horizontal,
  QScrollBar::sub-page:horizontal {
      background: none;
  }
  )");
}

WorkshopTags::~WorkshopTags()
{
}

void WorkshopTags::addTag(const QString &text)
{
  QLabel* label = new QLabel(text);

  label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  label->setStyleSheet(R"(
        QLabel {
            background-color: transparent;
            color: #9A99A7;
            border: 2px solid #9A99A7;
            border-radius: 10px;
            padding: 4px 10px;
        }
        QLabel:hover {
            background-color: #3a3a3a;
        }
    )");

  QFont font = label->font();
  font.setBold(true);
  font.setPointSize(9);
  label->setFont(font);

  m_layout->addWidget(label);
}

void WorkshopTags::clearAllTags()
{
  while (QLayoutItem* item = m_layout->takeAt(0))
  {
    if (QWidget* widget = item->widget())
    {
      widget->deleteLater();
    }
    delete item;
  }
}

