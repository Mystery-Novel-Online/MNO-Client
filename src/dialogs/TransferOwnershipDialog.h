#ifndef TRANSFEROWNERSHIPDIALOG_H
#define TRANSFEROWNERSHIPDIALOG_H

#include "network/workshop/workshop_models.h"
class TransferOwnershipDialog : public QDialog
{
  Q_OBJECT
public:
  TransferOwnershipDialog(const WorkshopContentEntry& content, QWidget* parent = nullptr);

private slots:
  void submit();
  void handleReply();

private:
  int m_contentId = -1;

  QLineEdit *m_userValue = nullptr;
  QPushButton *m_submitButton = nullptr;

  QNetworkReply *m_currentReply = nullptr;
};

#endif // TRANSFEROWNERSHIPDIALOG_H
