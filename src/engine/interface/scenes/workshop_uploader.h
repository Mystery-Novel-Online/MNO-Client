#ifndef WORKSHOP_UPLOADER_H
#define WORKSHOP_UPLOADER_H

class WorkshopUploader : public QDialog
{
  Q_OBJECT
public:
  explicit WorkshopUploader(QWidget *parent = nullptr);
  static void StartUpload();

private slots:
  void chooseFile();
  void submitForm();
  void handleReply(QNetworkReply *reply);
  void updateProgress(qint64 bytesSent, qint64 bytesTotal);

private:
  QLineEdit *m_filePath;
  QLineEdit *m_artist;
  QTextEdit *m_description;
  QPushButton *m_chooseButton;
  QPushButton *m_submitButton;
  QProgressBar *m_progress;

  QNetworkAccessManager *m_network;
  QNetworkReply *m_currentReply;
};

#endif // WORKSHOP_UPLOADER_H
