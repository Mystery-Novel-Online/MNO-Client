#include <engine/network/workshop/workshop_models.h>
#ifndef WORKSHOPCONTEXTMENU_H
#define WORKSHOPCONTEXTMENU_H

class WorkshopMenu : public QMenu
{
  Q_OBJECT
public:
  WorkshopMenu();
  void setContent(const WorkshopContentEntry& content) { m_targetContent = content;};

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnEditAction();
  void OnOpenAction();
  void OnCopyAction();

  void OnApprove();
  void OnApproveSilent();
  void OnUnapprove();
  void OnRename();
  void OnDelete();

private:
  WorkshopContentEntry m_targetContent;

  QAction *m_ActOpenContent;
  QAction *m_ActCopyGuid;
  QAction *m_ActEdit;

  //Mod Actions
  QMenu *m_MenuMod;
  QAction *m_ActModRename;
  QAction *m_ActModApprove;
  QAction *m_ActModUnapprove;
  QAction *m_ActModSilentApproval;
  QAction *m_ActModDelete;
};

#endif // WORKSHOPCONTEXTMENU_H
