#include "courtroom_layout.h"

#include <DRStickerViewer.h>
#include <aoapplication.h>
#include <drchatlog.h>
#include <QString>
#include "modules/theme/thememanager.h"

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
    float resizeFactor = ThemeManager::get().getResize();
    int scaledX = static_cast<int>(axisX * resizeFactor);
    int scaledY = static_cast<int>(axisY * resizeFactor);
    targetWidget->move(scaledX, scaledY);
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
    float resizeFactor = ThemeManager::get().getResize();

    int l_scaledWidth = static_cast<int>(sizeX * resizeFactor);
    int l_scaledHeight = static_cast<int>(sizeY * resizeFactor);
    int l_scaledX = static_cast<int>(axisX * resizeFactor);
    int l_scaledY = static_cast<int>(axisY * resizeFactor);

    targetSticker->resize(l_scaledWidth, l_scaledHeight);
    targetSticker->move(l_scaledX, l_scaledY);
    targetSticker->set_theme_image(image);
    targetSticker->start();
  }

  void ToggleWidgetVisibility(const char *name, bool visibleState)
  {
    if(!s_CourtroomWidgets.contains(name)) return;
    QWidget *widgetTarget = s_CourtroomWidgets[name];
    widgetTarget->setVisible(visibleState);
  }

  void AddToWidgetList(QString name, QWidget *widget)
  {
    s_CourtroomWidgets[name] = widget;
  }

  void RaiseWidget(const char *name)
  {
    if(!s_CourtroomWidgets.contains(name))
    {
      return;
    }
    s_CourtroomWidgets[name]->raise();
  }

  void AppendToOOC(const char* name, const char* message)
  {
    DRChatLog *oocChatlog = nullptr;
    QWidget *chatlogWidget = s_CourtroomWidgets["server_chatlog"];
    oocChatlog = dynamic_cast<DRChatLog*>(chatlogWidget);
    if(oocChatlog == nullptr) return;
    oocChatlog->append_chatmessage(name, message);
  }

  void SetScaleSlider(int scaleValue)
  {
    QSlider *sliderWidget = nullptr;
    sliderWidget = dynamic_cast<QSlider*>(s_CourtroomWidgets["scale_offset"]);
    if(sliderWidget == nullptr) return;
    sliderWidget->setValue(scaleValue);
  }

  void SetVerticalSlider(int verticalValue)
  {
    QSlider *sliderWidget = nullptr;
    sliderWidget = dynamic_cast<QSlider*>(s_CourtroomWidgets["vertical_offset"]);
    if(sliderWidget == nullptr) return;
    sliderWidget->setValue(verticalValue);
  }


}
