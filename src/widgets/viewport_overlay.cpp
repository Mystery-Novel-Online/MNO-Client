#include "viewport_overlay.h"

#include <modules/theme/legacythememanager.h>
#include <modules/managers/scene_manager.h>
#include "interface/courtroom_layout.h"
#include "system/theme_scripting.h"
#include "system/audio.h"

ViewportOverlay::ViewportOverlay(QWidget *parent)
    : QWidget{parent}
{
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
  for(InteractionObject *interaction : m_Interactions) {
    delete interaction;
  }

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

void ViewportOverlay::resizeWidgetAndChildren(QString t_widget, int t_width, int t_height)
{
  
  QWidget* l_widget = LegacyThemeManager::get().getWidget(t_widget);
  if(!l_widget){
    return;
  }

  int l_viewportNativeHeight = LegacyThemeManager::get().mCurrentThemeReader.GetWidgetTransform(ThemeSceneType::SceneType_Courtroom, "viewport").height;
  RPRect l_widgetSize = LegacyThemeManager::get().mCurrentThemeReader.GetWidgetTransform(ThemeSceneType::SceneType_Courtroom, t_widget);

  double l_scalingFactor = static_cast<double>(t_height) / l_viewportNativeHeight;

  int l_scaledWidth = static_cast<int>(l_widgetSize.width * l_scalingFactor);
  int l_scaledHeight = static_cast<int>(l_widgetSize.height * l_scalingFactor);
  int l_scaledX = static_cast<int>(l_widgetSize.x * l_scalingFactor);
  int l_scaledY = static_cast<int>(l_widgetSize.y * l_scalingFactor);

  LegacyThemeManager::get().setViewporResize(l_scalingFactor);

  if(t_widget.startsWith("viewport"))
  {
    l_scaledX = 0;
    l_scaledY = 0;
    l_scaledWidth = t_width;
    l_scaledHeight = t_height;
  }

  l_widget->resize(l_scaledWidth, l_scaledHeight);
  l_widget->move(l_scaledX, l_scaledY);

  if(dynamic_cast<AOImageDisplay*>(l_widget) != nullptr)
  {
    AOImageDisplay* l_imageDisplay = dynamic_cast<AOImageDisplay*>(l_widget);
    l_imageDisplay->refreshImage();
  }

  QStringList l_viewportChildren = LegacyThemeManager::get().mCurrentThemeReader.GetLayerChildren(t_widget);
  for(QString l_child : l_viewportChildren) {
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
  if(!m_targetWidget || !m_cursor) {
    return;
  }
  m_targetWidget->hide();
  m_cursor->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_idle.png"));
}

void InteractionObject::enterEvent(QEvent *event)
{
  if(!m_targetWidget || !m_textWidget || !m_cursor) {
    return;
  }
  m_targetWidget->show();
  m_textWidget->setText(m_name);
  m_cursor->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_hover.png"));

  LuaBridge::LuaEventCall("OnInteractionHover", m_name.toStdString());
  audio::system::Play("cursor_hover");
}

void InteractionObject::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    LuaBridge::LuaEventCall("OnInteractionClick", m_name.toStdString(), m_description.toStdString());
    audio::system::Play("cursor_click");

    if(SceneManager::get().m_areaPlayers.count() == 0) {
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
  else if(event->button() == Qt::RightButton)
  {
    // TODO: Right clicking should overlay the background on the viewport so it can be seen a bit more clearly during dialogue.
  }
}

void InteractionObject::mouseMoveEvent(QMouseEvent *event)
{
  ViewportOverlay* overlay = qobject_cast<ViewportOverlay*>(parentWidget());
  if(!overlay) {
    return;
  }
  overlay->moveCursor(event->x() + pos().x(), event->y() + pos().y());
}
