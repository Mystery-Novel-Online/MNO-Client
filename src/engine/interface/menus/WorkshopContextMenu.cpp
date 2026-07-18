#include "WorkshopContextMenu.h"
#include "dialogs/RenameContentDialog.h"
#include <engine/interface/scenes/workshop_uploader.h>
#include <engine/network/api_manager.h>

WorkshopMenu::WorkshopMenu()
{
  m_ActEdit = addAction(tr("Edit"));
  m_ActOpenContent = addAction(tr("Open in File Explorer"));
  m_ActCopyGuid = addAction(tr("Copy GUID"));

  connect(m_ActEdit, &QAction::triggered, this, &WorkshopMenu::OnEditAction);
  connect(m_ActOpenContent, &QAction::triggered, this, &WorkshopMenu::OnOpenAction);
  connect(m_ActCopyGuid, &QAction::triggered, this, &WorkshopMenu::OnCopyAction);
  if(ApiManager::instance().userPermission() > APIPerms_Auto)
  {
    m_MenuMod = addMenu("Moderation");
    m_ActModApprove = m_MenuMod->addAction("Approve");
    m_ActModSilentApproval = m_MenuMod->addAction("Silent Approval");
    m_ActModUnapprove = m_MenuMod->addAction("Un-approve");
    m_ActModRename = m_MenuMod->addAction("Rename");
    m_ActModDelete = m_MenuMod->addAction("Delete");

    connect(m_ActModApprove, &QAction::triggered, this, &WorkshopMenu::OnApprove);
    connect(m_ActModSilentApproval, &QAction::triggered, this, &WorkshopMenu::OnApproveSilent);
    connect(m_ActModUnapprove, &QAction::triggered, this, &WorkshopMenu::OnUnapprove);
    connect(m_ActModRename, &QAction::triggered, this, &WorkshopMenu::OnRename);
    connect(m_ActModDelete, &QAction::triggered, this, &WorkshopMenu::OnDelete);
  }
}

void WorkshopMenu::OnMenuRequested(QPoint p_point)
{
  popup(QCursor::pos());
}

void WorkshopMenu::OnEditAction()
{
  WorkshopUploader::StartEdit(m_targetContent.id, m_targetContent);
}

void WorkshopMenu::OnOpenAction()
{
  QUrl folderUrl = QUrl::fromLocalFile(engine::fs::characters::getDirectoryPath(m_targetContent.folder));
  QDesktopServices::openUrl(folderUrl);
}

void WorkshopMenu::OnCopyAction()
{
  QClipboard *clipboard = QGuiApplication::clipboard(); clipboard->setText(m_targetContent.guid);
}

void WorkshopMenu::OnApprove()
{
  QJsonObject json{{"key", ApiManager::authorizationKey()}, {"silent", false}};
  QJsonDocument doc(json);

  ApiManager::instance().post("api/workshop/approve/" + QString::number(m_targetContent.id), doc.toJson());
}

void WorkshopMenu::OnApproveSilent()
{
  QJsonObject json{{"key", ApiManager::authorizationKey()}, {"silent", true}};
  QJsonDocument doc(json);

  ApiManager::instance().post("api/workshop/approve/" + QString::number(m_targetContent.id), doc.toJson());
}

void WorkshopMenu::OnUnapprove()
{
  QJsonObject json{{"key", ApiManager::authorizationKey()}};
  QJsonDocument doc(json);

  ApiManager::instance().post("api/workshop/unapprove/" + QString::number(m_targetContent.id), doc.toJson());
}

void WorkshopMenu::OnRename()
{
  RenameContentDialog dlg(m_targetContent.id, m_targetContent.name);
  dlg.exec();
}

void WorkshopMenu::OnDelete()
{
  QJsonObject json{{"key", ApiManager::authorizationKey()}};
  QJsonDocument doc(json);

  ApiManager::instance().post("api/workshop/delete/" + QString::number(m_targetContent.id), doc.toJson());
}
