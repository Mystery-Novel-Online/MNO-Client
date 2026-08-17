#ifndef AREAPLAYERLIST_H
#define AREAPLAYERLIST_H

#include <QObject>
#include <QWidget>

class AreaPlayerList : public QWidget
{
  Q_OBJECT
public:
  explicit AreaPlayerList(QWidget *parent = nullptr);
  ~AreaPlayerList();
  void deconstruct();
  void constructLayout();

  void startClientTyping(int clientId, bool active);

  void assignNavigationButtons(RPButton* left, RPButton* right);

public slots:
  void navigationClickedLeft();
  void navigationClickedRight();

signals:
  void navigationComplete();

private:
  QVector<DrPlayerListEntry *> m_playerEntries = {};
  int m_pageMax = 10;
  int m_pageCurrent = 0;

  int m_playerRows = 0;

  RPButton* m_navigationLeft = nullptr;
  RPButton* m_navigationRight = nullptr;
};

#endif // AREAPLAYERLIST_H
