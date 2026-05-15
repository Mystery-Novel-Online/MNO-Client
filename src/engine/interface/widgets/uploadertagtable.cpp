#include "uploadertagtable.h"

#include <QHeaderView>

UploaderTagTable::UploaderTagTable(QWidget *parent) : QTableWidget(parent)
{
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  setColumnCount(3);

  QStringList headers;
  headers << "Category" << "Tag" << "";
  setHorizontalHeaderLabels(headers);

  horizontalHeader()->setStretchLastSection(false);
  horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  setColumnWidth(2, 80);

  setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed );

  connect(this, &QTableWidget::itemChanged, this, &UploaderTagTable::itemChangeEvent);
}

void UploaderTagTable::addTag(const QString &category, const QString &value, bool disableRemove)
{
  int row = rowCount();
  insertRow(row);

  QTableWidgetItem* categoryItem = new QTableWidgetItem(category);
  categoryItem->setFlags(categoryItem->flags() & ~Qt::ItemIsEditable);
  setItem(row, 0, categoryItem);

  QTableWidgetItem* tagItem = new QTableWidgetItem(value.trimmed());
  tagItem->setFlags(tagItem->flags() | Qt::ItemIsEditable);
  setItem(row, 1, tagItem);

  QTableWidgetItem* dummyItem = new QTableWidgetItem();
  dummyItem->setFlags(Qt::NoItemFlags);
  setItem(row, 2, dummyItem);

  if (!disableRemove)
  {
    QPushButton* removeBtn = new QPushButton("Remove", this);

    connect(removeBtn, &QPushButton::clicked, this, [this, removeBtn]() {
              for (int i = 0; i < rowCount(); i++)
              {
                if (cellWidget(i, 2) == removeBtn)
                {
                  removeRow(i);
                  break;
                }
              }
            });

    setCellWidget(row, 2, removeBtn);
  }
}


void UploaderTagTable::keyPressEvent(QKeyEvent *event)
{
  if (event->matches(QKeySequence::Copy))
  {
    copySelection();
    return;
  }

  if (event->matches(QKeySequence::Paste))
  {
    pasteSelection();
    return;
  }

  QTableWidget::keyPressEvent(event);
}

void UploaderTagTable::itemChangeEvent(QTableWidgetItem *item)
{
  if (item->column() != 1) return;
  item->setText(item->text().trimmed());
}

void UploaderTagTable::copySelection()
{
  QModelIndexList indexes = selectedIndexes();

  if (indexes.isEmpty())
    return;

  std::sort(indexes.begin(), indexes.end());

  QString text;
  int previousRow = indexes.first().row();

  for (const QModelIndex& index : indexes)
  {
    if (text.length() > 0)
    {
      if (index.row() != previousRow)
      {
        text += '\n';
        previousRow = index.row();
      }
      else
      {
        text += '\t';
      }
    }

    text += index.data().toString();
  }

  QGuiApplication::clipboard()->setText(text);
}

void UploaderTagTable::pasteSelection()
{
  QString text = QGuiApplication::clipboard()->text();

  QStringList lines = text.split('\n', Qt::SkipEmptyParts);

  for (QString line : lines)
  {
    line = line.trimmed();

    if (line.isEmpty())
      continue;

    QStringList parts = line.split('\t');

    if (parts.size() < 2)
      continue;

    QString categoryName = parts[0].trimmed();
    QString tagName = parts[1].trimmed();

    addTag(categoryName, tagName);
  }
}
