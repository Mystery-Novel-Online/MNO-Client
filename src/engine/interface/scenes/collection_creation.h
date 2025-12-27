#ifndef COLLECTION_CREATION_H
#define COLLECTION_CREATION_H

#include <QDialog>

class CollectionCreation : public QDialog
{
  Q_OBJECT
public:
  explicit CollectionCreation(QWidget *parent = nullptr);
  static void init();


private slots:
  void iconSelection();
  void submitForm();
  void handleReply();
  void updateProgress(qint64 bytesSent, qint64 bytesTotal);

private:
  QLineEdit *w_NameLineEdit = nullptr;
  QTextEdit *w_DescTextEdit = nullptr;
  QLineEdit *w_IconLineEdit = nullptr;
  QPushButton *w_IconPushBtn = nullptr;
  QPushButton *w_SbmtPushBtn = nullptr;
  QNetworkReply *m_currentReply = nullptr;
};

#endif // COLLECTION_CREATION_H
