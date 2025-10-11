#ifndef WORKSHOP_UPLOADER_H
#define WORKSHOP_UPLOADER_H

class WorkshopUploader : public QDialog
{
  Q_OBJECT
public:
  explicit WorkshopUploader(QWidget *parent = nullptr, bool edit = false, int edit_id = -1);
  static void StartUpload();
  static void StartEdit(int id);

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

  bool m_isEdit = false;
  int m_editTarget = -1;
  QCheckBox *m_private;

  QNetworkAccessManager *m_network;
  QNetworkReply *m_currentReply;
};

#endif // WORKSHOP_UPLOADER_H
