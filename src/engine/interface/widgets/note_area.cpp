#include "note_area.h"
#include "pch.h"

#include "note_picker.h"
#include "theme.h"
#include "engine/fs/fs_reading.h"

AONoteArea::AONoteArea(QWidget *p_parent, AOApplication *p_ao_app)
    : AOImageDisplay(p_parent, p_ao_app)
{
  ao_app = p_ao_app;
}
