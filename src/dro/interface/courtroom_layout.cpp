#include "courtroom_layout.h"

#include "dro/interface/widgets/sticker_viewer.h"
#include <aoapplication.h>
#include "dro/interface/widgets/chat_log.h"
#include <QString>
#include <courtroom.h>
#include "modules/theme/thememanager.h"
#include "dro/system/theme_scripting.h"

static QHash<QString, QWidget *> s_CourtroomWidgets = {};
static QVector<DRStickerViewer *> s_CourtroomStickers = {};
static QVector<RPButton *> s_CourtroomButtons = {};
static QHash<QString, QWidget *> s_TabWidgets = {};

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

  void reload()
  {
    auto it = s_CourtroomWidgets.begin();
    while (it != s_CourtroomWidgets.end())
    {
      RPWidget *widget = dynamic_cast<RPWidget*>(it.value());
      RPLineEdit *lineEdit = dynamic_cast<RPLineEdit*>(it.value());
      RPButton* rpButton = dynamic_cast<RPButton*>(it.value());
      RPComboBox* comboBox = dynamic_cast<RPComboBox*>(it.value());

      if(widget != nullptr)
      {
        widget->resetTransform();
      }
      if(lineEdit != nullptr)
      {
        lineEdit->refreshPosition();
        lineEdit->refreshCSS();
        lineEdit->show();
      }

      if(rpButton != nullptr)
      {
        rpButton->set_theme_image();
        rpButton->refresh_position();
        rpButton->show();
      }

      if(comboBox != nullptr)
      {
        comboBox->refreshPosition();
        comboBox->refreshCSS();
        comboBox->show();
      }
      ++it;
    }
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
        targetButton = new RPButton(s_CourtroomWidgets["courtroom"]);
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

  namespace tabs
  {

    void deleteToggle(const QString &name)
    {
      QString toggleName = name + "_toggle";
      if(s_TabWidgets.contains(toggleName))
      {
        delete s_TabWidgets[toggleName];
        s_TabWidgets.remove(toggleName);
      }
    }

  }

  namespace ic
  {

    void focusMessageBox()
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      QLineEdit* lineEditField = dynamic_cast<QLineEdit*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setFocus();
    }

    void setMessageBox(const std::string& text)
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      QLineEdit* lineEditField = dynamic_cast<QLineEdit*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setText(QString::fromStdString(text));
    }

    std::string getMessageBoxContents()
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return "";
      QLineEdit* lineEditField = dynamic_cast<QLineEdit*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField == nullptr) return "";
      return lineEditField->text().toStdString();
    }

    void appendMessageBox(const std::string &text)
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      QLineEdit* lineEditField = dynamic_cast<QLineEdit*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setText(lineEditField->text() + QString::fromStdString(text));
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
      if(name.endsWith("_toggle")) s_TabWidgets[name] = widget;
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

  namespace viewport
  {
    void screenshot()
    {
      Courtroom *courtroom = dynamic_cast<Courtroom*>(s_CourtroomWidgets["courtroom"]);
      DRGraphicsView *viewport = dynamic_cast<DRGraphicsView*>(s_CourtroomWidgets["viewport"]);

      if(courtroom != nullptr && viewport != nullptr)
      {
        QPixmap courtroomRender = courtroom->grab();

        QRect viewportArea(viewport->x(), viewport->y(), viewport->width(), viewport->height());
        QPixmap viewportCrop = courtroomRender.copy(viewportArea);

        QString outputFilename = QDateTime::currentDateTime().toString("yyyy-MM-dd (hh.mm.ss.z)'.png'");
        QString outputPath = "base/screenshots/" + outputFilename;

        if (!viewportCrop.save(outputPath, "PNG"))
          qWarning("Failed to save the screenshot.");
      }
    }
  }
}


