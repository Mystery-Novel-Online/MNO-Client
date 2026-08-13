#ifndef WORKSHOPDOWNLOADMANAGER_H
#define WORKSHOPDOWNLOADMANAGER_H

enum class DownloadStatus
{
  Queued,
  Downloading,
  Paused,
  Unpacking,
  Completed,
  Failed,
  Cancelled
};

class DownloadTask
{
public:
  QString guid;
  QString originUrl;
  QString destinationPath;

  long long totalBytes = 0;
  long long downloadedBytes = 0;

  DownloadStatus status = DownloadStatus::Queued;
  int errorCode = 0;

  double currentSpeed = 0.0;
  double progressPercentage = 0.0;

  std::chrono::steady_clock::time_point startTime;
  std::chrono::steady_clock::time_point lastUpdateTime;

  void updateProgress(long long bytesRecieved);
  void calculateSpeed();
  double getEta() const;
};

class WorkshopDownloadManager
{
public:
  static WorkshopDownloadManager& instance();
  WorkshopDownloadManager() = default;

  std::shared_ptr<DownloadTask> addTask(const QString& url, const QString& destination, const QString& guid);
  std::shared_ptr<DownloadTask> getTask(const QString& guid);
  std::vector<std::shared_ptr<DownloadTask>> getAllTasks();

private:
  std::unordered_map<QString, std::shared_ptr<DownloadTask>> m_tasksMap;

};

#endif // WORKSHOPDOWNLOADMANAGER_H
