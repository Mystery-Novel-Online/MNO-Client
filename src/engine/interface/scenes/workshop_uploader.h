
#include <QTableWidget>

#include <engine/interface/widgets/uploadertagtable.h>
#ifndef WORKSHOP_UPLOADER_H
#define WORKSHOP_UPLOADER_H

class WorkshopUploader : public QDialog
{
  Q_OBJECT
public:
  explicit WorkshopUploader(QWidget *parent = nullptr, bool edit = false, int edit_id = -1, QVector<QPair<QString, QString>> tagMap = {});
  static void StartUpload();
  static void StartEdit(int id, QVector<QPair<QString, QString>> tagMap = {});
  void addTag(int categoryId, const QString& categoryName, const QString& tagName, bool forcedTag = false);

private slots:
  void chooseFile();
  void addTagClicked();
  void choosePreviewFile();
  void submitForm();
  void handleReply();
  void updateProgress(qint64 bytesSent, qint64 bytesTotal);

private:
  QLineEdit *m_filePath;
  QLineEdit *m_previewPath;
  QTextEdit *m_description;
  QPushButton *m_addTag;
  QPushButton *m_imageButton;
  QPushButton *m_chooseButton;
  QPushButton *m_submitButton;
  QProgressBar *m_progress;

  QComboBox *m_collectionList;

  UploaderTagTable* m_tagTable;

  bool m_isEdit = false;
  int m_editTarget = -1;
  QCheckBox *m_private;

  QNetworkReply *m_currentReply;
};

#endif // WORKSHOP_UPLOADER_H
