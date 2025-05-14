#ifndef CHAR_BUTTON_H
#define CHAR_BUTTON_H

class AOApplication;
class AOImageDisplay;

#include <QPushButton>

class QLabel;

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos);

  QString character();
  void set_character(QString character, QString ini_character);
  void set_taken(const bool);
signals:
  void mouse_entered(AOCharButton *p_caller);
  void mouse_left();

private slots:
  void addToFavorites();
  void removeFavorites();
  void openCharacterFolder();

protected:
  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);

private:
  AOApplication *ao_app = nullptr;

  QString m_character;

  QLabel *ui_character = nullptr;
  AOImageDisplay *ui_taken = nullptr;

  void showContextMenu(QPoint pos);
};

#endif // CHAR_BUTTON_H
