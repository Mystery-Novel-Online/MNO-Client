#ifndef RP_VIEWPORT_H
#define RP_VIEWPORT_H

#include <QObject>

class RPViewport : public QObject
{
  Q_OBJECT
public:
  explicit RPViewport(QObject *parent = nullptr);

  virtual void constructViewport() = 0;
  virtual void update() = 0;
  virtual void loadCurrentMessage() = 0;
  virtual void loadBackground(QString background) = 0;
  virtual void refreshBackground(QString position) = 0;
  virtual void toggleChatbox(bool state) = 0;

signals:
  void videoDone();
  void preanimDone();
};

#endif // RP_VIEWPORT_H
