#include "courtroom_layout.h"

#include "modules/theme/thememanager.h"
#include "engine/system/theme_scripting.h"

#include <engine/animation/widget_animator.h>

static QHash<QString, QWidget *> s_CourtroomWidgets = {};
static QHash<QString, QWidget *> s_TabWidgets = {};
static QHash<QString, WidgetAnimator *> s_WidgetAnimators = {};

static QVector<DRStickerViewer *> s_CourtroomStickers = {};
static QVector<RPButton *> s_CourtroomButtons = {};
static QVector<RPSlider *> s_CourtroomSliders = {};

static QVector<QLineEdit *> s_CourtroomLineEdits = {};
static QVector<QTextEdit *> s_CourtroomTextEdits = {};
static QVector<QComboBox *> s_CourtroomComboBoxes = {};
static QVector<RPHoverWidget *> s_CourtroomHoverWidgets = {};

template <typename T>
void cleanupWidgets(QVector<T*> &list)
{
  QWidget* courtroomWidget = s_CourtroomWidgets.value("courtroom", nullptr);

  for (T *item : list)
  {
    auto it = s_CourtroomWidgets.begin();
    while (it != s_CourtroomWidgets.end())
    {
      if (it.value() == item)
        it = s_CourtroomWidgets.erase(it);
      else
        ++it;
    }

    if (courtroomWidget && item)
    {
      const auto& children = item->template findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
      for (QWidget* child : children)
      {
        child->setParent(courtroomWidget);
      }
    }

    delete item;
  }
  list.clear();
}


namespace courtroom
{

  void cleanup()
  {
    s_CourtroomWidgets.remove("viewport");
    cleanupWidgets(s_CourtroomStickers);
    cleanupWidgets(s_CourtroomButtons);
    cleanupWidgets(s_CourtroomSliders);
    cleanupWidgets(s_CourtroomLineEdits);
    cleanupWidgets(s_CourtroomTextEdits);
    cleanupWidgets(s_CourtroomComboBoxes);
    cleanupWidgets(s_CourtroomHoverWidgets);
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
      setValue("scale_offset", scaleValue);
    }

    void setVertical(int verticalValue)
    {
      setValue("vertical_offset", verticalValue);
    }

    void setHorizontal(int horizontalValue)
    {
      setValue("pair_offset", horizontalValue);
    }

    void create(const std::string &name, int x, int y, int width, int height, int min, int max)
    {
      if(!s_CourtroomWidgets.contains("courtroom")) return;
      const QString qName = QString::fromStdString(name);
      QString widgetName = "slider_" + QString(qName);

      RPSlider *slider = qobject_cast<RPSlider *>(s_CourtroomWidgets.value(widgetName));
      if(!slider)
      {
        slider = new RPSlider(Qt::Orientation::Horizontal, s_CourtroomWidgets["courtroom"]);
        s_CourtroomSliders.append(slider);
        s_CourtroomWidgets.insert(widgetName, slider);
        slider->raise();
        slider->show();

        QObject::connect(slider, &QAbstractSlider::valueChanged, [=](int value)
        {
          QString eventName = qName + "ValueChanged";
          LuaBridge::LuaEventCall(eventName.toUtf8(), value);
        });
      }

      float resizeFactor = ThemeManager::get().getResize();

      int l_scaledWidth = static_cast<int>(width * resizeFactor);
      int l_scaledHeight = static_cast<int>(height * resizeFactor);
      int l_scaledX = static_cast<int>(x * resizeFactor);
      int l_scaledY = static_cast<int>(y * resizeFactor);

      slider->resize(l_scaledWidth, l_scaledHeight);
      slider->move(l_scaledX, l_scaledY);
      slider->setMinimum(min);
      slider->setMaximum(max);

    }

    int getValue(const std::string &name)
    {
      if (auto *slider = qobject_cast<QSlider *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        return slider->value();
      return 0;
    }

    void setValue(const std::string &name, int value)
    {
      if (auto *slider = qobject_cast<QSlider *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        slider->setValue(value);
    }

    void createVertical(const std::string &name, int x, int y, int width, int height, int min, int max)
    {
      if(!s_CourtroomWidgets.contains("courtroom")) return;
      const QString qName = QString::fromStdString(name);
      QString widgetName = "slider_" + QString(qName);

      RPSlider *slider = qobject_cast<RPSlider *>(s_CourtroomWidgets.value(widgetName));
      if(!slider)
      {
        slider = new RPSlider(Qt::Orientation::Vertical, s_CourtroomWidgets["courtroom"]);
        s_CourtroomSliders.append(slider);
        s_CourtroomWidgets.insert(widgetName, slider);
        slider->raise();
        slider->show();

        QObject::connect(slider, &QAbstractSlider::valueChanged, [=](int value)
                         {
                           QString eventName = qName + "ValueChanged";
                           LuaBridge::LuaEventCall(eventName.toUtf8(), value);
                         });
      }

      float resizeFactor = ThemeManager::get().getResize();

      int l_scaledWidth = static_cast<int>(width * resizeFactor);
      int l_scaledHeight = static_cast<int>(height * resizeFactor);
      int l_scaledX = static_cast<int>(x * resizeFactor);
      int l_scaledY = static_cast<int>(y * resizeFactor);

      slider->resize(l_scaledWidth, l_scaledHeight);
      slider->move(l_scaledX, l_scaledY);
      slider->setMinimum(min);
      slider->setMaximum(max);
    }

  }

  namespace lists
  {
    void setAnimations(const QStringList &animations)
    {
      if (auto *list = qobject_cast<QListWidget *>(s_CourtroomWidgets.value("chara_animations")))
      {
        list->clear();
        list->addItems(animations);
      }
    }

    std::string getAnimation()
    {
      if (auto *list = qobject_cast<QListWidget *>(s_CourtroomWidgets.value("chara_animations")))
      {
        if(list->currentItem() != nullptr)
        {
          return list->currentItem()->text().toStdString();
        }
      }
      return "";
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


  namespace lineedit
  {
    void create(const std::string& name, const std::string& css, int x, int y, int width, int height)
    {
      const QString qName = QString::fromStdString(name);
      if (!s_CourtroomWidgets.contains("courtroom")) return;

      if (!s_CourtroomWidgets.contains(qName))
      {
        RPLineEdit* lineEdit = new RPLineEdit(QString::fromStdString(name), "[" + QString::fromStdString(css) + "]", s_CourtroomWidgets["courtroom"]);
        lineEdit->refreshCSS();
        s_CourtroomLineEdits.append(lineEdit);
        s_CourtroomWidgets.insert(qName, lineEdit);
        lineEdit->raise();
        lineEdit->show();

        QObject::connect(lineEdit, &QLineEdit::textChanged, [=](const QString &text) {
                           QString eventName = QString::fromStdString(name) + "TextChanged";
                           LuaBridge::LuaEventCall(eventName.toUtf8(), text.toStdString());
                         });

        QObject::connect(lineEdit, &QLineEdit::returnPressed, [=]() {
                           QString eventName = QString::fromStdString(name) + "ReturnPressed";
                           LuaBridge::LuaEventCall(eventName.toUtf8());
                         });
        QObject::connect(lineEdit, &QLineEdit::textEdited, [=](const QString &text) { QString eventName = QString::fromStdString(name) + "TextEdited"; LuaBridge::LuaEventCall(eventName.toUtf8(), text.toStdString()); });
        QObject::connect(lineEdit, &QLineEdit::editingFinished, [=]() { QString eventName = QString::fromStdString(name) + "EditingFinished"; LuaBridge::LuaEventCall(eventName.toUtf8()); });
        QObject::connect(lineEdit, &QLineEdit::selectionChanged, [=]() { QString eventName = QString::fromStdString(name) + "SelectionChanged"; LuaBridge::LuaEventCall(eventName.toUtf8()); });

      }

      float resizeFactor = ThemeManager::get().getResize();
      s_CourtroomWidgets[qName]->resize(width * resizeFactor, height * resizeFactor);
      s_CourtroomWidgets[qName]->move(x * resizeFactor, y * resizeFactor);
    }

    std::string getValue(const std::string &name)
    {
      if (auto *box = qobject_cast<RPLineEdit *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        return box->text().toStdString();
      return "";
    }

    void setValue(const std::string &name, const std::string &value)
    {
      if (auto *box = qobject_cast<RPLineEdit *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        box->setText(QString::fromStdString(value));
    }

  }

  namespace textedit
  {
    void create(const std::string& name, int x, int y, int width, int height)
    {
      const QString qName = QString::fromStdString(name);
      if (!s_CourtroomWidgets.contains("courtroom")) return;

      if (!s_CourtroomWidgets.contains(qName))
      {
        RPTextEdit* textEdit = new RPTextEdit(QString::fromStdString(name), s_CourtroomWidgets["courtroom"]);
        s_CourtroomTextEdits.append(textEdit);
        s_CourtroomWidgets.insert(qName, textEdit);

        set_drtextedit_font(textEdit, QString::fromStdString(name), COURTROOM_FONTS_INI, AOApplication::getInstance());
        textEdit->setPlainText(textEdit->toPlainText());
        textEdit->setReadOnly(true);

        textEdit->raise();
        textEdit->show();
      }

      float resizeFactor = ThemeManager::get().getResize();
      s_CourtroomWidgets[qName]->resize(width * resizeFactor, height * resizeFactor);
      s_CourtroomWidgets[qName]->move(x * resizeFactor, y * resizeFactor);
    }

    void setText(const std::string &name, const std::string &text)
    {
      if (auto *box = qobject_cast<RPTextEdit *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        box->setText(QString::fromStdString(text));
    }

    void setFrame(const std::string &name, const bool state)
    {
      if (auto *box = qobject_cast<RPTextEdit *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        box->setFrameStyle(state ? QFrame::Box : QFrame::NoFrame);
    }

  }

  namespace combobox
  {
  void create(const std::string& name, const std::string &css, int x, int y, int width, int height)
    {
      const QString qName = QString::fromStdString(name);
      if (!s_CourtroomWidgets.contains("courtroom")) return;

      if (!s_CourtroomWidgets.contains(qName))
      {

        RPComboBox* comboBox = new RPComboBox(s_CourtroomWidgets["courtroom"], AOApplication::getInstance());
        comboBox->setWidgetInfo(QString::fromStdString(name), "[" + QString::fromStdString(css) + "]", "courtroom");
        comboBox->refreshCSS();
        s_CourtroomComboBoxes.append(comboBox);
        s_CourtroomWidgets.insert(qName, comboBox);
        comboBox->raise();
        comboBox->show();

        QObject::connect(comboBox, &QComboBox::currentTextChanged, [=](const QString &text) {
                           QString eventName = QString::fromStdString(name) + "SelectionChanged";
                           LuaBridge::LuaEventCall(eventName.toUtf8(), text);
                         });

        QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                           QString eventName = QString::fromStdString(name) + "IndexChanged";
                           LuaBridge::LuaEventCall(eventName.toUtf8(), index);
                         });
      }

      float resizeFactor = ThemeManager::get().getResize();
      s_CourtroomWidgets[qName]->resize(width * resizeFactor, height * resizeFactor);
      s_CourtroomWidgets[qName]->move(x * resizeFactor, y * resizeFactor);
    }

    void addItem(const std::string &name, const std::string &value)
    {
      if (auto *combobox = qobject_cast<RPComboBox *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        combobox->addItem(QString::fromStdString(value));
    }

    void removeItem(const std::string &name, const std::string &value)
    {
      //if (auto *combobox = qobject_cast<RPComboBox *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
      //  combobox->addItem(QString::fromStdString(value));
    }

    void clearItems(const std::string &name)
    {
      if (auto *combobox = qobject_cast<RPComboBox *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
        combobox->clear();
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

    void cleanupToggles()
    {
      for(QString toggle : s_TabWidgets.keys())
      {
        delete s_TabWidgets[toggle];
        s_TabWidgets.remove(toggle);
      }
    }

  }

  namespace ic
  {

    void focusMessageBox()
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      RPMessageInput* lineEditField = dynamic_cast<RPMessageInput*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setFocus();
    }

    void setMessageBox(const std::string& text)
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      RPMessageInput* lineEditField = dynamic_cast<RPMessageInput*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setText(QString::fromStdString(text));
    }

    std::string getMessageBoxContents()
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return "";
      RPMessageInput* lineEditField = dynamic_cast<RPMessageInput*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField == nullptr) return "";
      return lineEditField->text().toStdString();
    }

    void appendMessageBox(const std::string &text)
    {
      if(!s_CourtroomWidgets.contains("ao2_ic_chat_message_field")) return;
      RPMessageInput* lineEditField = dynamic_cast<RPMessageInput*>(s_CourtroomWidgets["ao2_ic_chat_message_field"]);
      if(lineEditField != nullptr) lineEditField->setText(lineEditField->text() + QString::fromStdString(text));
    }

  }

  namespace ooc
  {
    void appendMessage(const std::string& name, const std::string& message)
    {
      DRChatLog *oocChatlog = nullptr;
      QWidget *chatlogWidget = s_CourtroomWidgets["server_chatlog"];
      oocChatlog = dynamic_cast<DRChatLog*>(chatlogWidget);
      if(oocChatlog == nullptr) return;
      oocChatlog->append_chatmessage(QString::fromStdString(name), QString::fromStdString(message));
    }

    std::string getInputFieldContents()
    {
      if (auto *box = qobject_cast<QLineEdit *>(s_CourtroomWidgets.value("ooc_chat_message")))
        return box->text().toStdString();
      return "";
    }

    void setInputFieldContents(const std::string &text)
    {
      if (auto *box = qobject_cast<QLineEdit *>(s_CourtroomWidgets.value("ooc_chat_message")))
        box->setText(QString::fromStdString(text));
    }

    std::string getDisplayName()
    {
      if (auto *box = qobject_cast<QLineEdit *>(s_CourtroomWidgets.value("ooc_chat_name")))
        return box->text().toStdString();
      return "";
    }

    void setDisplayName(const std::string &text)
    {
      if (auto *box = qobject_cast<QLineEdit *>(s_CourtroomWidgets.value("ooc_chat_name")))
        box->setText(QString::fromStdString(text));
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

    void resizeWidget(const char *name, int width, int height)
    {
      if(!s_CourtroomWidgets.contains(name)) return;
      QWidget *targetWidget = s_CourtroomWidgets[name];
      float resizeFactor = ThemeManager::get().getResize();
      int scaledX = static_cast<int>(width * resizeFactor);
      int scaledY = static_cast<int>(height * resizeFactor);
      targetWidget->resize(scaledX, scaledY);
    }

  }

  namespace viewport
  {
    void update()
    {
      DRGraphicsView *viewport = dynamic_cast<DRGraphicsView*>(s_CourtroomWidgets["viewport"]);
      if(viewport != nullptr)
      {
        viewport->scene()->update();
      }
    }

    QPixmap getScreenshot()
    {
      Courtroom *courtroom = dynamic_cast<Courtroom*>(s_CourtroomWidgets["courtroom"]);
      DRGraphicsView *viewport = dynamic_cast<DRGraphicsView*>(s_CourtroomWidgets["viewport"]);

      if(courtroom != nullptr && viewport != nullptr)
      {
        QPixmap courtroomRender = courtroom->grab();

        QRect viewportArea(viewport->x(), viewport->y(), viewport->width(), viewport->height());
        QPixmap viewportCrop = courtroomRender.copy(viewportArea);
        return viewportCrop;
      }

      return QPixmap(0, 0);
    }

    void clearInteractions()
    {
      if (auto *overlay = qobject_cast<ViewportOverlay *>(s_CourtroomWidgets.value("viewport_overlay")))
        overlay->clearInteractions();
    }

    void addInteraction(const std::string &name, const std::string &description, int x, int y, int width, int height)
    {
      if (auto *overlay = qobject_cast<ViewportOverlay *>(s_CourtroomWidgets.value("viewport_overlay")))
        overlay->addInteraction({x, y, width, height}, QString::fromStdString(name), QString::fromStdString(description));
    }

    void screenshot(CaptureType captureMode)
    {
      Courtroom *courtroom = dynamic_cast<Courtroom*>(s_CourtroomWidgets["courtroom"]);

      QString targetWidget = "viewport";
      switch(captureMode)
      {
      case Capture_Window:
        targetWidget = "courtroom";
        break;

      case Capture_ICLog:
        targetWidget = "ic_chatlog";
        break;

      default:
        break;
      }

      QWidget *viewport = dynamic_cast<QWidget*>(s_CourtroomWidgets[targetWidget]);

      if(courtroom != nullptr && viewport != nullptr)
      {
        QPixmap courtroomRender = courtroom->grab();

        QPoint topLeft = viewport->mapTo(courtroom, QPoint(0, 0));

        QRect viewportArea(topLeft, viewport->size());

        QPixmap viewportCrop = courtroomRender.copy(viewportArea);

        QString outputFilename = QDateTime::currentDateTime().toString("yyyy-MM-dd (hh.mm.ss.z)'.png'");
        QString outputPath = "base/screenshots/" + outputFilename;

        if (!viewportCrop.save(outputPath, "PNG"))
          qWarning("Failed to save the screenshot.");
      }
    }

  }

  void hovercontroller::create(const std::string &name, int x, int y, int width, int height)
  {
    const QString qName = QString::fromStdString(name);
    if (!s_CourtroomWidgets.contains("courtroom")) return;

    if (!s_CourtroomWidgets.contains(qName))
    {
      RPHoverWidget* hoverWidget = new RPHoverWidget(s_CourtroomWidgets["courtroom"]);
      s_CourtroomHoverWidgets.append(hoverWidget);
      s_CourtroomWidgets.insert(qName, hoverWidget);

      hoverWidget->raise();
      hoverWidget->show();
    }

    float resizeFactor = ThemeManager::get().getResize();
    s_CourtroomWidgets[qName]->resize(width * resizeFactor, height * resizeFactor);
    s_CourtroomWidgets[qName]->move(x * resizeFactor, y * resizeFactor);
  }

  void hovercontroller::addWidget(const std::string &name, const std::string &child)
  {
    if (auto *hoverWidget = qobject_cast<RPHoverWidget *>(s_CourtroomWidgets.value(QString::fromStdString(name))))
    {
      if (auto *childWidget = qobject_cast<QWidget *>(s_CourtroomWidgets.value(QString::fromStdString(child))))
      {
        hoverWidget->addWidget(childWidget);
      }
    }
  }

  void areas::switchName(const std::string &name)
  {
    if (auto *areaList = qobject_cast<RPListWidget *>(s_CourtroomWidgets.value("area_list")))
    {
      for(int i = 0; i < areaList->count(); i++)
      {
        const QString l_area_name = areaList->item(i)->text();
        if(l_area_name.contains(QString::fromStdString(name)))
        {
          AOApplication::getInstance()->send_server_packet(DRPacket("MC", {l_area_name, QString::number(user::GetCharacterId())}));
          return;
        }
      }

    }
  }

  void animations::createAnimation(const std::string &name, const std::string &target, bool loop)
  {
    QString qTarget = QString::fromStdString(target);
    QString qName = QString::fromStdString(name);
    if (auto *targetWidget = qobject_cast<QWidget *>(s_CourtroomWidgets.value(qTarget)))
    {
      if(s_WidgetAnimators.contains(qName)) delete s_WidgetAnimators[qName];
      s_WidgetAnimators[qName] = new WidgetAnimator(targetWidget);
      s_WidgetAnimators[qName]->setFriendlyName(qName);
      s_WidgetAnimators[qName]->setRunningState(false);
      s_WidgetAnimators[qName]->SetLoop(loop);
    }
  }

  void animations::addKeyframe(const std::string &name, float timeMs, float x, float y, float z)
  {
    QString qAnimName = QString::fromStdString(name);

    if (!s_WidgetAnimators.contains(qAnimName)) {
      qWarning() << "Animation" << qAnimName << "not found!";
      return;
    }

    auto *animator = s_WidgetAnimators[qAnimName];
    auto channel = animator->GetChannel<QVector3D>("position");

    if (!channel)
    {
      auto newChannel = std::make_unique<KeyframeChannel<QVector3D>>();
      newChannel->AddKeyframe(timeMs, QVector3D(x, y, z),
                              static_cast<KeyframeCurve>(CurveLinear),
                              static_cast<KeyframeCurve>(CurveLinear));
      animator->AddChannel("position", std::move(newChannel));
    }
    else
    {
      channel->AddKeyframe(timeMs, QVector3D(x, y, z),
                           static_cast<KeyframeCurve>(CurveLinear),
                           static_cast<KeyframeCurve>(CurveLinear));
      animator->CalculateLength("position");
    }
  }

  void animations::reset(const std::string &name)
  {
    QString qAnimName = QString::fromStdString(name);
    if (!s_WidgetAnimators.contains(qAnimName)) {
      qWarning() << "Animation" << qAnimName << "not found!";
      return;
    }

    s_WidgetAnimators[qAnimName]->Cleanup();
  }

  void animations::playAnimation(const std::string &name)
  {
    QString qAnimName = QString::fromStdString(name);
    if (!s_WidgetAnimators.contains(qAnimName)) {
      qWarning() << "Animation" << qAnimName << "not found!";
      return;
    }

    s_WidgetAnimators[qAnimName]->setRunningState(true);
  }

  void animations::stopAnimation(const std::string &name)
  {
    QString qAnimName = QString::fromStdString(name);
    if (!s_WidgetAnimators.contains(qAnimName)) {
      qWarning() << "Animation" << qAnimName << "not found!";
      return;
    }

    s_WidgetAnimators[qAnimName]->setRunningState(false);
  }


}


