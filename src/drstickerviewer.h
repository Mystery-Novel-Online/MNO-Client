#pragma once

#include "mk2/spriteviewer.h"

class AOApplication;

class DRStickerViewer : public mk2::SpriteViewer
{
  Q_OBJECT

public:
  explicit DRStickerViewer(AOApplication *ao_app, QWidget *parent = nullptr);
  ~DRStickerViewer();

  void set_hide_when_done(bool enabled);

  void set_theme_image(QString p_file_name, QString p_character);

  void set_theme_image(QString p_file_name);

  void set_chatbox_image(QString p_chatbox_name, bool p_is_self, bool chatmessage_is_empty, QString pair);

private:
  AOApplication *ao_app = nullptr;

  bool m_hide_when_done;

private slots:
  void maybe_hide();
};
