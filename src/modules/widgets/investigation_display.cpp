#include "investigation_display.h"
#include "datatypes.h"
#include "commondefs.h"
#include "theme.h"

ViewportInvestigationDisplay::ViewportInvestigationDisplay(QWidget *parent, AOApplication *p_ao_ap) : AOImageDisplay(parent, p_ao_ap)
{
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setCursor(Qt::BlankCursor);

  m_MouseUpdateTimer = new QTimer(this);
  m_MouseUpdateTimer->setSingleShot(true);
  connect(m_MouseUpdateTimer, &QTimer::timeout, this, &ViewportInvestigationDisplay::onMouseMoveTimeout);

  m_CursorOverlay = new AOImageDisplay(this, p_ao_ap);
  m_CursorOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_CursorOverlay->resize(61, 61);
  m_CursorOverlay->set_image(p_ao_ap->find_theme_asset_path("cursor_idle.png"));

  m_NameDisplay = new AOImageDisplay(this, p_ao_ap);
  m_NameDisplay->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_NameDisplay->resize(300, 40);
  m_NameDisplay->move(330, 10);
  m_NameDisplay->set_image(p_ao_ap->find_theme_asset_path("cursor_name_display.png"));
  m_NameDisplay->hide();


  m_NameText = new DRTextEdit(m_NameDisplay);
  m_NameText->move(4, 7);
  m_NameText->resize(292, 31);
  //ThemeManager::get().AutoAdjustWidgetDimensions(m_TextShowname, "showname", SceneTypeViewport, m_ViewportScale);
  m_NameText->setFrameStyle(QFrame::NoFrame);
  m_NameText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_NameText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_NameText->setReadOnly(true);
  set_drtextedit_font(m_NameText, "cursor_text", COURTROOM_FONTS_INI, p_ao_ap);
  m_NameText->setText("Object");
}
