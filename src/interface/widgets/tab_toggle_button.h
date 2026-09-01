#ifndef TABTOGGLEBUTTON_H
#define TABTOGGLEBUTTON_H

class TabToggleButton : public RPButton
{
  Q_OBJECT
public:
  TabToggleButton(QWidget *parent, AOApplication *p_ao_app);

  void refreshButtonStatus();
  void setTabName(QString t_name);
  void setTabGroup(QString t_name);
  void setActiveStatus(bool t_isActive);

signals:
  void tabClicked(TabToggleButton *emote_number);

private slots:
  void on_clicked();
  void detatchTab();

private:
  QString mTabName = "";
  QString mTabGroup = "default";
  bool mTabActive = false;
  bool mIsDetatched = false;

};

#endif // TABTOGGLEBUTTON_H
