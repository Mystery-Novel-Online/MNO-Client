#include "courtroom_layout.h"

#include <DRStickerViewer.h>
#include <aoapplication.h>

static QHash<QString, QWidget *> s_CourtroomWidgets = {};
static QVector<DRStickerViewer *> s_CourtroomStickers = {};
namespace Layout::Courtroom
{

  void SetWidgetList(QHash<QString, QWidget *> widgetList)
  {
    s_CourtroomWidgets = widgetList;
  }

  void MoveWidget(const char *name, int axisX, int axisY)
  {
    if(!s_CourtroomWidgets.contains(name)) return;
    QWidget *targetWidget = s_CourtroomWidgets[name];
    targetWidget->move(axisX, axisY);
  }

  void CreateSticker(const char *name, const char *image, int axisX, int axisY, int sizeX, int sizeY)
  {
    if(!s_CourtroomWidgets.contains("courtroom")) return;
    DRStickerViewer *targetSticker = nullptr;
    if(!s_CourtroomWidgets.contains(name))
    {
      targetSticker = new DRStickerViewer(AOApplication::getInstance(), s_CourtroomWidgets["courtroom"]);
      targetSticker->setObjectName(name);
      s_CourtroomStickers.append(targetSticker);
      s_CourtroomWidgets.insert(name, targetSticker);
      targetSticker->raise();
      targetSticker->show();
    }
    else
    {
      QWidget *sticketWidget = s_CourtroomWidgets[name];
      targetSticker = dynamic_cast<DRStickerViewer*>(sticketWidget);
      if(targetSticker == nullptr) return;

    }

    targetSticker->resize(sizeX, sizeY);
    targetSticker->move(axisX, axisY);
    targetSticker->set_theme_image(image);
    targetSticker->start();
  }

  void ToggleWidgetVisibility(const char *name, bool visibleState)
  {
    if(!s_CourtroomWidgets.contains(name)) return;
    s_CourtroomWidgets[name]->setVisible(visibleState);
  }

  void AddToWidgetList(QString name, QWidget *widget)
  {
    s_CourtroomWidgets[name] = widget;
  }

  void RaiseWidget(const char *name)
  {
    if(!s_CourtroomWidgets.contains(name)) return;
    s_CourtroomWidgets[name]->raise();
  }


}
