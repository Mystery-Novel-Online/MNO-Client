#pragma once
#include <QTableWidget>

class UploaderTagTable : public QTableWidget
{
public:
  UploaderTagTable(QWidget* parent);
  std::function<void(const QString&, const QString&)> onTagPaste;

protected:
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void itemChangeEvent(QTableWidgetItem* item);

private:
  void copySelection();

  void pasteSelection();

};
