#ifndef AREAPLAYERLIST_H
#define AREAPLAYERLIST_H

#include <QObject>
#include <QWidget>

class AreaPlayerList : public QWidget
{
  Q_OBJECT
public:
  explicit AreaPlayerList(QWidget *a_parent = nullptr);
  ~AreaPlayerList();

  void deconstruct(bool a_destroyAll = false);
  void constructLayout();

  void startClientTyping(int a_targetClient, bool a_activeState);

  void assignNavigationButtons(RPButton* a_leftButton, RPButton* a_rightButton);

private:
  void updatePageNavigation();
  void populatePlayers();
  int calculateEntryCount();

public slots:
  void navigationClickedLeft();
  void navigationClickedRight();

signals:
  void navigationComplete();

private:
  QMap<int, AreaPlayerEntry *> u_playerEntries = {};

  int m_pageMax = 10;
  int m_pageCurrent = 0;
  int m_playerRows = 0;
  int m_playerSpacing = 0;

  RPButton* u_navigationLeft = nullptr;
  RPButton* u_navigationRight = nullptr;
};

#endif // AREAPLAYERLIST_H
