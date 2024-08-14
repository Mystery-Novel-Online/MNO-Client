#include "investigation_display.h"

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

}
