#include "courtroom_layout.h"

#include "dro/interface/widgets/sticker_viewer.h"
#include <aoapplication.h>
#include "dro/interface/widgets/chat_log.h"
#include <QString>
#include "modules/theme/thememanager.h"
#include "dro/system/theme_scripting.h"

static QHash<QString, QWidget *> s_CourtroomWidgets = {};
static QVector<DRStickerViewer *> s_CourtroomStickers = {};
static QVector<RPButton *> s_CourtroomButtons = {};

namespace courtroom
{

  void cleanup()
  {
    for (DRStickerViewer *sticker : s_CourtroomStickers)
    {
      auto it = s_CourtroomWidgets.begin();
      while (it != s_CourtroomWidgets.end())
      {
        if (it.value() == sticker) it = s_CourtroomWidgets.erase(it);
        else ++it;
      }
      delete sticker;
    }

    for (RPButton *button : s_CourtroomButtons)
    {
      auto it = s_CourtroomWidgets.begin();
      while (it != s_CourtroomWidgets.end())
      {
        if (it.value() == button) it = s_CourtroomWidgets.erase(it);
        else ++it;
      }
      delete button;
    }

    s_CourtroomButtons.clear();
    s_CourtroomStickers.clear();
  }

  namespace sliders
  {

    void setScale(int scaleValue)
    {
      QSlider *sliderWidget = nullptr;
      sliderWidget = dynamic_cast<QSlider*>(s_CourtroomWidgets["scale_offset"]);
      if(sliderWidget == nullptr) return;
      sliderWidget->setValue(scaleValue);
    }

    void setVertical(int verticalValue)
    {
      QSlider *sliderWidget = nullptr;
      sliderWidget = dynamic_cast<QSlider*>(s_CourtroomWidgets["vertical_offset"]);
      if(sliderWidget == nullptr) return;
      sliderWidget->setValue(verticalValue);
    }

    void setHorizontal(int horizontalValue)
    {
      QSlider *sliderWidget = nullptr;
      sliderWidget = dynamic_cast<QSlider*>(s_CourtroomWidgets["pair_offset"]);
      if(sliderWidget == nullptr) return;
      sliderWidget->setValue(horizontalValue);
    }

  }

  namespace buttons
  {
    void create(const std::string& name, int axisX, int axisY, int sizeX, int sizeY)
    {
      const QString qName = QString::fromStdString(name);

      if(!s_CourtroomWidgets.contains("courtroom")) return;
      QString widgetName = "button_" + QString(qName);

      RPButton *targetButton = nullptr;
      if(!s_CourtroomWidgets.contains(widgetName))
      {
        targetButton = new RPButton(s_CourtroomWidgets["courtroom"], AOApplication::getInstance());
        targetButton->setObjectName(widgetName);
        s_CourtroomButtons.append(targetButton);
        s_CourtroomWidgets.insert(widgetName, targetButton);
        targetButton->raise();
        targetButton->show();

        QObject::connect(targetButton, &QPushButton::clicked, [=]()
        {
          QString eventName = qName + "ButtonEvent";
          LuaBridge::LuaEventCall(eventName.toUtf8());
        });
      }
      else
      {
        QWidget *buttonWidget = s_CourtroomWidgets[widgetName];
        targetButton = dynamic_cast<RPButton*>(buttonWidget);
        if(targetButton == nullptr) return;
      }
      float resizeFactor = ThemeManager::get().getResize();

      int l_scaledWidth = static_cast<int>(sizeX * resizeFactor);
      int l_scaledHeight = static_cast<int>(sizeY * resizeFactor);
      int l_scaledX = static_cast<int>(axisX * resizeFactor);
      int l_scaledY = static_cast<int>(axisY * resizeFactor);

      targetButton->resize(l_scaledWidth, l_scaledHeight);
      targetButton->move(l_scaledX, l_scaledY);
      targetButton->set_theme_image(widgetName, widgetName + ".png", "courtroom", qName);

    }
  }

  namespace stickers
  {
    void create(const std::string& name, const std::string& image, int axisX, int axisY, int sizeX, int sizeY)
    {
      const QString qName = QString::fromStdString(name);
      const QString qImage = QString::fromStdString(image);

      if(!s_CourtroomWidgets.contains("courtroom"))
        return;

      DRStickerViewer *targetSticker = nullptr;

      if(!s_CourtroomWidgets.contains(qName))
      {
        targetSticker = new DRStickerViewer(AOApplication::getInstance(), s_CourtroomWidgets["courtroom"]);
        targetSticker->setObjectName(qName);
        s_CourtroomStickers.append(targetSticker);
        s_CourtroomWidgets.insert(qName, targetSticker);
        targetSticker->raise();
        targetSticker->show();
      }
      else
      {
        QWidget *sticketWidget = s_CourtroomWidgets[qName];
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
      targetSticker->set_theme_image(qImage);
      targetSticker->start();
    }
  }

  namespace ooc
  {
    void appendMessage(const char *name, const char *message)
    {
      DRChatLog *oocChatlog = nullptr;
      QWidget *chatlogWidget = s_CourtroomWidgets["server_chatlog"];
      oocChatlog = dynamic_cast<DRChatLog*>(chatlogWidget);
      if(oocChatlog == nullptr) return;
      oocChatlog->append_chatmessage(name, message);
    }
  }

  namespace layout
  {
    void setWidgetList(QHash<QString, QWidget *> widgetList)
    {
      s_CourtroomWidgets = widgetList;
    }

    void moveWidget(const char *name, int axisX, int axisY)
    {
      if(!s_CourtroomWidgets.contains(name)) return;
      QWidget *targetWidget = s_CourtroomWidgets[name];
      float resizeFactor = ThemeManager::get().getResize();
      int scaledX = static_cast<int>(axisX * resizeFactor);
      int scaledY = static_cast<int>(axisY * resizeFactor);
      targetWidget->move(scaledX, scaledY);
    }

    void setVisibility(const char *name, bool visibleState)
    {
      if(!s_CourtroomWidgets.contains(name)) return;
      QWidget *widgetTarget = s_CourtroomWidgets[name];
      widgetTarget->setVisible(visibleState);
    }

    void addWidget(QString& name, QWidget *widget)
    {
      s_CourtroomWidgets[name] = widget;
    }

    void raiseWidget(const char *name)
    {
      if(!s_CourtroomWidgets.contains(name)) return;
      s_CourtroomWidgets[name]->raise();
    }

    void setParent(const std::string &parent, const std::string &child)
    {
      const QString qParent = QString::fromStdString(parent);
      const QString qChild = QString::fromStdString(child);

      if(!s_CourtroomWidgets.contains(qParent)) return;
      if(!s_CourtroomWidgets.contains(qChild)) return;
      s_CourtroomWidgets[qChild]->setParent(s_CourtroomWidgets[qParent]);
    }

  }

}


