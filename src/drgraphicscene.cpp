/**************************************************************************
**
** mk2
** Copyright (C) 2022 Tricky Leifa
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Affero General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/
#include "drgraphicscene.h"
#include "pch.h"

DRGraphicsView::DRGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
{
  setInteractive(false);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setFrameShape(QFrame::NoFrame);
  setFrameStyle(0);

  setScene(m_scene);

  setBackgroundBrush(Qt::black);
}

DRGraphicsView::~DRGraphicsView()
{}

void DRGraphicsView::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);
  for (QGraphicsItem *i_item : scene()->items())
  {
    auto l_object = dynamic_cast<QObject *>(i_item);
    if (l_object)
    {
      l_object->setProperty("size", event->size());
    }
  }
  m_scene->setSceneRect(rect());
  setSceneRect(rect());
}
