#include "emotion_selector.h"
#include "engine/param/actor/actor_loader.h"
#include "engine/param/actor_repository.h"

#include "modules/theme/thememanager.h"

#include "drtheme.h"
#include "engine/interface/courtroom_layout.h"

using namespace engine::actor::user;

EmotionSelector::EmotionSelector(QWidget *parent)
    : RPWidget{"emotes", parent}
{
  m_ContextMenu = new EmoteMenu(this);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, m_ContextMenu, &EmoteMenu::OnMenuRequested);
}

void EmotionSelector::emotionChange(ActorEmote emote)
{
  if(m_ContextMenu == nullptr) return;
  m_ContextMenu->EmoteChange(emote);
}

void EmotionSelector::actorChange(rolechat::actor::IActorData *actor)
{
  m_ActorEmotions.clear();
  if(retrieve() != nullptr) retrieve()->emotes();

  m_ContextMenu->ClearPresets();

  for(rolechat::actor::ActorScalingPreset presetData : actor->scalingPresets())
    m_ContextMenu->AddPreset(QString::fromStdString(presetData.name));

  if(m_ActorEmotions.count() > 0)
  {
    m_ContextMenu->EmoteChange(m_ActorEmotions[0]);
  }

}

void EmotionSelector::outfitChange()
{
  m_ActorEmotions.clear();
  m_ActorEmotions = QVector<ActorEmote>::fromStdVector(retrieve()->emotes());

  if(m_ActorEmotions.count() > 0)
  {
    m_ContextMenu->EmoteChange(m_ActorEmotions[0]);
  }
  else m_ContextMenu->clearLayers();
}

int EmotionSelector::calculateTrueIndex(int id)
{
  return (m_PageLimit * m_PageIndex) + id;
}

void EmotionSelector::constructEmotes()
{
  while (!m_EmotionButtons.isEmpty())
    delete m_EmotionButtons.takeLast();

  QPoint f_spacing = m_app->current_theme->get_widget_settings_spacing("emotes", "courtroom", "emote_button_spacing");

  float buttonSize = EmoteMenu::isDoubleSize() ? 82 : 40;

  int buttonResize = (int)(buttonSize * ThemeManager::get().getResize());
  const int button_width = buttonResize;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = buttonResize;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  m_EmoteColumns = ((width() - button_width) / (x_spacing + button_width)) + 1;
  m_EmoteRows = ((height() - button_height) / (y_spacing + button_height)) + 1;

  m_PageLimit = qMax(1, m_EmoteColumns * m_EmoteRows);
  for (int n = 0; n < m_PageLimit; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEmoteButton *f_emote = new AOEmoteButton(this, m_app, x_pos, y_pos);
    m_EmotionButtons.append(f_emote);

    f_emote->set_emote_number(n);

    Courtroom *courtroom = static_cast<Courtroom*>(ThemeManager::get().getWidget("courtroom"));
    connect(f_emote, SIGNAL(emote_clicked(int)), this, SLOT(emoteClicked(int)));
    if(courtroom != nullptr)
    {
      connect(f_emote, SIGNAL(tooltip_requested(int, QPoint)), courtroom, SLOT(show_emote_tooltip(int, QPoint)));
      connect(f_emote, SIGNAL(mouse_left(int)), courtroom, SLOT(hide_emote_tooltip(int)));
    }

    ++x_mod_count;

    if (x_mod_count == m_EmoteColumns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  refreshEmotes(true);
}

void EmotionSelector::refreshEmotes(bool scrollToCurrent)
{
  RPButton* l_emotesLeft = ThemeManager::get().GetButton("emote_left");
  RPButton* l_emotesRight = ThemeManager::get().GetButton("emote_right");

  l_emotesLeft->hide();
  l_emotesRight->hide();

  for (AOEmoteButton *i_button : qAsConst(m_EmotionButtons)) i_button->hide();


  const int l_emote_count = m_ActorEmotions.length();
  const int l_page_count = qFloor(l_emote_count / m_PageLimit) + bool(l_emote_count % m_PageLimit);

  if (scrollToCurrent)
    m_PageIndex = m_SelectedIndex / m_PageLimit;

  m_PageIndex = qBound(0, m_PageIndex, l_page_count - 1);

  const int l_current_page_emote_count = qBound(0, l_emote_count - m_PageIndex * m_PageLimit, m_PageLimit);

  if (m_PageIndex + 1 < l_page_count)
    l_emotesRight->show();

  if (m_PageIndex > 0)
    l_emotesLeft->show();

  for (int i = 0; i < l_current_page_emote_count; ++i)
  {
    const int l_real_i = i + m_PageIndex * m_PageLimit;
    AOEmoteButton *l_button = m_EmotionButtons.at(i);
    l_button->set_image(getEmote(l_real_i), l_real_i == m_SelectedIndex);
    l_button->show();
  }
}

void EmotionSelector::refreshSelection(bool changedActor)
{
  const int l_prev_emote_count = m_ActorEmotions.count();
  m_ActorEmotions = QVector<ActorEmote>::fromStdVector(retrieve()->emotes());

  QComboBox* l_emoteCombobox = dynamic_cast<QComboBox*>(ThemeManager::get().getWidget("emote_dropdown"));
  QCheckBox* l_preCheckbox = dynamic_cast<QCheckBox*>(ThemeManager::get().getWidget("pre"));

  if(l_emoteCombobox == nullptr)
    return;

  if(l_preCheckbox == nullptr)
    return;

  const QString l_prev_emote = l_emoteCombobox->currentText();

  QSignalBlocker l_blocker(l_emoteCombobox);
  l_emoteCombobox->clear();

  QStringList l_emote_list;
  for (const ActorEmote &i_emote : qAsConst(m_ActorEmotions))
    l_emote_list.append(QString::fromStdString(i_emote.comment));
  l_emoteCombobox->addItems(l_emote_list);

  if (changedActor || l_prev_emote_count != m_ActorEmotions.count())
  {
    m_SelectedIndex = 0;
    m_PageIndex = 0;
    l_preCheckbox->setChecked(l_preCheckbox->isChecked() || SceneManager::get().pConfigAO->always_pre_enabled());
  }
  else
  {
    l_emoteCombobox->setCurrentText(l_prev_emote);
  }
}

void EmotionSelector::resetPage()
{
  m_SelectedIndex = 0;
  m_PageIndex = 0;

  QComboBox* emoteCombobox = dynamic_cast<QComboBox*>(ThemeManager::get().getWidget("emote_dropdown"));

  if (emoteCombobox != nullptr)
  {
    if (emoteCombobox->count())
      emoteCombobox->setCurrentIndex(m_SelectedIndex);
  }

  refreshEmotes(true);
}

ActorEmote EmotionSelector::getEmote(int emoteId)
{
  if (emoteId < 0 || emoteId >= m_ActorEmotions.length())
    return ActorEmote();
  return m_ActorEmotions.at(emoteId);
}

ActorEmote EmotionSelector::getSelectedEmote()
{
  if (m_SelectedIndex < 0 || m_SelectedIndex >= m_ActorEmotions.length())
    return ActorEmote();
  return m_ActorEmotions.at(m_SelectedIndex);
}

void EmotionSelector::switchPageNext()
{
  ++m_PageIndex;
  refreshEmotes(false);
}

void EmotionSelector::switchPagePrevious()
{
  --m_PageIndex;
  refreshEmotes(false);
}

void EmotionSelector::selectEmote(int emoteTarget)
{
  const int l_min = m_PageIndex * m_PageLimit;
  const int l_max = (m_PageLimit - 1) + m_PageIndex * m_PageLimit;

  const ActorEmote &l_prev_emote = getSelectedEmote();
  if (m_SelectedIndex >= l_min && m_SelectedIndex <= l_max)
  {
    AOEmoteButton *l_prev_button = m_EmotionButtons.at(m_SelectedIndex % m_PageLimit);
    l_prev_button->set_image(l_prev_emote, false);
    l_prev_button->repaint();
  }

  const int l_prev_emote_id = m_SelectedIndex;
  m_SelectedIndex = calculateTrueIndex(emoteTarget);
  const ActorEmote &l_emote = getSelectedEmote();

  if (m_SelectedIndex >= l_min && m_SelectedIndex <= l_max)
  {
    AOEmoteButton *l_new_button = m_EmotionButtons.at(m_SelectedIndex % m_PageLimit);
    l_new_button->set_image(l_emote, true);
    l_new_button->repaint();
  }

  const int emote_mod = l_emote.modifier;

  QCheckBox* l_preCheckbox = dynamic_cast<QCheckBox*>(ThemeManager::get().getWidget("pre"));
  if (l_prev_emote_id == m_SelectedIndex) // toggle
    l_preCheckbox->setChecked(!l_preCheckbox->isChecked());
  else
    l_preCheckbox->setChecked(emote_mod == 1 || SceneManager::get().pConfigAO->always_pre_enabled());


}

void EmotionSelector::dropdownChanged(int id)
{
  m_PageIndex = 0;
  selectEmote(id);

  QComboBox* emoteCombobox = dynamic_cast<QComboBox*>(ThemeManager::get().getWidget("emote_dropdown"));
  RPListWidget* sfxList = dynamic_cast<RPListWidget*>(ThemeManager::get().getWidget("sfx_list"));
  RPListWidget* animList = dynamic_cast<RPListWidget*>(ThemeManager::get().getWidget("chara_animations"));

  if (sfxList != nullptr)
    sfxList->selectDefault();

  if (animList != nullptr)
    animList->selectText(QString::fromStdString(getSelectedEmote().sequence));

  emotionChange(getSelectedEmote());

  if(emoteCombobox != nullptr) emoteCombobox->setCurrentIndex(getSelectedIndex());
  courtroom::ic::focusMessageBox();
  refreshEmotes(true);
}

void EmotionSelector::emoteClicked(int id)
{
  selectEmote(id);

  QComboBox* emoteCombobox = dynamic_cast<QComboBox*>(ThemeManager::get().getWidget("emote_dropdown"));
  RPListWidget* sfxList = dynamic_cast<RPListWidget*>(ThemeManager::get().getWidget("sfx_list"));
  RPListWidget* animList = dynamic_cast<RPListWidget*>(ThemeManager::get().getWidget("chara_animations"));

  if (sfxList != nullptr)
    sfxList->selectDefault();

  if (animList != nullptr)
    animList->selectText(QString::fromStdString(getSelectedEmote().sequence));


  emotionChange(getSelectedEmote());

  if(emoteCombobox != nullptr) emoteCombobox->setCurrentIndex(getSelectedIndex());
  courtroom::ic::focusMessageBox();
}

void EmotionSelector::showEmoteTooltip(int id, QPoint pos)
{

}

void EmotionSelector::hideEmoteTooltip(int id)
{

}

void EmotionSelector::wheelEvent(QWheelEvent *event)
{
  int delta = event->angleDelta().y();

  if (delta > 0) { switchPagePrevious();}
  else if (delta < 0) { switchPageNext();}

  QWidget::wheelEvent(event);

}
