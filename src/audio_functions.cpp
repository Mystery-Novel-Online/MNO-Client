#include "courtroom.h"

#include "draudioengine.h"
#include "dro/system/theme_scripting.h"

bool Courtroom::is_audio_suppressed() const
{
  return is_audio_muted;
}

void Courtroom::suppress_audio(bool p_enabled)
{
  is_audio_muted = p_enabled;

  // suppress audio
  for (auto &family : DRAudioEngine::get_family_list())
  {
    family->set_suppressed(is_audio_muted);
  }
}

void Courtroom::stop_all_audio()
{
  for (auto &family : DRAudioEngine::get_family_list())
    for (auto &stream : family->get_stream_list())
      stream->stop();
}

void Courtroom::keyPressEvent(QKeyEvent *event)
{
  if (event)
  {
    int key = event->key();
    QString keyText = QKeySequence(event->key()).toString();
    LuaBridge::LuaEventCall("OnKeyboardInput", keyText.toStdString(), key);
  }
}
