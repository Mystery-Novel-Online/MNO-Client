#include "datatypes.h"

#include <QMap>

QMap<DR::Color, DR::ColorInfo> DR::get_default_color_map()
{
  QMap<Color, ColorInfo> default_color_map;

  default_color_map[DR::CWhite] = ColorInfo("White", "#d5d5d5");
  default_color_map[DR::CGreen] = ColorInfo("Green", "#65c856");
  default_color_map[DR::CRed] = ColorInfo("Red", "#ba1518");
  default_color_map[DR::COrange] = ColorInfo("Orange", "#d55900");
  default_color_map[DR::CBlue] = ColorInfo("Blue", "#1588c8");
  default_color_map[DR::CYellow] = ColorInfo("Yellow", "#e7ce4e");
  default_color_map[DR::CPurple] = ColorInfo("Purple", "#f776fd");
  default_color_map[DR::CPink] = ColorInfo("Pink", "#da7c80");

  return default_color_map;
}

QString DRServerInfo::to_info() const
{
  QString r_info = "Unnamed Server";

  if (!name.isEmpty())
  {
    r_info = name;
  }
  else if (!address.isEmpty())
  {
    r_info = to_address();
  }

  return r_info;
}

QString DRServerInfo::to_address() const
{
  return address + ":" + QString::number(port);
}

bool DRServerInfo::operator==(const DRServerInfo &o) const
{
  return address == o.address && port == o.port;
}

bool DRServerInfo::operator!=(const DRServerInfo &other) const
{
  return !operator==(other);
}

VersionNumber::VersionNumber()
{}

VersionNumber::VersionNumber(int p_release, int p_major, int p_minor)
    : release(p_release)
    , major(p_major)
    , minor(p_minor)
{}

QString VersionNumber::to_string() const
{
  return QString("%1.%2.%3").arg(release).arg(major).arg(minor);
}

bool VersionNumber::operator==(const VersionNumber &other) const
{
  return release == other.release && major == other.major;
}

bool VersionNumber::operator<(const VersionNumber &other) const
{
  return release < other.release || (release == other.release && major < other.major);
}

bool VersionNumber::operator>(const VersionNumber &other) const
{
  return release > other.release || (release == other.release && major > other.major);
}

const QMap<SpriteCategory, QString> s_sprite_category_string_map{
    {SpriteGUI, "gui"},
    {SpriteStage, "background"},
    {SpriteCharacter, "character"},
    {SpriteEffect, "effect"},
    {SpriteShout, "shout"},
    {SpriteSticker, "sticker"},
};

SpriteCategory string_to_sprite_category(QString p_category)
{
  return s_sprite_category_string_map.key(p_category, SpriteGUI);
}

QString sprite_category_to_string(SpriteCategory p_category)
{
  return s_sprite_category_string_map.value(p_category, "gui");
}

QStringList sprite_category_string_list()
{
  return s_sprite_category_string_map.values();
}

SpriteCategory viewport_sprite_to_sprite_category(ViewportSprite p_type)
{
  return QMap<ViewportSprite, SpriteCategory>{
      {ViewportStageBack, SpriteStage},
      {ViewportStageFront, SpriteStage},
      {ViewportCharacterPre, SpriteCharacter},
      {ViewportCharacterIdle, SpriteCharacter},
      {ViewportCharacterTalk, SpriteCharacter},
      {ViewportPairCharacterIdle, SpritePairCharacter},
      {ViewportEffect, SpriteEffect},
      {ViewportShout, SpriteShout},
  }
      .value(p_type, SpriteGUI);
}
