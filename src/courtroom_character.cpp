#include "courtroom.h"

#include "aoapplication.h"
#include "aoconfig.h"
#include "commondefs.h"
#include "drpacket.h"
#include "file_functions.h"
#include "theme.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QCompleter>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QListView>
#include <QPixmap>
#include <QUrl>
#include "modules/debug/time_debugger.h"
#include "modules/managers/character_manager.h"
#include <QtConcurrent/QtConcurrent>

int Courtroom::get_character_id()
{
  return m_chr_id;
}

void Courtroom::set_character_id(const int p_chr_id)
{
  if (p_chr_id != SpectatorId && m_chr_id == p_chr_id)
    return;
  m_chr_id = p_chr_id;
  load_character();
  Q_EMIT character_id_changed(m_chr_id);
}

QString Courtroom::get_character()
{
  return is_spectating() ? nullptr : CharacterManager::get().mServerCharacters.at(m_chr_id).name;
}

QString Courtroom::get_character_ini()
{
  return ao_config->character_ini(get_character());
}

QString Courtroom::get_character_content_url()
{
  QFile l_contentFile(ao_app->get_character_path(get_character_ini(), "CONTENT.txt"));
  if (!l_contentFile.open(QIODevice::ReadOnly))
    return nullptr;

  const QUrl l_url(QString(l_contentFile.readAll()).simplified());
  if (l_url.isRelative() || l_url.isLocalFile())
    return nullptr;

  return l_url.toString(QUrl::FullyEncoded);
}

namespace
{
void drSetItemIcon(QComboBox *p_widget, const int p_index, const QString &p_chr_name, AOApplication *ao_app)
{
  static const QIcon s_blank_icon = []() -> QIcon {
    QPixmap l_blank_texture(64, 64);
    l_blank_texture.fill(Qt::transparent);
    return QIcon(l_blank_texture);
  }();

  const QString l_icon_file = ao_app->get_character_path(p_chr_name, "char_icon.png");
  p_widget->setItemIcon(p_index, file_exists(l_icon_file) ? QIcon(l_icon_file) : s_blank_icon);
}
} // namespace

void Courtroom::update_iniswap_list()
{
  //ui_iniswap_dropdown->setEditable(false);

  {
    QSignalBlocker b_ini_list(ui_iniswap_dropdown);
    ui_iniswap_dropdown->clear();

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, &Courtroom::UpdateIniswapList);

    QFuture<void> future = QtConcurrent::run(this, &Courtroom::SearchForCharacterListAsync);
    watcher->setFuture(future);
  }

}


void Courtroom::SearchForCharacterListAsync()
{
  currentIniswapList = QStringList{"Default"};

  QStringList l_package_folders{};

  for (int i=0; i< ao_app->package_names.size(); i++)
  {
    if(!ao_app->m_disabled_packages.contains(ao_app-> package_names.at(i)))
    {
      const QString l_path = ao_app->get_package_path(ao_app->package_names.at(i)) + "/characters";
      if(dir_exists(l_path)) l_package_folders.append(l_path);
    }
  }

  l_package_folders.append(ao_app->get_base_path() + "/characters");

  for (int i = 0; i < l_package_folders.length(); ++i)
  {
    const QFileInfoList l_info_list = QDir(l_package_folders.at(i)).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &i_info : l_info_list)
    {
      const QString l_name = i_info.fileName();
      if (!file_exists(ao_app->get_character_path(l_name, CHARACTER_CHAR_INI)) && !file_exists(ao_app->get_character_path(l_name, CHARACTER_CHAR_JSON)))
        continue;
      if(!currentIniswapList.contains(l_name))
      {
        currentIniswapList.append(l_name);
      }
    }
  }

}

void Courtroom::UpdateIniswapList()
{
  ui_iniswap_dropdown->addItems(currentIniswapList);

  update_default_iniswap_item();
  select_base_character_iniswap();
  iniswapTimer = new QTimer(this);
  //UpdateIniswapIcons(true, 2);
  update_iniswap_dropdown_searchable();


}

void Courtroom::UpdateIniswapIcons(bool reset, int batch_count, int starting_index)
{

  if(reset)
  {
    if (iniswapTimer && iniswapTimer->isActive())
    {
      iniswapTimer->stop();
    }

    iniswapTimer = new QTimer(this);

    currentIniswapIconIndex = starting_index;
  }


  for (int i = 0; i < batch_count && currentIniswapIconIndex < currentIniswapList.length(); ++i, ++currentIniswapIconIndex)
  {
    const QString &i_name = currentIniswapList.at(currentIniswapIconIndex);
    drSetItemIcon(ui_iniswap_dropdown, currentIniswapIconIndex, i_name, ao_app);
  }

  if (currentIniswapIconIndex < currentIniswapList.length())
  {
    if (iniswapTimer && iniswapTimer->isActive())
    {
      iniswapTimer->stop();
    }

    iniswapTimer = new QTimer(this);
    connect(iniswapTimer, &QTimer::timeout, this, &Courtroom::OnIniswapTimerTimeout);
    iniswapTimer->start(100);
  }
  else
  {
    if (iniswapTimer && iniswapTimer->isActive())
    {
      iniswapTimer->stop();
    }
  }


}


void Courtroom::OnIniswapTimerTimeout()
{
  UpdateIniswapIcons(false, 2);
}

void Courtroom::SetChatboxFocus()
{
  ui_ic_chat_message_field->setFocus();
}


void Courtroom::update_default_iniswap_item()
{
  drSetItemIcon(ui_iniswap_dropdown, 0, get_character(), ao_app);
}

void Courtroom::select_base_character_iniswap()
{
  const QString l_current_chr = get_character_ini();
  if (get_character() == l_current_chr)
  {
    ui_iniswap_dropdown->setCurrentIndex(0);
    return;
  }
  ui_iniswap_dropdown->setCurrentText(l_current_chr);
}

void Courtroom::refresh_character_content_url()
{
  const QString l_new_content_url = get_character_content_url();
  if (m_character_content_url == l_new_content_url)
    return;
  m_character_content_url = l_new_content_url;
  ao_app->send_server_packet(DRPacket("FS", {m_character_content_url}));
}

void Courtroom::on_iniswap_dropdown_changed(int p_index)
{
  ao_config->set_character_ini(get_character(),
                               p_index == 0 ? get_character() : ui_iniswap_dropdown->itemText(p_index));
}

void Courtroom::onCharacterSelectPackageChanged(int p_index)
{
  m_current_chr_page = 0;
  set_char_select_page();
}

void Courtroom::update_iniswap_dropdown_searchable()
{
  const bool l_enabled = ao_config->searchable_iniswap_enabled();
  ui_iniswap_dropdown->setEditable(l_enabled);
  if (l_enabled)
  {
    QCompleter *l_completer = ui_iniswap_dropdown->completer();
    l_completer->setCompletionMode(QCompleter::PopupCompletion);
    l_completer->setFilterMode(Qt::MatchContains);
    QAbstractItemView *l_list = l_completer->popup();
    l_list->setTextElideMode(Qt::TextElideMode::ElideNone);
    l_list->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  }
}

void Courtroom::UpdateIniswapStylesheet()
{
  update_iniswap_dropdown_searchable();
  ui_iniswap_dropdown->blockSignals(true);
  ui_iniswap_dropdown->clear();
  set_stylesheet(ui_iniswap_dropdown, "[INISWAP DROPDOWN]", COURTROOM_STYLESHEETS_CSS, ao_app);
  update_iniswap_list();
  ui_iniswap_dropdown->blockSignals(false);
}
