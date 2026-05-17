#include "config_tab_callwords.h"
#include "ui_config_tab_callwords.h"
#include <rolechat/userdata/RolechatDatabase.h>
#include <vector>

ConfigTabCallwords::ConfigTabCallwords(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabCallwords)
{
  ui->setupUi(this);

  ui->callwordTable->setColumnCount(3);

  QStringList headers;
  headers << "Word" << "Match Mode" << "";

  ui->callwordTable->setHorizontalHeaderLabels(headers);

  ui->callwordTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui->callwordTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  ui->callwordTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

  ui->callwordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->callwordTable->setSelectionMode(QAbstractItemView::SingleSelection);

  for(auto& callword : RolechatDatabase::instance().getCallwords())
  {
    addWord(QString::fromStdString(callword.word), (WordMatchMode)callword.mode);
  }

}

ConfigTabCallwords::~ConfigTabCallwords()
{
  delete ui;
}

void ConfigTabCallwords::addWord(const QString &word, WordMatchMode mode)
{
  int row = ui->callwordTable->rowCount();
  ui->callwordTable->insertRow(row);

  QTableWidgetItem* wordItem = new QTableWidgetItem(word);
  ui->callwordTable->setItem(row, 0, wordItem);

  QComboBox* combo = new QComboBox(ui->callwordTable);
  connect(combo, &QComboBox::currentTextChanged, this, [this]() {save();});

  combo->addItem("Contains Anywhere",
                 static_cast<int>(WordMatchMode::ContainsAnywhere));

  combo->addItem("Whole Word Only",
                 static_cast<int>(WordMatchMode::WholeWordOnly));

  combo->setCurrentIndex(static_cast<int>(mode));

  ui->callwordTable->setCellWidget(row, 1, combo);


  QPushButton* removeButton = new QPushButton("Remove");

  connect(removeButton, &QPushButton::clicked, this,
          [this, removeButton]()
          {
            for (int row = 0; row < ui->callwordTable->rowCount(); ++row)
            {
              if (ui->callwordTable->cellWidget(row, 2) == removeButton)
              {
                ui->callwordTable->removeRow(row);
                save();
                return;
              }
            }
          });

  ui->callwordTable->setCellWidget(row, 2, removeButton);
  save();
}

bool ConfigTabCallwords::matchWord(const QString &message, const QString &word, WordMatchMode mode)
{
  switch (mode)
  {
    case WordMatchMode::ContainsAnywhere:
    {
      return message.contains(word, Qt::CaseInsensitive);
    }

    case WordMatchMode::WholeWordOnly:
    {
      QString pattern =
          QString("\\b%1\\b")
              .arg(QRegularExpression::escape(word));

      QRegularExpression regex(
          pattern,
          QRegularExpression::CaseInsensitiveOption);

      return regex.match(message).hasMatch();
    }
  }

  return false;
}

bool ConfigTabCallwords::messageCheck(const QString &message, QString& matchedWord)
{
  QTableWidget* table = ui->callwordTable;

  for (int row = 0; row < table->rowCount(); ++row)
  {
    QTableWidgetItem* wordItem = table->item(row, 0);

    if (!wordItem)
      continue;

    const QString word = wordItem->text().trimmed();

    if (word.isEmpty())
      continue;

    QComboBox* combo = qobject_cast<QComboBox*>(table->cellWidget(row, 1));

    if (!combo)
      continue;

    WordMatchMode mode = static_cast<WordMatchMode>(combo->currentData().toInt());

    if (matchWord(message, word, mode))
    {
      matchedWord = word;
      return true;
    }
  }

  return false;
}

void ConfigTabCallwords::on_callwordAdd_released()
{
  if(ui->callwordInput->text().trimmed().isEmpty())
    return;

  addWord(ui->callwordInput->text(), WordMatchMode::ContainsAnywhere);
}

void ConfigTabCallwords::save()
{
  QTableWidget* table = ui->callwordTable;
  std::vector<UserCallword> data = {};

  for (int row = 0; row < table->rowCount(); ++row)
  {
    QTableWidgetItem* wordItem = table->item(row, 0);

    if (!wordItem)
      continue;

    const QString word = wordItem->text().trimmed();

    if (word.isEmpty())
      continue;

    QComboBox* combo = qobject_cast<QComboBox*>(table->cellWidget(row, 1));

    if (!combo)
      continue;

    WordMatchMode mode = static_cast<WordMatchMode>(combo->currentData().toInt());
    data.push_back({word.toStdString(), (int)mode});
    RolechatDatabase::instance().setCallwords(data);
  }
}

