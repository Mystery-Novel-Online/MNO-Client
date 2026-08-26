#include "WorkshopDownloadManager.h"

WorkshopDownloadManager &WorkshopDownloadManager::instance()
{
  static WorkshopDownloadManager instance;
  return instance;
}

std::shared_ptr<DownloadTask> WorkshopDownloadManager::addTask(const QString &url, const QString &destination, const QString &guid)
{
  auto task = std::make_shared<DownloadTask>();
  task->originUrl = url;
  task->destinationPath = destination;
  task->guid = guid;

  m_tasksMap[guid] = task;
  return task;

}

std::shared_ptr<DownloadTask> WorkshopDownloadManager::getTask(const QString &guid)
{
  auto it = m_tasksMap.find(guid);
  if(it != m_tasksMap.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<DownloadTask> > WorkshopDownloadManager::getAllTasks()
{
  std::vector<std::shared_ptr<DownloadTask>> task_list;
  task_list.reserve(m_tasksMap.size());

  for(const auto& [id, task_ptr] : m_tasksMap) {
    task_list.push_back(task_ptr);
  }
  return task_list;
}
