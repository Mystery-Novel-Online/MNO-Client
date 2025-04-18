#ifndef DROEMOTEBUTTONS_H
#define DROEMOTEBUTTONS_H

#include <AOApplication.h>
#include <QObject>
#include <QWidget>
#include "dro/interface/menus/emote_menu.h"

class DROEmoteButtons : public QWidget
{
  Q_OBJECT
public:
  explicit DROEmoteButtons(QWidget *parent);
  void EmoteChange(DREmote emote);

private:
  EmoteMenu* m_ContextMenu = nullptr;
signals:

protected:
  void wheelEvent(QWheelEvent *event) override;

};

#endif // DROEMOTEBUTTONS_H
