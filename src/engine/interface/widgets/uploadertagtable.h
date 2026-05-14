#pragma once
#include <QTableWidget>

class UploaderTagTable : public QTableWidget
{
public:
  UploaderTagTable(QWidget* parent);
  std::function<void(const QString&, const QString&)> onTagPaste;

protected:
  void keyPressEvent(QKeyEvent* event) override
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

private:
  void copySelection()
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

  void pasteSelection()
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

      if (onTagPaste)
        onTagPaste(categoryName, tagName);
    }
  }

};
