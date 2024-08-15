#ifndef INVESTIGATION_DISPLAY_H
#define INVESTIGATION_DISPLAY_H

#include "qdebug.h"
#include "qevent.h"
#include "qtimer.h"
#include <QObject>
#include <QWidget>
#include <aoimagedisplay.h>
#include <qpainter.h>
#include <modules/managers/audio_manager.h>

class InvestigationObject
{
public:
  InvestigationObject(QString t_name, QRect t_rectangle)
  {
    m_Name = t_name;
    m_Rect = t_rectangle;
  };

  void SetDescription(QString t_desc)
  {
    if(t_desc.trimmed().isEmpty()) return;
    m_Description = t_desc;
  }

  QString GetObjectName()
  {
    return m_Name;
  }

  QString GetDescription()
  {
    return m_Description;
  }

  bool CheckCollision(int x, int y, double t_scale)
  {
    if(x < (m_Rect.x() * t_scale)) return false;
    if(y < (m_Rect.y() * t_scale)) return false;
    if(x > ((m_Rect.x() * t_scale) + (m_Rect.width() * t_scale))) return false;
    if(y > ((m_Rect.y() * t_scale) + (m_Rect.height() * t_scale))) return false;

    return true;
  }


private:
  QRect m_Rect = QRect();
  QString m_Name = "Object";
  QString m_Description = "This object isn't too interesting.";
};

class ViewportInvestigationDisplay : public AOImageDisplay
{
  Q_OBJECT
public:
  ViewportInvestigationDisplay(QWidget *parent, AOApplication *p_ao_ap);

  void ClearObjects()
  {
    l_InvestigationActive = false;
    m_CurrentObject = nullptr;
    for(InvestigationObject * r_obj : m_AreaObjects)
      delete r_obj;

    m_AreaObjects.clear();
  }

  void AddObject(InvestigationObject * r_object)
  {
    m_AreaObjects.append(r_object);
  }

protected:
  void mouseMoveEvent(QMouseEvent *event) override
  {
    if (!m_MouseUpdateTimer->isActive())
    {
      m_MouseUpdateTimer->start(15);
    }
    m_LastMousePosition = event->pos();
  }

  void enterEvent(QEvent *event) override
  {
    if(m_AreaObjects.count() > 0)
    {
      setCursor(Qt::BlankCursor);
      l_InvestigationActive = true;
    }
  }

  void leaveEvent(QEvent *event) override
  {
    setCursor(Qt::ArrowCursor);
    l_InvestigationActive = false;
    m_CursorOverlay->move(9999, 9999);
    UpdateAlpha(0);
  }

  void mousePressEvent(QMouseEvent *event) override
  {
    if (event->button() == Qt::LeftButton)
    {
      if(m_CurrentObject != nullptr)
      {
        emit InteractionClicked(m_CurrentObject);
      }
    }
    else if (event->button() == Qt::RightButton)
    {
      if(l_InvestigationActive)
      {
        UpdateAlpha(150);
      }
    }
  }

signals:
  void InteractionClicked(InvestigationObject * t_Interaction);

private slots:
  void onMouseMoveTimeout()
  {
    if (l_InvestigationActive)
    {
      double scaleFactor = static_cast<double>(this->width()) / static_cast<double>(960);
      int l_PositionX = m_LastMousePosition.x() - 30;
      int l_PositionY = m_LastMousePosition.y() - 30;
      bool l_FoundHighlight = false;
      for(InvestigationObject * r_object: m_AreaObjects)
      {
        l_FoundHighlight = r_object->CheckCollision(m_LastMousePosition.x(), m_LastMousePosition.y(), scaleFactor);
        if(l_FoundHighlight == true)
        {
          if(m_CurrentObject == nullptr)
          {
            AudioManager::get().PlaySFX("cursor_hover");
          }
          m_CurrentObject = r_object;
          break;
        }
      }

      if(l_FoundHighlight)
      {
        m_CursorOverlay->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_hover.png"), false);
      }
      else
      {
        m_CurrentObject = nullptr;
        m_CursorOverlay->set_image(AOApplication::getInstance()->find_theme_asset_path("cursor_idle.png"), false);
      }

      m_CursorOverlay->move(l_PositionX, l_PositionY);
    }
  }

private:
  bool l_InvestigationActive = false;
  QTimer *m_MouseUpdateTimer;
  QPoint m_LastMousePosition;

  AOImageDisplay *m_CursorOverlay = nullptr;
  QList<InvestigationObject *> m_AreaObjects = {};
  InvestigationObject *m_CurrentObject = nullptr;

  QString l_HoverPath = "";
  QString l_CursorPath = "";
};

#endif // INVESTIGATION_DISPLAY_H
