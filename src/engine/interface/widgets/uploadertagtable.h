#pragma once
#include <QTableWidget>

class UploaderTagTable : public QTableWidget
{
public:
  UploaderTagTable(QWidget* parent);

  void addTag(const QString& category, const QString& value, bool disableRemove = false);

protected:
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void itemChangeEvent(QTableWidgetItem* item);

private:
  void copySelection();
  void pasteSelection();

};
