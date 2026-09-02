#ifndef CHOICE_DIALOG_H
#define CHOICE_DIALOG_H

enum class ChoiceEvent
{
  Invalid = 0,
  Pair = 1,
  Party = 2,
  Reserved = 3,
  Lua = 4

};

class ChoiceDialog : public RPWidget
{
  Q_OBJECT
public:
  explicit ChoiceDialog(QWidget *parent = nullptr);
  void resetTransform() override;

  void SetupButton(RPButton* t_button, int t_x, int t_y, int t_width, int t_height, QString name);

  void setText(QString message);

  void setSenderId(int sender);
  void setEventType(ChoiceEvent notify);
  void setKey(QString key);

private slots:
  void OnAcceptClicked();
  void OnDeclineClicked();

private:
  RPTextEdit *m_choiceText = nullptr;
  RPButton *m_acceptButton = nullptr;
  RPButton *m_declineButton = nullptr;

  QString m_senderKey = "";
  int m_senderId = -1;
  ChoiceEvent m_choiceEvent = ChoiceEvent::Invalid;

signals:

};

#endif // CHOICE_DIALOG_H
