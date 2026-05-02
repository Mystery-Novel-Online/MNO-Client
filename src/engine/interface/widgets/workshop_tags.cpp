#include "workshop_tags.h"

WorkshopTags::WorkshopTags(QWidget *parent) : QWidget(parent)
{
  m_layout = new QGridLayout(this);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(6);
  setLayout(m_layout);
}

WorkshopTags::~WorkshopTags()
{
}

void WorkshopTags::addTag(const QString &text)
{
  QLabel* label = new QLabel(text);

  label->setStyleSheet(R"(
        QLabel {
            background-color: #2b2b2b;
            color: #eaeaea;
            border: 1px solid #444;
            border-radius: 10px;
            padding: 4px 10px;
        }
        QLabel:hover {
            background-color: #3a3a3a;
        }
    )");

  label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  int row = 0;
  int col = 0;

  while (m_layout->itemAtPosition(row, col))
  {
    ++col;
    if (col >= m_maxCols)
    {
      col = 0;
      ++row;
    }
  }

  m_layout->addWidget(label, row, col);
}

