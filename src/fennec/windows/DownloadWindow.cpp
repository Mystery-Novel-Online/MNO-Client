#include "DownloadWindow.h"
#include "networking/WorkshopDownloadManager.h"

void DownloadWindow::draw()
{
  std::vector<std::shared_ptr<DownloadTask>> tasks = WorkshopDownloadManager::instance().getAllTasks();
  ImGui::Begin("Download Manager");
  for(std::shared_ptr<DownloadTask>& task : tasks)
  {
    ImGui::TextUnformatted(task->destinationPath.toStdString().c_str());
    ImGui::ProgressBar(task->progressPercentage, ImVec2(-FLT_MIN, 0));
    ImGui::Text("%llu / %llu MB (%.1f%%)",
                task->downloadedBytes / (1024 * 1024),
                task->totalBytes / (1024 * 1024),
                task->progressPercentage * 100.0f);
    ImGui::Separator();
  }
  ImGui::End();
}
