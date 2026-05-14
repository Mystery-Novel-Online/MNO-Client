#include "uploadertagtable.h"

UploaderTagTable::UploaderTagTable(QWidget *parent) : QTableWidget(parent)
{
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
}
