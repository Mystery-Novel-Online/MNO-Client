#ifndef RENAMECONTENTDIALOG_H
#define RENAMECONTENTDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>

class RenameContentDialog : public QDialog
{
  Q_OBJECT
public:
  RenameContentDialog(int contentId, const QString& contentName, QWidget* parent = nullptr);

private slots:
  void submit();
  void handleReply();

private:
  QLineEdit *m_nameEdit = nullptr;
  QPushButton *m_submitButton = nullptr;

  int m_workshopId = -1;

  QNetworkReply *m_currentReply = nullptr;
};

#endif // RENAMECONTENTDIALOG_H
