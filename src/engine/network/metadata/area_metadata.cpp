#include "area_metadata.h"

static QString s_AreaDescription = "";

void AreaMetadata::SetDescription(const std::string& description)
{
  s_AreaDescription = QString::fromStdString(description);
  if(s_AreaDescription.isEmpty())
  {
    s_AreaDescription = "Nothing particularly interesting.";
  }
  Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
  if(courtroom != nullptr)
  {
    courtroom->m_area_description = s_AreaDescription;
    courtroom->write_area_desc();
  }

}
