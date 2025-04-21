#include "drdiscord.h"

#include <QDateTime>
#include <QDebug>
#include <QGlobalStatic>
#include <QTimer>

#include <cstring>

Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, DISCORD_APPLICATION_ID, ("818850172330442793"));
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, DISCORD_LARGE_IMAGE_KEY, ("danganronpa_online"));
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, DISCORD_LARGE_IMAGE_TEXT, ("Sore Wa Chigau Yo!"));

QByteArray resize_buf(const QString &f_str_message, const int f_max_size)
{
  QByteArray l_message = f_str_message.toUtf8();

  // reduce by one to take '\0' in account
  const int l_target_max_size = f_max_size - 1;

  if (l_message.size() > l_target_max_size)
  {
    const QByteArray l_omission_text("...");
    l_message = l_message.mid(0, l_target_max_size - l_omission_text.size()) + l_omission_text;
  }

  return l_message;
}

DRDiscord::DRDiscord(QObject *f_parent)
    : QObject(f_parent)
{
  m_waiter = new QTimer(this);
  m_waiter->setInterval(std::chrono::seconds(1));
  m_waiter->setSingleShot(true);
  connect(m_waiter, SIGNAL(timeout()), this, SLOT(on_update_queued()));
  connect(this, SIGNAL(options_changed(DRDiscord::Options)), m_waiter, SLOT(start()));
  connect(this, SIGNAL(state_changed(DRDiscord::State)), m_waiter, SLOT(start()));
  connect(this, SIGNAL(server_name_changed(QString)), m_waiter, SLOT(start()));
  connect(this, SIGNAL(server_name_cleared()), m_waiter, SLOT(start()));
  connect(this, SIGNAL(character_name_changed(QString)), m_waiter, SLOT(start()));
  connect(this, SIGNAL(character_name_cleared()), m_waiter, SLOT(start()));
  connect(this, SIGNAL(started()), m_waiter, SLOT(start()));
  connect(this, SIGNAL(stopped()), m_waiter, SLOT(start()));

  start();
}

DRDiscord::~DRDiscord()
{
  this->disconnect(m_waiter);
  stop();
}

void DRDiscord::start(const bool p_restart)
{
  if (p_restart)
    stop();
  if (m_started)
    return;
  m_started = true;
  m_timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
  qInfo().noquote() << QString("[Discord-RPC] %1").arg(p_restart ? "Restarting" : "Starting");
  // reset & set
  std::memset(&m_handler, 0, sizeof(m_handler));
  m_handler.ready = [](const DiscordUser *user) {
    qInfo().noquote() << QString("[Discord-RPC] Ready, as username: %1").arg(user->username);
  };
  m_handler.disconnected = [](int errorCode, const char *message) {
    qInfo().noquote() << QString("[Discord-RPC] Disconnected, code %1: %2").arg(errorCode).arg(message);
  };
  m_handler.errored = [](int errorCode, const char *message) {
    qWarning().noquote() << QString("[Discord-RPC] Error %1: %2").arg(errorCode).arg(message);
  };
  Discord_Initialize(DISCORD_APPLICATION_ID->constData(), &m_handler, 1, nullptr);
  Q_EMIT started();
}

void DRDiscord::stop()
{
  if (!m_started)
    return;
  m_started = false;
  m_timestamp = 0;
  qInfo().noquote() << "[Discord-RPC] Shutting down";
  Discord_Shutdown();
  Q_EMIT stopped();
}

DRDiscord::Options DRDiscord::get_options() const
{
  return m_options;
}

bool DRDiscord::option_enabled(const DRDiscord::Option p_option) const
{
  return m_options.testFlag(p_option);
}

bool DRDiscord::presence_enabled() const
{
  return option_enabled(OPresence);
}

bool DRDiscord::hide_server_enabled() const
{
  return option_enabled(OHideServer);
}

bool DRDiscord::hide_character_enabled() const
{
  return option_enabled(OHideCharacter);
}

DRDiscord::State DRDiscord::get_state() const
{
  return m_state;
}

bool DRDiscord::is_state(const DRDiscord::State p_state) const
{
  return m_state == p_state;
}

bool DRDiscord::is_idle() const
{
  return is_state(State::Idle);
}

bool DRDiscord::is_connected() const
{
  return is_state(State::Connected);
}

void DRDiscord::set_options(const Options &f_options)
{
  if (m_options == f_options)
    return;
  m_options = f_options;
  Q_EMIT options_changed(m_options);
}

void DRDiscord::set_option(const DRDiscord::Option &f_option, const bool p_enabled)
{
  if (option_enabled(f_option) == p_enabled)
    return;
  Options l_options = m_options;
  l_options.setFlag(f_option, p_enabled);
  set_options(l_options);
}

void DRDiscord::set_presence(const bool p_enabled)
{
  set_option(OPresence, p_enabled);
}

void DRDiscord::set_hide_server(const bool p_enabled)
{
  set_option(OHideServer, p_enabled);
}

void DRDiscord::set_hide_character(const bool p_enabled)
{
  set_option(OHideCharacter, p_enabled);
}

void DRDiscord::set_state(const DRDiscord::State f_state)
{
  if (m_state == f_state)
    return;
  m_state = f_state;
  Q_EMIT state_changed(m_state);
}

void DRDiscord::set_server_name(const QString &f_server_name)
{
  Q_ASSERT_X(!f_server_name.trimmed().isEmpty(), "DRDiscord", "a server name is required");
  if (m_server_name.has_value() && m_server_name.value() == f_server_name)
    return;
  m_server_name = f_server_name;
  Q_EMIT server_name_changed(m_server_name.value());
}

void DRDiscord::clear_server_name()
{
  m_server_name.reset();
  Q_EMIT server_name_cleared();
}

void DRDiscord::set_character_name(const QString &f_character_name)
{
  Q_ASSERT_X(!f_character_name.trimmed().isEmpty(), "DRDiscord", "a server name is required");
  if (m_character_name.has_value() && m_character_name.value() == f_character_name)
    return;
  m_character_name = f_character_name;
  Q_EMIT character_name_changed(m_character_name.value());
}

void DRDiscord::clear_character_name()
{
  m_character_name.reset();
  Q_EMIT character_name_cleared();
}

void DRDiscord::on_update_queued()
{
  if (!presence_enabled())
  {
    Discord_ClearPresence();
    return;
  }

  switch (m_state)
  {
  default:
  case State::Idle:
    m_buf_details.clear();
    if (!hide_server_enabled())
    {
      m_buf_details = "Lobby";
    }
    m_buf_state.clear();
    break;

  case State::Connected:
    m_buf_details.clear();
    if (!hide_server_enabled())
    {
      m_buf_details = QString("In: %1")
                          .arg(m_server_name.has_value() ? m_server_name.value().toUtf8() : QString("<private>"))
                          .toUtf8();
    }

    m_buf_state.clear();
    if (!hide_character_enabled())
    {
      m_buf_state.clear();
      m_buf_state = m_character_name.has_value() ? QString("As: %1").arg(m_character_name.value()).toUtf8()
                                                 : QByteArray("Spectating");
    }
    break;
  }

  // respect max size for each fields; see discord_rpc.h struct
  m_buf_details = resize_buf(QString::fromUtf8(m_buf_details), 128);
  m_buf_state = resize_buf(QString::fromUtf8(m_buf_state), 128);

  // reset & set presence
  std::memset(&m_presence, 0, sizeof(m_presence));
  m_presence.largeImageKey = DISCORD_LARGE_IMAGE_KEY->constData();
  m_presence.largeImageText = DISCORD_LARGE_IMAGE_TEXT->constData();
  m_presence.state = m_buf_state.constData();
  m_presence.details = m_buf_details.constData();
  m_presence.startTimestamp = m_timestamp;
  Discord_UpdatePresence(&m_presence);
}
