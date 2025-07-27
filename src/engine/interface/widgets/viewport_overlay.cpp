#include "viewport_overlay.h"

#include <modules/theme/thememanager.h>
#include <modules/managers/scene_manager.h>
#include "engine/interface/courtroom_layout.h"
#include "engine/system/theme_scripting.h"
#include "engine/system/audio.h"

ViewportOverlay::ViewportOverlay(QWidget *parent)
    : QWidget{parent}
{
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &ViewportOverlay::displayContextMenu);

  setMouseTracking(true);
  m_CursorOverlay = new AOImageDisplay(this, AOApplication::getInstance());
  m_CursorOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_CursorOverlay->resize(61, 61);
  m_CursorOverlay->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_idle.png"));
  m_CursorOverlay->hide();

  m_NameDisplay = new AOImageDisplay(this, AOApplication::getInstance());
  m_NameDisplay->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_NameDisplay->resize(300, 40);
  m_NameDisplay->move(330, 10);
  m_NameDisplay->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_name_display.png"));
  m_NameDisplay->hide();


  m_NameText = new RPTextEdit("cursor_text", m_NameDisplay);
  m_NameText->move(4, 7);
  m_NameText->resize(292, 31);
  m_NameText->setFrameStyle(QFrame::NoFrame);
  m_NameText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_NameText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_NameText->setReadOnly(true);
  set_drtextedit_font(m_NameText, "cursor_text", COURTROOM_FONTS_INI, AOApplication::getInstance());
  m_NameText->setText("Object");
}

void ViewportOverlay::clearInteractions()
{
  for(InteractionObject *interaction : m_Interactions)
    delete interaction;

  m_Interactions.clear();
  m_CursorOverlay->hide();
  setCursor(Qt::ArrowCursor);
}

void ViewportOverlay::addInteraction(QRect area, const QString &name, const QString &description)
{
  InteractionObject *interaction = new InteractionObject(this);
  interaction->move(area.x(), area.y());
  interaction->resize(area.width(), area.height());
  interaction->setName(name);
  interaction->setDescription(description);
  interaction->setTextWidget(m_NameText);
  interaction->setToggleObject(m_NameDisplay);
  interaction->setCursorWidget(m_CursorOverlay);
  interaction->show();
  interaction->raise();
  m_Interactions.append(interaction);
}

void ViewportOverlay::moveCursor(int x, int y)
{
  m_CursorOverlay->move(x - 30, y - 30);
}

QAction *ViewportOverlay::createResizeAction(int t_width, int t_height)
{
  QAction *l_resizeAction = new QAction(QString::number(t_width) + " x " + QString::number(t_height));
  QObject::connect(l_resizeAction, &QAction::triggered, [this, t_width, t_height](){resizeViewport(t_width, t_height);});
  return l_resizeAction;
}

void ViewportOverlay::resizeWidgetAndChildren(QString t_widget, int t_width, int t_height)
{
  
  QWidget* l_widget = ThemeManager::get().getWidget(t_widget);
  if(l_widget == nullptr) return;

  int l_viewportNativeHeight = ThemeManager::get().mCurrentThemeReader.GetWidgetTransform(ThemeSceneType::SceneType_Courtroom, "viewport").height;
  RPRect l_widgetSize = ThemeManager::get().mCurrentThemeReader.GetWidgetTransform(ThemeSceneType::SceneType_Courtroom, t_widget);

  double l_scalingFactor = static_cast<double>(t_height) / l_viewportNativeHeight;

  int l_scaledWidth = static_cast<int>(l_widgetSize.width * l_scalingFactor);
  int l_scaledHeight = static_cast<int>(l_widgetSize.height * l_scalingFactor);
  int l_scaledX = static_cast<int>(l_widgetSize.x * l_scalingFactor);
  int l_scaledY = static_cast<int>(l_widgetSize.y * l_scalingFactor);

  ThemeManager::get().setViewporResize(l_scalingFactor);

  if(t_widget.startsWith("viewport"))
  {
    l_scaledX = 0;
    l_scaledY = 0;
    l_scaledWidth = t_width;
    l_scaledHeight = t_height;
  }

  l_widget->resize(l_scaledWidth, l_scaledHeight);
  l_widget->move(l_scaledX, l_scaledY);

  if (dynamic_cast<AOImageDisplay*>(l_widget) != nullptr)
  {
    AOImageDisplay* l_imageDisplay = dynamic_cast<AOImageDisplay*>(l_widget);
    l_imageDisplay->refreshImage();
  }

  QStringList l_viewportChildren = ThemeManager::get().mCurrentThemeReader.GetLayerChildren(t_widget);
  for(QString l_child : l_viewportChildren)
  {
    resizeWidgetAndChildren(l_child, t_width, t_height);
  }

}

void ViewportOverlay::mouseMoveEvent(QMouseEvent *event)
{
  moveCursor(event->x(), event->y());
}

void ViewportOverlay::leaveEvent(QEvent *event)
{
  m_CursorOverlay->hide();
  setCursor(Qt::ArrowCursor);
}

void ViewportOverlay::enterEvent(QEvent *event)
{
  if(m_Interactions.isEmpty()) return;
  m_CursorOverlay->show();
  setCursor(Qt::BlankCursor);
}

void ViewportOverlay::displayContextMenu(QPoint t_position)
{
  //if(AOApplication::getInstance()->current_theme->read_config_bool("detatchable_viewport"))
  //{
  //  QMenu *l_menu = new QMenu(this);

  //  //Create the detachWindowAction
  //  QAction *l_detachWindowAction = new QAction("Detach from Window");
  //  QObject::connect(l_detachWindowAction, &QAction::triggered, [this](){detatchViewport();});
  //  l_menu->addAction(l_detachWindowAction);


  //  //Create the resize Actions
  //  QMenu *l_scaleMenu = l_menu->addMenu("Scale (1.76:1)");

  //  l_scaleMenu->addAction(createResizeAction(480, 272));
  //  l_scaleMenu->addAction(createResizeAction(960, 544));
  //  l_scaleMenu->addAction(createResizeAction(1280, 725));
  //  l_scaleMenu->addAction(createResizeAction(1440, 816));
  //  l_scaleMenu->addAction(createResizeAction(1920, 1088));

  //  l_menu->popup(this->mapToGlobal(t_position));
  //}
}

void ViewportOverlay::detatchViewport()
{
  QWidget* l_viewport = ThemeManager::get().getWidget("viewport");

  l_viewport->setParent(nullptr);
  l_viewport->show();
  l_viewport->move(20, 20);
}

void ViewportOverlay::resizeViewport(int t_width, int t_height)
{
  QWidget* l_viewport = ThemeManager::get().getWidget("viewport");
  l_viewport->resize(t_width, t_height);

  QStringList l_viewportChildren = ThemeManager::get().mCurrentThemeReader.GetLayerChildren("viewport");
  for(QString l_child : l_viewportChildren)
  {
    resizeWidgetAndChildren(l_child, t_width, t_height);
  }

}

InteractionObject::InteractionObject(QWidget *parent) : QWidget{parent}
{
  setMouseTracking(true);
}

void InteractionObject::setName(const QString &name)
{
  m_name = name;
}

void InteractionObject::setDescription(const QString &name)
{
  m_description = name;
}

void InteractionObject::setToggleObject(QWidget *widget)
{
  m_targetWidget = widget;
}

void InteractionObject::setTextWidget(RPTextEdit *widget)
{
  m_textWidget = widget;
}

void InteractionObject::setCursorWidget(AOImageDisplay *widget)
{
  m_cursor = widget;
}

void InteractionObject::leaveEvent(QEvent *event)
{
  if(m_targetWidget == nullptr) return;
  m_targetWidget->hide();
  if(m_cursor == nullptr) return;
  m_cursor->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_idle.png"));
}

void InteractionObject::enterEvent(QEvent *event)
{
  if(m_targetWidget == nullptr) return;
  if(m_textWidget == nullptr) return;
  m_targetWidget->show();
  m_textWidget->setText(m_name);
  if(m_cursor == nullptr) return;
  m_cursor->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_hover.png"));

  LuaBridge::LuaEventCall("OnInteractionHover", m_name.toStdString());
  audio::system::Play("cursor_hover");
}

void InteractionObject::paintEvent(QPaintEvent *event)
{
  //QPainter painter(this);
  //painter.setPen(Qt::red);
  //QRect rect(QPoint(), size());
  //painter.drawRect(rect);
}
void InteractionObject::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    LuaBridge::LuaEventCall("OnInteractionClick", m_name.toStdString(), m_description.toStdString());
    audio::system::Play("cursor_click");

    if(SceneManager::get().mPlayerDataList.count() == 0)
    {

      QStringList messagePacket =
      {
              "-1",
              "-",
              "",
              "../../misc/blank",
              m_description,
              "wit",
              "1","1",
              "0", "0", "0", "0",
              "0", "0", "0",
              m_name,
              "0", "1", "-1",
          };
      AOApplication::getInstance()->get_courtroom()->next_chatmessage(messagePacket);
      return;
    }

    courtroom::ooc::appendMessage(("[" + m_name + "]").toStdString(), ("\nDescription: " + m_description).toStdString());
  }
  else if (event->button() == Qt::RightButton)
  {
    //Preview Mode
  }
}

void InteractionObject::mouseMoveEvent(QMouseEvent *event)
{
  ViewportOverlay* overlay = qobject_cast<ViewportOverlay*>(parentWidget());
  if (overlay)
    overlay->moveCursor(event->x() + pos().x(), event->y() + pos().y());
}
