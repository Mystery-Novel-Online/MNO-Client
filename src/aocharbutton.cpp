#include "aocharbutton.h"

#include "aoapplication.h"
#include "aoimagedisplay.h"
#include <QDesktopServices>
#include "modules/managers/character_manager.h"
#include "modules/managers/localization_manager.h"
#include "dro/fs/file_utils.h"

#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QUrl>

AOCharButton::AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos)
    : QPushButton(parent)
{
  ao_app = p_ao_app;

  this->resize(60, 60);
  this->move(x_pos, y_pos);

  ui_character = new QLabel(this);
  ui_character->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_character->resize(30, 30);
  ui_character->move(28, 28);

  ui_taken = new AOImageDisplay(this, ao_app);
  ui_taken->resize(60, 60);
  ui_taken->set_theme_image("char_taken.png");
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->hide();

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &AOCharButton::showContextMenu);
}

void AOCharButton::showContextMenu(QPoint pos)
{
  QMenu *menu = new QMenu(this);


  QAction *a = new QAction(LocalizationManager::get().getLocalizationText("CSS_FAVORITES_ADD"));
  QObject::connect(a, &QAction::triggered, [this](){addToFavorites();});
  menu->addAction(a);


  QAction *copyIDAction = new QAction(LocalizationManager::get().getLocalizationText("CSS_FAVORITES_REMOVE"));
  QObject::connect(copyIDAction, &QAction::triggered, [this](){removeFavorites();});
  menu->addAction(copyIDAction);

  QAction *opencharfolder = new QAction(LocalizationManager::get().getLocalizationText("OPEN_CHAR_FOLDER"));
  QObject::connect(opencharfolder, &QAction::triggered, [this](){openCharacterFolder();});
  menu->addAction(opencharfolder);

  menu->popup(this->mapToGlobal(pos));
}


QString AOCharButton::character()
{
  return m_character;
}

void AOCharButton::set_character(QString p_character, QString p_character_ini)
{
  m_character = p_character;
  const QString l_icon_path = ao_app->get_character_path(m_character, "char_icon.png");
  const bool l_file_exist = FSChecks::FileExists(l_icon_path);
  setStyleSheet(l_file_exist ? QString("AOCharButton { border-image: url(\"%1\");  }").arg(l_icon_path) : nullptr);
  const QString l_final_character = QString(m_character).replace("&", "&&");
  setText(l_file_exist ? nullptr : l_final_character);

  const bool l_is_different_chr = m_character != p_character_ini;
  if (l_is_different_chr)
    ui_character->setStyleSheet(
        QString("border-image: url(\"%1\");").arg(ao_app->get_character_path(p_character_ini, "char_icon.png")));
  ui_character->setVisible(l_is_different_chr);
  setToolTip(l_is_different_chr ? QString("%1 as %2").arg(m_character, QString(p_character_ini).replace("&", "&&"))
                                : l_final_character);
}

void AOCharButton::set_taken(const bool p_enabled)
{
  ui_taken->setVisible(p_enabled);
}

void AOCharButton::addToFavorites()
{
  CharacterManager::get().AddToFavorites(m_character);
}

void AOCharButton::removeFavorites()
{
  CharacterManager::get().RemoveFromFavorites(m_character);
}

void AOCharButton::openCharacterFolder()
{
  QString folderPath = "characters/" + m_character;

  QUrl folderUrl = QUrl::fromLocalFile(ao_app->get_package_or_base_path(folderPath));

  QDesktopServices::openUrl(folderUrl);
}

void AOCharButton::enterEvent(QEvent *e)
{
  setFlat(false);
  QPushButton::enterEvent(e);
  Q_EMIT mouse_entered(this);
}

void AOCharButton::leaveEvent(QEvent *e)
{
  QPushButton::leaveEvent(e);
  Q_EMIT mouse_left();
}
