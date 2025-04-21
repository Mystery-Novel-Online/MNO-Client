#pragma once

#include <discord-rpc/discord_rpc.h>

#include <QObject>
#include <QPointer>

class QTimer;

#include <optional>

class DRDiscord : public QObject
{
  Q_OBJECT

public:
  enum Option : uint32_t
  {
    OPresence = 0x1,
    OHideServer = 0x2,
    OHideCharacter = 0x4,
  };
  Q_DECLARE_FLAGS(Options, Option)

  enum class State
  {
    Idle,
    Connected,
  };
  Q_ENUM(State)

  DRDiscord(QObject *f_parent = nullptr);
  ~DRDiscord();

  Options get_options() const;
  bool option_enabled(const Option p_option) const;
  bool presence_enabled() const;
  bool hide_server_enabled() const;
  bool hide_character_enabled() const;
  State get_state() const;
  bool is_state(const State p_state) const;
  bool is_idle() const;
  bool is_connected() const;

public slots:
  void set_options(const DRDiscord::Options &f_options);
  void set_option(const DRDiscord::Option &f_option, const bool p_enabled);
  void set_presence(const bool p_enabled);
  void set_hide_server(const bool p_enabled);
  void set_hide_character(const bool p_enabled);
  void set_state(const DRDiscord::State f_state);
  void set_server_name(const QString &f_server_name);
  void clear_server_name();
  void set_character_name(const QString &f_character_name);
  void clear_character_name();

  void start(const bool p_restart = false);
  void stop();

signals:
  void options_changed(DRDiscord::Options);
  void state_changed(DRDiscord::State);
  void server_name_changed(QString);
  void server_name_cleared();
  void character_name_changed(QString);
  void character_name_cleared();
  void started();
  void stopped();

private:
  Options m_options;
  State m_state = State::Idle;
  QPointer<QTimer> m_waiter;
  DiscordEventHandlers m_handler;
  DiscordRichPresence m_presence;
  bool m_started = false;
  std::optional<QString> m_server_name;
  std::optional<QString> m_character_name;
  QByteArray m_buf_details, m_buf_state;
  int64_t m_timestamp;

private slots:
  void on_update_queued();
};
