#ifndef SERVER_SELECT_ENTRY_H
#define SERVER_SELECT_ENTRY_H

#include <QObject>
#include <QWidget>

class FavoritesLabel : public QLabel
{
  Q_OBJECT
public:
  explicit FavoritesLabel(QWidget* parent = nullptr) : QLabel(parent)
  {
    setCursor(Qt::PointingHandCursor);
  }

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent* event) override
  {
    if (event->button() == Qt::LeftButton)
      emit clicked();

    QLabel::mousePressEvent(event);
  }
};

class ServerSelectEntry : public QWidget
{
  Q_OBJECT
public:
  ServerSelectEntry(const QString& title, QWidget *parent = nullptr);
  void setId(int id) { m_id = id; }
  int id() const { return m_id; }

  void setIcon(QString path);

signals:
  void clicked(int id);
  void favoriteToggled(int id);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  void setupLayout();
  QWidget* createMainRow();
  QWidget* createIcon();
  QVBoxLayout* createText();
  void createShadow();

private:
  QString m_title;
  int m_id;

  QVBoxLayout *m_rootLayout;
  QHBoxLayout *m_mainLayout;
  QVBoxLayout *m_childrenLayout;

  FavoritesLabel *m_iconLabel = nullptr;
  QLabel* m_titleLabel = nullptr;
  QPushButton* m_favoriteButton = nullptr;
};


#endif // SERVER_SELECT_ENTRY_H
