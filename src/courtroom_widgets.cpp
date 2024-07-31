#include "courtroom.h"

#include "aoapplication.h"
#include "aoblipplayer.h"
#include "aobutton.h"
#include "aoconfig.h"
#include "aoimagedisplay.h"
#include "aolabel.h"
#include "aolineedit.h"
#include "modules/managers/pair_manager.h"
#include "aomusicplayer.h"
#include "aonotearea.h"
#include "aonotepicker.h"
#include "aosfxplayer.h"
#include "aoshoutplayer.h"
#include "aosystemplayer.h"
#include "aotimer.h"
#include "commondefs.h"
#include "drcharactermovie.h"
#include "drchatlog.h"
#include "drtheme.h"
#include "dreffectmovie.h"
#include "drscenemovie.h"
#include "drshoutmovie.h"
#include "drsplashmovie.h"
#include "drstickerviewer.h"
#include "drtextedit.h"
#include "file_functions.h"
#include "mk2/graphicsvideoscreen.h"
#include "modules/managers/notify_manager.h"
#include "theme.h"

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QGraphicsBlurEffect>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMenu>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>

#include <modules/theme/thememanager.h>

#include <modules/theme/widgets/dro_combo_box.h>
#include <modules/theme/widgets/dro_line_edit.h>
#include "modules/debug/time_debugger.h"

#include <modules/managers/evidence_manager.h>
#include <modules/managers/localization_manager.h>
#include <modules/managers/variable_manager.h>

void Courtroom::create_widgets()
{
  TimeDebugger::get().StartTimer("Theme Widgets");
  m_keepalive_timer = new QTimer(this);
  m_keepalive_timer->start(60000);

  m_tick_timer = new QTimer(this);
  m_tick_timer->setSingleShot(true);
  m_tick_timer->setTimerType(Qt::PreciseTimer);

  m_sound_timer = new QTimer(this);
  m_sound_timer->setSingleShot(true);

  m_flash_timer = new QTimer(this);
  m_flash_timer->setSingleShot(true);

  char_button_mapper = new QSignalMapper(this);

  m_system_player = new AOSystemPlayer(ao_app, this);
  m_effects_player = new AOSfxPlayer(ao_app, this);
  m_shouts_player = new AOShoutPlayer(ao_app, this);
  m_blips_player = new AOBlipPlayer(ao_app, this);

  ui_background = new AOImageDisplay(this, ao_app);

  ThemeManager::get().RegisterCourtroomBackground(ui_background);


#ifdef QT_DEBUG
  //m_ViewportVerTwo = new DROViewportWidget(nullptr);
  //ThemeManager::get().AutoAdjustWidgetDimensions(m_ViewportVerTwo, "viewport", SceneTypeReplays);
  //m_ViewportVerTwo->ConstructViewport(SceneTypeReplays);
  //m_ViewportVerTwo->show();
#endif

  ui_viewport = new DRGraphicsView(this);
  wShoutsLayer = new KeyframePlayer(this);

  SceneManager::get().CreateTransition(this, ao_app, ui_viewport);

  { // populate scene
    auto *l_scene = ui_viewport->scene();

    ui_vp_background = new DRSceneMovie(ao_app);

    background_anim = new QPropertyAnimation(ui_vp_background, "pos", this);
    l_scene->addItem(ui_vp_background);

    ui_vp_player_pair = new DRCharacterMovie(ao_app);
    l_scene->addItem(ui_vp_player_pair);

    ui_vp_player_char = new DRCharacterMovie(ao_app);
    player_sprite_anim = new QPropertyAnimation(ui_vp_player_char, "pos", this);
    aniPlayerChar = new GraphicObjectAnimator(ui_vp_player_char, 60);
    l_scene->addItem(ui_vp_player_char);


    ui_vp_desk = new DRSceneMovie(ao_app);
    l_scene->addItem(ui_vp_desk);

    ui_vp_effect = new DREffectMovie(ao_app);
    l_scene->addItem(ui_vp_effect);

    vpWeatherLayer = new DRSceneMovie(ao_app);
    l_scene->addItem(vpWeatherLayer);

    ui_vp_wtce = new DRSplashMovie(ao_app);
    l_scene->addItem(ui_vp_wtce);

    ui_vp_objection = new DRShoutMovie(ao_app);
    l_scene->addItem(ui_vp_objection);

    ui_video = new DRVideoScreen(ao_app);
    l_scene->addItem(ui_video);

    // move to the corner
    for (auto i_item : l_scene->items())
    {
      i_item->setPos(0, 0);
    }
  }

  w_ViewportOverlay = new ViewportOverlay(ui_viewport);

  wEvidencePreviewImage = new AOImageDisplay(this, ao_app);

  ui_vp_music_display_a = new AOImageDisplay(this, ao_app);
  ui_vp_music_display_b = new AOImageDisplay(this, ao_app);
  ui_vp_music_area = new QWidget(ui_vp_music_display_a);
  ui_vp_music_name = new DRTextEdit(ui_vp_music_area);
  ui_vp_music_name->setFrameStyle(QFrame::NoFrame);
  ui_vp_music_name->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setReadOnly(true);
  music_anim = new QPropertyAnimation(ui_vp_music_name, "geometry", this);
  set_music_text("DANGANRONPA ONLINE");

  ui_vp_clock = new DRStickerViewer(ao_app, this);

  ui_vp_chatbox = new DRStickerViewer(ao_app, this);
  chatbox_anim = new QPropertyAnimation(ui_vp_chatbox, "pos", this);
  ui_vp_showname = new DRTextEdit(ui_vp_chatbox);
  ui_vp_showname->setFrameStyle(QFrame::NoFrame);
  ui_vp_showname->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_showname->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_showname->setReadOnly(true);
  ui_vp_message = new DRTextEdit(ui_vp_chatbox);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);

  ui_vp_showname_image = new AOImageDisplay(this, ao_app);

  ui_vp_chat_arrow = new DRStickerViewer(ao_app, this);
  ui_vp_loading = new DRStickerViewer(ao_app, this);
  ui_vp_loading->hide();

  m_loading_timer = new QTimer(this);
  m_loading_timer->setSingleShot(true);
  m_loading_timer->setInterval(ao_config->loading_bar_delay());

  p_DropdownMusicCategory = new QComboBox(this);

  ui_iniswap_dropdown = new QComboBox(this);
  ui_iniswap_dropdown->setInsertPolicy(QComboBox::NoInsert);
  {
    QListView *l_view = new QListView(ui_iniswap_dropdown);
    ui_iniswap_dropdown->setView(l_view);
    l_view->setTextElideMode(Qt::TextElideMode::ElideNone);
    l_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  }

  ui_ic_chatlog = new DRTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);
  ui_ic_chatlog->set_auto_align(false);
  ui_ic_chatlog_scroll_topdown = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "ic_chatlog_scroll_topdown", "ic_chatlog_scroll_topdown.png", "", this);
  ui_ic_chatlog_scroll_bottomup = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "ic_chatlog_scroll_bottomup", "ic_chatlog_scroll_bottomup.png", "", this);

  ui_area_desc = new DRTextEdit(this);
  ui_area_desc->setReadOnly(true);
  ui_area_desc->set_auto_align(false);


  wEvidenceDescription = new DRTextEdit(this);
  wEvidenceDescription->setFrameStyle(QFrame::NoFrame);
  wEvidenceDescription->setReadOnly(true);
  wEvidenceDescription->set_auto_align(false);

  EvidenceManager::get().SetEvidenceDescription("");

  ui_ooc_chatlog = new DRChatLog(this);
  ui_ooc_chatlog->setReadOnly(true);
  ui_ooc_chatlog->setOpenExternalLinks(true);

  ui_area_list = new QListWidget(this);
  ui_area_search = new QLineEdit(this);
  ui_area_search->setFrame(false);
  ui_area_search->setPlaceholderText(LocalizationManager::get().getLocalizationText("TEXTBOX_AREA"));

  ui_music_list = new QListWidget(this);
  ui_music_list->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_music_search = setupLineEditWidget("music_search", LocalizationManager::get().getLocalizationText("TEXTBOX_MUSIC"), "[MUSIC SEARCH]", "");
  ui_music_search->setFrame(false);
  ui_music_menu = new QMenu(this);
  ui_music_menu_play = ui_music_menu->addAction(tr("Play"));
  ui_music_menu_insert_ooc = ui_music_menu->addAction(tr("Insert to OOC"));
  p_ActionPinMusic = ui_music_menu->addAction(tr("Add song to pinned."));


  wCharaAnimList = new QListWidget(this);

  ui_sfx_list = new QListWidget(this);
  ui_sfx_list->setContextMenuPolicy(Qt::CustomContextMenu);

  ui_sfx_search = setupLineEditWidget("sfx_search", "", "[SFX SEARCH]", "");
  ui_sfx_search->setFrame(false);

  ui_sfx_menu = new QMenu(this);
  ui_sfx_menu_preview = ui_sfx_menu->addAction(LocalizationManager::get().getLocalizationText("SFX_PREVIEW"));
  ui_sfx_menu_insert_file_name = ui_sfx_menu->addAction(LocalizationManager::get().getLocalizationText("SFX_FILENAME"));
  ui_sfx_menu_insert_caption = ui_sfx_menu->addAction(LocalizationManager::get().getLocalizationText("SFX_CAPTION"));

  ui_ic_chat_message = new QWidget(this);

  ui_ic_chat_message_field = new QLineEdit(ui_ic_chat_message);
  ui_ic_chat_message_field->setFrame(false);
  ui_ic_chat_message_field->setPlaceholderText(LocalizationManager::get().getLocalizationText("CHATBOX_IC"));
  ui_ic_chat_message_field->setMaxLength(255);

  ui_ooc_chat_message = setupLineEditWidget("ooc_chat_message", LocalizationManager::get().getLocalizationText("CHATBOX_OOC"), "[OOC LINE]", "");
  ui_ooc_chat_message->setFrame(false);
  ui_ooc_chat_message->setMaxLength(1023);

  ui_ic_chat_message_counter = new QLabel(ui_ic_chat_message);
  ui_ic_chat_message_counter->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  ui_ic_chat_message_counter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  {
    auto l_layout = new QHBoxLayout(ui_ic_chat_message);
    ui_ic_chat_message_counter->setIndent(l_layout->contentsMargins().right());
    l_layout->setContentsMargins(0, 0, 0, 0);
    l_layout->addWidget(ui_ic_chat_message_field);
    l_layout->addWidget(ui_ic_chat_message_counter);
  }

  ui_ic_chat_message_counter->hide();

  ui_ooc_chat_name = setupLineEditWidget("ooc_chat_name", LocalizationManager::get().getLocalizationText("TEXTBOX_OOC_NAME") , "[OOC NAME LINE]", ao_config->username());
  ui_ooc_chat_name->setFrame(false);

  ui_ic_chat_showname = setupLineEditWidget("ic_chat_name", LocalizationManager::get().getLocalizationText("TEXTBOX_SHOWNAME"), "[IC NAME LINE]", ao_config->showname());
  ui_ic_chat_showname->setFrame(false);

  ui_note_area = new AONoteArea(this, ao_app);
  ui_note_area->add_button = new AOButton(ui_note_area, ao_app);
  ui_note_area->m_layout = new QVBoxLayout(ui_note_area);


  pUIPairOffsetSlider = new QSlider(Qt::Horizontal, this);
  pUIPairOffsetSlider->setMinimum(0);
  pUIPairOffsetSlider->setMaximum(960);

  PairManager::get().SetSlider(pUIPairOffsetSlider);

  pNotifyPopup = new RPNotifyMenu(this);

  NotifyManager::get().ThemeSetupPopup(pNotifyPopup);

  ui_note_scroll_area = new QScrollArea(this);
  ui_note_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_note_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_note_scroll_area->setWidgetResizable(true);

  ui_set_notes = new AOButton(this, ao_app);

  construct_emotes();

  ui_defense_bar = new AOImageDisplay(this, ao_app);
  ui_prosecution_bar = new AOImageDisplay(this, ao_app);

  ui_shout_up = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "shout_up", "shoutup.png", "", this);
  ui_shout_up->setProperty("cycle_id", 1);
  ui_shout_down = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "shout_down", "shoutdown.png", "", this);
  ui_shout_down->setProperty("cycle_id", 0);

  ui_effect_down = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "effect_down", "effectdown.png", "", this);
  ui_effect_down->setProperty("cycle_id", 2);
  ui_effect_up = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "effect_up", "effectup.png", "", this);
  ui_effect_up->setProperty("cycle_id", 3);

  ui_wtce_up = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "wtce_up", "wtceup.png", "", this);
  ui_wtce_up->setProperty("cycle_id", 5);
  ui_wtce_down = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "wtce_down", "wtcedown.png", "", this);
  ui_wtce_down->setProperty("cycle_id", 4);

  ui_change_character = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "change_character", "changecharacter.png", "Change Character", this);
  ui_call_mod = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "call_mod", "callmod.png", LocalizationManager::get().getLocalizationText("PING_MODS"), this);
  ui_switch_area_music = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "switch_area_music", "switch_area_music.png", "A/M", this);

  ui_config_panel = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "config_panel", "config_panel.png", "Config", this);
  ui_note_button = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "note_button", "notebutton.png", "Notes", this);


  ui_label_images.resize(label_images.size());
  for (int i = 0; i < ui_label_images.size(); ++i)
  {
    ui_label_images[i] = new AOImageDisplay(this, ao_app);
  }

  ui_pre = new QCheckBox(this);
  ui_pre->setText(LocalizationManager::get().getLocalizationText("TITLE_PRE"));
  ui_pre->setToolTip(LocalizationManager::get().getLocalizationText("TOOLTIP_PRE"));

  ui_flip = new QCheckBox(this);
  ui_flip->setText(LocalizationManager::get().getLocalizationText("TITLE_FLIP"));
  ui_flip->setToolTip(LocalizationManager::get().getLocalizationText("TOOLTIP_FLIP"));
  ui_flip->hide();

  ui_hide_character = new QCheckBox(this);
  ui_hide_character->setText(LocalizationManager::get().getLocalizationText("TITLE_HIDE"));
  ui_hide_character->setToolTip(LocalizationManager::get().getLocalizationText("TOOLTIP_HIDE"));

  // filling vectors with existing label/checkbox pointers
  ui_checks.push_back(ui_pre);
  ui_checks.push_back(ui_flip);
  ui_checks.push_back(ui_hide_character);

  ui_defense_plus = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "defense_plus", "defplus.png", "", this);
  ui_defense_minus = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "defense_minus", "defminus.png", "", this);

  ui_prosecution_plus = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "prosecution_plus", "proplus.png", "", this);
  ui_prosecution_minus = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "prosecution_minus", "prominus.png", "", this);

  //Setup Combo Boxes
  QStringList l_colorNames =
  {
      LocalizationManager::get().getLocalizationText("COLOR_WHITE"),
      LocalizationManager::get().getLocalizationText("COLOR_GREEN"),
      LocalizationManager::get().getLocalizationText("COLOR_RED"),
      LocalizationManager::get().getLocalizationText("COLOR_ORANGE"),
      LocalizationManager::get().getLocalizationText("COLOR_BLUE"),
      LocalizationManager::get().getLocalizationText("COLOR_YELLOW"),
      LocalizationManager::get().getLocalizationText("COLOR_PURPLE"),
      LocalizationManager::get().getLocalizationText("COLOR_PINK")
  };

  ui_text_color = setupComboBoxWidget(l_colorNames, "text_color", "[TEXT COLOR]");

  QStringList l_chatTypes =
  {
      LocalizationManager::get().getLocalizationText("CHAT_TALK"),
      LocalizationManager::get().getLocalizationText("CHAT_SHOUT"),
      LocalizationManager::get().getLocalizationText("CHAT_THINK"),
      LocalizationManager::get().getLocalizationText("CHAT_CG"),
  };

  ui_chat_type_dropdown = setupComboBoxWidget(l_chatTypes, "chat_type", "[CHAT TYPE]");
  wOutfitDropdown = setupComboBoxWidget({}, "outfit_selector", "[OUTFIT SELECTION]");


  ui_vp_notepad_image = new AOImageDisplay(this, ao_app);
  ui_vp_notepad = new DRTextEdit(this);
  ui_vp_notepad->setFrameStyle(QFrame::NoFrame);

  ui_timers.resize(1);
  ui_timers[0] = new AOTimer(this);


  wEvidenceLeft = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "evidence_left", "arrow_left.png", "<-", this);
  wEvidenceRight = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "evidence_right", "arrow_right.png", "<-", this);
  wEvidencePresent = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "evidence_present", "evidence_present.png", "Present", this);


  ui_player_list_left = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "player_list_left", "arrow_left.png", "<-", this);
  ui_player_list_right = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "player_list_right", "arrow_right.png", "->", this);
  ui_area_look = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "area_look", "area_look.png", LocalizationManager::get().getLocalizationText("TITLE_LOOK"), this);

  p_ButtonScreenshot = ThemeManager::get().CreateWidgetButton(SceneTypeCourtroom, "screenshot", "screenshot.png", "Screenshot", this);
  constructEvidenceList();
  construct_playerlist();

  construct_char_select();

  TimeDebugger::get().EndTimer("Theme Widgets");
}

QComboBox *Courtroom::setupComboBoxWidget(const QStringList& items, QString name, QString cssHeader)
{
  DROComboBox *comboBox = new DROComboBox(this, ao_app);
  comboBox->addItems(items);
  comboBox->setWidgetInfo(name, cssHeader, "courtroom");
  ThemeManager::get().RegisterWidgetComboBox(name, comboBox);
  return comboBox;
}


QLineEdit *Courtroom::setupLineEditWidget(const QString name, QString placeholder, QString legacy_css, QString text,  QWidget *parent)
{
  DROLineEdit *lineEdit;

  if(parent == nullptr) lineEdit = new DROLineEdit(this, ao_app);
  else lineEdit = new DROLineEdit(parent, ao_app);

  lineEdit->set_theme_settings(name, legacy_css, "courtroom");

  if(mDefaultWidgetCSS.contains(name)) lineEdit->setDefaultCSS(mDefaultWidgetCSS[name]);

  lineEdit->setPlaceholderText(placeholder);
  lineEdit->setText(text);

  ThemeManager::get().RegisterWidgetLineEdit(name, lineEdit);

  return lineEdit;
}

void Courtroom::connect_widgets()
{
  connect(m_keepalive_timer, &QTimer::timeout, this, &Courtroom::ping_server);

  connect(ui_video, SIGNAL(finished()), this, SLOT(video_finished()));
  connect(wShoutsLayer, SIGNAL(animationFinished()), this, SLOT(objection_done()));
  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));
  connect(ui_vp_player_char, SIGNAL(done()), this, SLOT(preanim_done()));

  connect(m_sound_timer, SIGNAL(timeout()), this, SLOT(play_sfx()));

  connect(m_tick_timer, SIGNAL(timeout()), this, SLOT(next_chat_letter()));

  connect(m_flash_timer, SIGNAL(timeout()), this, SLOT(realization_done()));

  connect(ao_config, SIGNAL(searchable_iniswap_changed(bool)), this, SLOT(update_iniswap_dropdown_searchable()));
  connect(ao_config, SIGNAL(emote_preview_changed(bool)), this, SLOT(on_emote_preview_toggled(bool)));
  connect(ui_emote_left, SIGNAL(clicked()), this, SLOT(on_emote_left_clicked()));
  connect(ui_emote_right, SIGNAL(clicked()), this, SLOT(on_emote_right_clicked()));

  connect(ui_emote_dropdown, SIGNAL(activated(int)), this, SLOT(on_emote_dropdown_changed(int)));
  connect(ui_iniswap_dropdown, SIGNAL(activated(int)), this, SLOT(on_iniswap_dropdown_changed(int)));

  connect(p_DropdownMusicCategory, SIGNAL(activated(int)), this, SLOT(OnMusicCategoryChanged()));
  connect(p_DropdownPosition, SIGNAL(activated(int)), this, SLOT(on_pos_dropdown_changed()));


  connect(pCharaSelectSeries, SIGNAL(activated(int)), this, SLOT(onCharacterSelectPackageChanged(int)));

  connect(ao_config, SIGNAL(showname_changed(QString)), this, SLOT(on_showname_changed(QString)));
  connect(ao_config, SIGNAL(showname_placeholder_changed(QString)), this, SLOT(on_showname_placeholder_changed(QString)));
  connect(ao_config, SIGNAL(character_ini_changed(QString)), this, SLOT(on_character_ini_changed()));
  connect(ui_ic_chat_showname, SIGNAL(editingFinished()), this, SLOT(on_ic_showname_editing_finished()));
  connect(ui_ic_chat_message_field, SIGNAL(returnPressed()), this, SLOT(on_ic_message_return_pressed()));
  connect(ao_config, SIGNAL(message_length_threshold_changed(int)), this, SLOT(handle_ic_message_length()));
  connect(ui_ic_chat_message_field, SIGNAL(textChanged(QString)), this, SLOT(handle_ic_message_length()));
  connect(ui_ic_chatlog->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_ic_chatlog_scroll_changed()));
  connect(ui_ic_chatlog_scroll_topdown, SIGNAL(clicked()), this, SLOT(on_ic_chatlog_scroll_topdown_clicked()));
  connect(ui_ic_chatlog_scroll_bottomup, SIGNAL(clicked()), this, SLOT(on_ic_chatlog_scroll_bottomup_clicked()));
  connect(ao_config, SIGNAL(username_changed(QString)), ui_ooc_chat_name, SLOT(setText(QString)));
  connect(ui_ooc_chat_name, SIGNAL(editingFinished()), this, SLOT(on_ooc_name_editing_finished()));
  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this, SLOT(on_ooc_message_return_pressed()));

  connect(ui_music_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_music_list_clicked()));
  connect(ui_music_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_music_list_double_clicked(QModelIndex)));
  connect(ui_music_list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_music_list_context_menu_requested(QPoint)));

  connect(ui_music_menu_play, SIGNAL(triggered()), this, SLOT(on_music_menu_play_triggered()));

  connect(p_ActionPinMusic, SIGNAL(triggered()), this, SLOT(OnMusicMenuPinSongTriggered()));

  connect(ui_music_menu_insert_ooc, SIGNAL(triggered()), this, SLOT(on_music_menu_insert_ooc_triggered()));

  connect(ui_area_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_area_list_clicked()));
  connect(ui_area_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_area_list_double_clicked(QModelIndex)));

  // connect events for shout/effect/wtce buttons happen in load_shouts(),
  // load_effects(), load_wtce()
  connect(ui_shout_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_shout_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_effect_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_effect_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_wtce_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_wtce_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_defense_minus, SIGNAL(clicked()), this, SLOT(on_defense_minus_clicked()));
  connect(ui_defense_plus, SIGNAL(clicked()), this, SLOT(on_defense_plus_clicked()));
  connect(ui_prosecution_minus, SIGNAL(clicked()), this, SLOT(on_prosecution_minus_clicked()));
  connect(ui_prosecution_plus, SIGNAL(clicked()), this, SLOT(on_prosecution_plus_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this, SLOT(on_text_color_changed(int)));


  connect(ui_chat_type_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(on_chat_type_changed(int)));
  connect(wOutfitDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutfitChanged(int)));

  connect(this, SIGNAL(loaded_theme()), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_max_lines_changed(int)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_display_timestamp_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_display_client_id_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_display_self_highlight_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_format_use_newline_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_display_empty_messages_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_display_music_switch_changed(bool)), this, SLOT(on_chat_config_changed()));
  connect(ao_config, SIGNAL(log_is_topdown_changed(bool)), this, SLOT(on_chat_config_changed()));

  connect(ui_area_search, SIGNAL(textChanged(QString)), this, SLOT(on_area_search_edited(QString)));
  connect(ui_music_search, SIGNAL(textChanged(QString)), this, SLOT(on_music_search_edited(QString)));
  connect(ui_sfx_search, SIGNAL(textChanged(QString)), this, SLOT(filter_sfx_list(QString)));

  connect(ui_change_character, SIGNAL(clicked()), this, SLOT(on_change_character_clicked()));
  connect(pCharaSelectSearch, SIGNAL(textChanged(QString)), this, SLOT(CharacterSearchUpdated()));


  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));



  connect(ui_switch_area_music, SIGNAL(clicked()), this, SLOT(on_switch_area_music_clicked()));

  connect(ui_config_panel, SIGNAL(clicked()), this, SLOT(on_config_panel_clicked()));
  connect(ui_note_button, SIGNAL(clicked()), this, SLOT(on_note_button_clicked()));

  connect(ui_vp_notepad, SIGNAL(textChanged()), this, SLOT(on_note_text_changed()));

  connect(ui_pre, SIGNAL(clicked()), this, SLOT(on_pre_clicked()));
  connect(ui_flip, SIGNAL(clicked()), this, SLOT(on_flip_clicked()));
  connect(ui_hide_character, SIGNAL(clicked()), this, SLOT(on_hidden_clicked()));

  connect(ui_sfx_list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_sfx_list_current_item_changed(QListWidgetItem *, QListWidgetItem *)));
  connect(ui_sfx_list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_sfx_list_context_menu_requested(QPoint)));

  connect(ui_sfx_menu_preview, SIGNAL(triggered()), this, SLOT(on_sfx_menu_preview_triggered()));
  connect(ui_sfx_menu_insert_file_name, SIGNAL(triggered()), this, SLOT(on_sfx_menu_insert_file_name_triggered()));
  connect(ui_sfx_menu_insert_caption, SIGNAL(triggered()), this, SLOT(on_sfx_menu_insert_caption_triggered()));
  connect(pUIPairOffsetSlider, SIGNAL(sliderReleased()), this, SLOT(on_pair_offset_changed()));



  connect(ui_note_area->add_button, SIGNAL(clicked(bool)), this, SLOT(on_add_button_clicked()));
  connect(ui_set_notes, SIGNAL(clicked(bool)), this, SLOT(on_set_notes_clicked()));

  // performance
  connect(ao_config, SIGNAL(sprite_caching_toggled(int, bool)), this, SLOT(assign_readers_for_viewers(int, bool)));
  connect(ao_config, SIGNAL(caching_threshold_changed(int)), m_preloader_sync, SLOT(set_threshold(int)));
  connect(m_preloader_sync, SIGNAL(finished()), this, SLOT(start_chatmessage()));
  connect(ao_config, SIGNAL(loading_bar_delay_changed(int)), this, SLOT(on_loading_bar_delay_changed(int)));
  connect(m_loading_timer, SIGNAL(timeout()), ui_vp_loading, SLOT(show()));


  //Evidence List
  connect(wEvidenceLeft, SIGNAL(clicked()), this, SLOT(onEvidenceLeftClicked()));
  connect(wEvidenceRight, SIGNAL(clicked()), this, SLOT(onEvidenceRightClicked()));
  connect(wEvidencePresent, SIGNAL(clicked()), this, SLOT(onEvidencePresentClicked()));

  //Player List
  connect(ui_player_list_left, SIGNAL(clicked()), this, SLOT(on_player_list_left_clicked()));
  connect(ui_player_list_right, SIGNAL(clicked()), this, SLOT(on_player_list_right_clicked()));
  connect(ui_area_look, SIGNAL(clicked()), this, SLOT(on_area_look_clicked()));

  connect(p_ButtonScreenshot, SIGNAL(clicked()), this, SLOT(onScreenshotClicked()));

  // for some reason this didn't work without the new syntax
  connect(ao_config, &AOConfig::shortcuts_changed, this, &Courtroom::bind_shortcuts);
}

void Courtroom::reset_widget_toggles()
{
    widget_toggles = { };

    for (const QString widgeToggle : widget_toggles)
    {
      if(widget_names.contains(widgeToggle)) widget_names[widgeToggle]->show();
    }


    if(ao_app->current_theme->m_jsonLoaded)
    {
      QString l_parent_name = "Chat";

      QStringList chat_tab = ao_app->current_theme->get_tab_widgets("Chat");
      QStringList area_tab = ao_app->current_theme->get_tab_widgets("Area");
      QStringList gm_tab = ao_app->current_theme->get_tab_widgets("GM");


      for (const QString chatTabWidget : chat_tab)
      {
        widget_toggles[chatTabWidget] = "Chat";
      }

      for (const QString areaTabWidget : area_tab)
      {
        widget_toggles[areaTabWidget] = "Area";
      }

      for (const QString gmTabWidget : gm_tab)
      {
        widget_toggles[gmTabWidget] = "GM";
      }

    }
    else
    {
      const QString l_ini_path = ao_app->find_theme_asset_path(COURTROOM_TOGGLES_INI);
      QFile l_toggle_ini(l_ini_path);


      if (!l_toggle_ini.open(QIODevice::ReadOnly))
        return;


      QTextStream in(&l_toggle_ini);

      QString l_parent_name = "Chat";


      while (!in.atEnd())
      {
        QString l_line = in.readLine().trimmed();

        if (l_line.isEmpty()) { continue; }

        if (l_line.startsWith("["))
        {
          l_parent_name = l_line.remove(0, 1).chopped(1).trimmed();
        }
        else
        {
          QStringList line_elements = l_line.split("=");

          if(line_elements.count() <= 2) widget_toggles[line_elements.at(0).trimmed()] = l_parent_name;

        }
      }
    }
}

void Courtroom::bind_shortcuts()
{
  if (p_ScreenshotShortcut != nullptr)
    p_ScreenshotShortcut->deleteLater();
  p_ScreenshotShortcut = new QShortcut(ao_config->screenshot_shortcut(), p_ButtonScreenshot);
  connect(p_ScreenshotShortcut, SIGNAL(activated()), this, SLOT(onScreenshotClicked()));

  if (p_LookShortcut != nullptr)
    p_LookShortcut->deleteLater();
  p_LookShortcut = new QShortcut(ao_config->look_shortcut(), ui_area_look);
  connect(p_LookShortcut, SIGNAL(activated()), this, SLOT(on_area_look_clicked()));
}

void Courtroom::reset_widget_names()
{
  // Assign names to the default widgets
  widget_names = {
      {"courtroom", this},
      {"viewport", ui_viewport},
      {"music_display_a", ui_vp_music_display_a},
      {"music_display_b", ui_vp_music_display_b},
      {"music_area", ui_vp_music_area},
      {"music_name", ui_vp_music_name},
      // music_anim
      {"clock", ui_vp_clock},
      {"ao2_chatbox", ui_vp_chatbox},
      {"showname", ui_vp_showname},
      {"message", ui_vp_message},
      {"showname_image", ui_vp_showname_image},
      {"chat_arrow", ui_vp_chat_arrow},
      {"loading", ui_vp_loading},
      {"ic_chatlog", ui_ic_chatlog},
      {"ic_chatlog_scroll_topdown", ui_ic_chatlog_scroll_topdown},
      {"ic_chatlog_scroll_bottomup", ui_ic_chatlog_scroll_bottomup},
      {"server_chatlog", ui_ooc_chatlog},
      {"area_list", ui_area_list},
      {"area_search", ui_area_search},
      {"music_list", ui_music_list},
      {"music_search", ui_music_search},
      {"sfx_list", ui_sfx_list},
      {"sfx_search", ui_sfx_search},
      {"ic_chat_name", ui_ic_chat_showname},
      {"ao2_ic_chat_message", ui_ic_chat_message},
      {"notify_popup", pNotifyPopup},
      // ui_muted
      {"ooc_chat_message", ui_ooc_chat_message},
      {"ooc_chat_name", ui_ooc_chat_name},
      {"note_scroll_area", ui_note_scroll_area},
      {"note_area", ui_note_area},
      // add_button
      // m_layout
      {"set_notes_button", ui_set_notes},
      {"emotes", ui_emotes},
      {"emote_left", ui_emote_left},
      {"emote_right", ui_emote_right},
      {"emote_dropdown", ui_emote_dropdown},
      {"iniswap_dropdown", ui_iniswap_dropdown},
      {"pos_dropdown", p_DropdownPosition},
      {"category_dropdown", p_DropdownMusicCategory},
      {"defense_bar", ui_defense_bar},
      {"prosecution_bar", ui_prosecution_bar},
      // Each ui_shouts[i]
      {"shout_up", ui_shout_up},
      {"shout_down", ui_shout_down},
      // Each ui_effects[i]
      {"effect_down", ui_effect_down},
      {"effect_up", ui_effect_up},
      // Each ui_wtce[i]
      {"wtce_up", ui_wtce_up},
      {"wtce_down", ui_wtce_down},
      {"change_character", ui_change_character},
      {"call_mod", ui_call_mod},
      {"switch_area_music", ui_switch_area_music},
      {"config_panel", ui_config_panel},
      {"note_button", ui_note_button},
      // Each ui_label_images[i]
      {"pre", ui_pre},
      {"flip", ui_flip},
      {"hidden", ui_hide_character},
      {"defense_plus", ui_defense_plus},
      {"defense_minus", ui_defense_minus},
      {"prosecution_plus", ui_prosecution_plus},
      {"prosecution_minus", ui_prosecution_minus},
      {"text_color", ui_text_color},
      {"chat_type", ui_chat_type_dropdown},
      {"notepad_image", ui_vp_notepad_image},
      {"notepad", ui_vp_notepad},
      // Each ui_timers[i]
      {"char_select", ui_char_select_background},
      {"back_to_lobby", ui_back_to_lobby},
      {"char_buttons", ui_char_buttons},
      {"char_select_left", ui_chr_select_left},
      {"char_select_right", ui_chr_select_right},
      {"character_search", pCharaSelectSearch},
      {"character_packages", pCharaSelectSeries},
      {"spectator", ui_spectator},
      {"char_select_refresh", pBtnCharSelectRefresh},
      {"char_select_random", pBtnCharSelectRandom},
      {"player_list", ui_player_list},
      {"player_list_left", ui_player_list_left},
      {"player_list_right", ui_player_list_right},
      {"area_look", ui_area_look},
      {"area_desc", ui_area_desc},
      {"pair_offset", pUIPairOffsetSlider},
      {"viewport_transition", SceneManager::get().GetTransition()},
      {"viewport_overlay", w_ViewportOverlay},
      {"outfit_selector", wOutfitDropdown},
      {"evidence_image", wEvidencePreviewImage},
      {"evidence_description", wEvidenceDescription},
      {"evidence_list", wEvidenceList},
      {"evidence_left", wEvidenceLeft},
      {"evidence_right", wEvidenceRight},
      {"evidence_present", wEvidencePresent},
      {"chara_animations", wCharaAnimList},
      {"shouts_player", wShoutsLayer},
      {"screenshot", p_ButtonScreenshot}
  };

    ThemeManager::get().RegisterWidgetGenericBulk(widget_names);
}

void Courtroom::insert_widget_name(QString p_widget_name, QWidget *p_widget)
{
  if (widget_names.contains(p_widget_name))
    qWarning() << QString("[WARNING] Widget <%1> is already defined").arg(p_widget_name);
  widget_names.insert(p_widget_name, p_widget);
  p_widget->setObjectName(p_widget_name);
}

void Courtroom::insert_widget_names(QVector<QString> &p_name_list, QVector<QWidget *> &p_widget_list)
{
  if (p_name_list.length() != p_widget_list.length())
    qFatal("[WARNING] Length of names and widgets differs!");
  for (int i = 0; i < p_widget_list.length(); ++i)
    insert_widget_name(p_name_list[i], p_widget_list[i]);
}

void Courtroom::setupWidgetTabs()
{
  ThemeManager::get().CreateTabWidgets();
}

void Courtroom::set_widget_names()
{
  // Assign names to the default widgets
  reset_widget_names();

  // set existing widget names
  for (const QString &widget_name : widget_names.keys())
    widget_names[widget_name]->setObjectName(widget_name);

  // setup table of widgets and names
  insert_widget_names(effect_names, ui_effects);
  insert_widget_names(shout_names, ui_shouts);
  insert_widget_names(wtce_names, ui_wtce);

  for (auto *i_block : qAsConst(ui_free_blocks))
  {
    widget_names.insert(i_block->objectName(), i_block);
    ThemeManager::get().RegisterWidgetGeneric(i_block->objectName(), i_block);
  }

  // timers are special children
  QVector<QString> timer_names;
  for (int i = 0; i < ui_timers.length(); ++i)
  {
    timer_names.append("timer_" + QString::number(i));
  }
  insert_widget_names(timer_names, ui_timers);
}

void Courtroom::set_widget_layers()
{

  if(!ao_app->current_theme->m_jsonLoaded)
  {
    set_widget_layers_legacy();
    return;
  }
  QStringList l_widget_records;


  for(QStringList widget_layers : ao_app->current_theme->widget_layers)
  {
    int count = 0;
    QString l_parent_name = objectName();

    for(QString l_child_name : widget_layers)
    {
      if(l_child_name == "char_select") ThemeManager::get().ParentWidgetsToTabs();
      if(count != 0)
      {
        l_parent_name = widget_layers[0];
      }
      else
      {
        QString l_parent_name = objectName();
      }

      count += 1;

      if (l_widget_records.contains(l_child_name))
      {
        qWarning() << "error: widget already recorded:" << l_child_name;
        continue;
      }
      l_widget_records.append(l_child_name);

      if (!widget_names.contains(l_child_name))
      {
        qWarning() << "widget does not exist:" << l_child_name;
        continue;
      }
      QWidget *l_widget = widget_names.value(l_child_name);
      QWidget *l_parent = widget_names.value(l_parent_name, this);

      qDebug() << "attaching widget" << l_widget->objectName() << "to parent" << l_parent->objectName();

      const bool l_visible = l_widget->isVisible();

      l_widget->setParent(l_parent);

      if (l_visible) l_widget->setVisible(l_visible);
      l_widget->raise();

      if(l_child_name == "viewport")
      {
        QWidget *transitionWidget = widget_names.value("viewport_transition");
        transitionWidget->setVisible(true);
        transitionWidget->raise();
      }

    }
  }


         // do special logic if config panel was not found in courtroom_layers. In
         // particular, make it visible and raise it in front of all assets. This can
         // help assist a theme designer who accidentally missed config_panel and would
         // have become unable to reload themes had they closed the config panel
  if (!l_widget_records.contains("config_panel"))
  {
    ui_config_panel->setParent(this);
    ui_config_panel->setVisible(true);
    ui_config_panel->raise();
  }
}

void Courtroom::set_widget_layers_legacy()
{
  QStringList l_widget_records;

  const QString l_ini_path = ao_app->find_theme_asset_path(COURTROOM_LAYERS_INI);
  QFile l_layer_ini(l_ini_path);
  if (l_layer_ini.open(QFile::ReadOnly))
  {
    QTextStream in(&l_layer_ini);

    const QString l_default_parent_name = objectName();

    l_widget_records.append(l_default_parent_name);

    QString l_parent_name = l_default_parent_name;
    while (!in.atEnd())
    {
      QString l_line = in.readLine().trimmed();

      // skip if line is empty
      if (l_line.isEmpty())
      {
        continue;
      }

      // revert to default parent if we encounter an end scope
      if (l_line.startsWith("[\\"))
      {
        l_parent_name = l_default_parent_name;
      }
      // is this a parent?
      else if (l_line.startsWith("["))
      {
        // update the current parent
        l_parent_name = l_line.remove(0, 1).chopped(1).trimmed();
      }
      // if this is not a parent, it's a child
      else
      {
        // if the child is already known, skip
        if (l_widget_records.contains(l_line))
        {
          qWarning() << "error: widget already recorded:" << l_line;
          continue;
        }
        l_widget_records.append(l_line);

        // attach the children to the parents'
        if (!widget_names.contains(l_line))
        {
          qWarning() << "widget does not exist:" << l_line;
          continue;
        }

        QWidget *l_widget = widget_names.value(l_line);
        QWidget *l_parent = widget_names.value(l_parent_name, this);

        // set child to parent
        qDebug() << "attaching widget" << l_widget->objectName() << "to parent" << l_parent->objectName();
        const bool l_visible = l_widget->isVisible();
        l_widget->setParent(l_parent);
        if (l_visible)
        {
          l_widget->setVisible(l_visible);
        }
        l_widget->raise();

        if(l_line == "viewport")
        {
          QWidget *transitionWidget = widget_names.value("viewport_transition");
          transitionWidget->setVisible(true);
          transitionWidget->raise();
        }

      }
    }
  }

  // do special logic if config panel was not found in courtroom_layers. In
  // particular, make it visible and raise it in front of all assets. This can
  // help assist a theme designer who accidentally missed config_panel and would
  // have become unable to reload themes had they closed the config panel
  if (!l_widget_records.contains("config_panel"))
  {
    ui_config_panel->setParent(this);
    ui_config_panel->setVisible(true);
    ui_config_panel->raise();
  }
}

void Courtroom::set_widgets()
{
  pos_size_type courtroomDimensions = ao_app->get_element_dimensions("courtroom", COURTROOM_DESIGN_INI);
  if (courtroomDimensions.width < 0 || courtroomDimensions.height < 0)
  {
    qWarning() << "W: did not find courtroom width or height in " << COURTROOM_DESIGN_INI;
    courtroomDimensions.width = DEFAULT_WIDTH;
    courtroomDimensions.height = DEFAULT_HEIGHT;
  }

  m_default_size = QSize(courtroomDimensions.width, courtroomDimensions.height);

  if (!m_is_maximized)
  {
    resize(m_default_size);
  }

  if (m_first_theme_loading)
  {
    m_first_theme_loading = false;
    center_widget_to_screen(this);
  }

  ui_background->move(0, 0);
  ui_background->resize(m_default_size);
  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Set Resize");
  ui_background->set_theme_image(ao_app->current_theme->get_widget_image("courtroom", "courtroombackground.png", "courtroom"));

  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Set Background");

  ThemeManager::get().RefreshWidgetsButton();
  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "SetWidget-BUttons");
  ThemeManager::get().RefreshWidgetsLineEdit();
  ThemeManager::get().RefreshWidgetsComboBox();
  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "SetWidget-ComboBox");

  setupWidgetElement(ui_viewport, "viewport");
  setupWidgetElement(wShoutsLayer, "viewport");

  setupWidgetElement(SceneManager::get().GetTransition(), "viewport");
  SceneManager::get().GetTransition()->move(0,0);
  setupWidgetElement(ui_vp_notepad_image, "notepad_image", "notepad_image.png", false);
  setupWidgetElement(ui_vp_notepad, "notepad", "", Qt::TextEditable, false);
  setupWidgetElement(ui_vp_showname, "showname");
  setupWidgetElement(ui_vp_showname_image, "showname_image");

  ui_vp_showname_image->hide();

  setupWidgetElement(ui_vp_message, "message", "", Qt::NoTextInteraction);
  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Setup Viewport Widgets");

  set_size_and_pos(ui_vp_chat_arrow, "chat_arrow", COURTROOM_DESIGN_INI, ao_app);

  if (!ao_app->find_theme_asset_path("chat_arrow", animated_or_static_extensions()).isEmpty())
  {
    ui_vp_chat_arrow->set_theme_image("chat_arrow");
  }
  set_sticker_play_once(ui_vp_chat_arrow, "chat_arrow", COURTROOM_CONFIG_INI, ao_app);
  ui_vp_chat_arrow->hide();

  {
    const bool l_visible = ui_vp_loading->isVisible();
    set_size_and_pos(ui_vp_loading, "loading", COURTROOM_DESIGN_INI, ao_app);
    ui_vp_loading->set_theme_image("loading");
    ui_vp_loading->start();
    ui_vp_loading->setVisible(l_visible);
  }

  set_size_and_pos(wEvidenceDescription, "evidence_description", COURTROOM_DESIGN_INI, ao_app);
  set_size_and_pos(ui_ic_chatlog, "ic_chatlog", COURTROOM_DESIGN_INI, ao_app);

  if(ao_app->current_theme->get_widget_settings_bool("ic_chatlog", "courtroom", "hide_frame")) ui_ic_chatlog->setFrameStyle(QFrame::NoFrame);

  setupWidgetElement(ui_area_desc, "area_desc");

  set_size_and_pos(ui_area_desc, "area_desc", COURTROOM_DESIGN_INI, ao_app);

  if(ao_app->current_theme->get_widget_settings_bool("area_desc", "courtroom", "hide_frame")) ui_area_desc->setFrameStyle(QFrame::NoFrame);

  NotifyManager::get().ReloadNotification();

  set_size_and_pos(ui_ooc_chatlog, "server_chatlog", COURTROOM_DESIGN_INI, ao_app);

  if(ao_app->current_theme->get_widget_settings_bool("server_chatlog", "courtroom", "hide_frame"))
  {
    ui_ooc_chatlog->setFrameStyle(QFrame::NoFrame);
  }

  set_size_and_pos(ui_sfx_list, "sfx_list", COURTROOM_DESIGN_INI, ao_app);

  set_size_and_pos(wCharaAnimList, "chara_animations", COURTROOM_DESIGN_INI, ao_app);

  set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message", COURTROOM_DESIGN_INI, ao_app);
  set_text_alignment(ui_ic_chat_message_field, "ao2_ic_chat_message", COURTROOM_FONTS_INI, ao_app);
  if (!set_stylesheet(ui_ic_chat_message, "[IC LINE]", COURTROOM_STYLESHEETS_CSS, ao_app))
  {
    ui_ic_chat_message->setStyleSheet("background-color: rgba(100, 100, 100, 255);");
  }
  ui_ic_chat_message_field->setStyleSheet(ui_ic_chat_message->styleSheet());
  ui_ic_chat_message_counter->setStyleSheet(ui_ic_chat_message->styleSheet());




  set_size_and_pos(ui_vp_chatbox, "ao2_chatbox", COURTROOM_DESIGN_INI, ao_app);
  set_sticker_play_once(ui_vp_chatbox, "ao2_chatbox", COURTROOM_CONFIG_INI, ao_app);

  set_size_and_pos(ui_vp_music_area, "music_area", COURTROOM_DESIGN_INI, ao_app);
  ui_vp_music_area->show();
  set_size_and_pos(ui_vp_music_name, "music_name", COURTROOM_DESIGN_INI, ao_app);

  setupWidgetElement(w_ViewportOverlay, "viewport", true);
  w_ViewportOverlay->move(0, 0);


  setupWidgetElement(wEvidencePreviewImage, "evidence_image", "evidence_missing.png", true);

  setupWidgetElement(ui_vp_music_display_a, "music_display_a", "music_display_a.png", true);
  setupWidgetElement(ui_vp_music_display_b, "music_display_b", "music_display_b.png", true);

  set_size_and_pos(ui_vp_clock, "clock", COURTROOM_DESIGN_INI, ao_app);
  if (m_current_clock == -1)
    ui_vp_clock->hide();
  set_sticker_play_once(ui_vp_clock, "clock", COURTROOM_CONFIG_INI, ao_app);

  ui_vp_chatbox->set_theme_image("chatmed");
  ui_vp_chatbox->hide();


  set_size_and_pos(ui_music_list, "music_list", COURTROOM_DESIGN_INI, ao_app);
  set_stylesheet(ui_music_list, "[music_list]", COURTROOM_STYLESHEETS_CSS, ao_app);


  ui_music_list->show();
  ui_music_search->show();

  { // area separation logic
    const bool l_is_area_music_list_separated = is_area_music_list_separated();
    const QString p_area_identifier = l_is_area_music_list_separated ? "area" : "music";

    set_size_and_pos(ui_area_list, p_area_identifier + "_list", COURTROOM_DESIGN_INI, ao_app);
    set_size_and_pos(ui_area_search, p_area_identifier + "_search", COURTROOM_DESIGN_INI, ao_app);
    set_text_alignment(ui_area_search, p_area_identifier + "_search", COURTROOM_FONTS_INI, ao_app);
    set_stylesheet(ui_area_search, "[AREA SEARCH]", COURTROOM_STYLESHEETS_CSS, ao_app);

    ui_area_list->setVisible(l_is_area_music_list_separated);
    ui_area_search->setVisible(l_is_area_music_list_separated);
    ui_switch_area_music->setHidden(l_is_area_music_list_separated);
  }

  // emotes
  set_size_and_pos(ui_emotes, "emotes", COURTROOM_DESIGN_INI, ao_app);
  construct_emote_page_layout();



  if(ThemeManager::get().GetFullReloadQueued())
  {
    { // emote preview
      pos_size_type l_emote_preview_size = ao_app->get_element_dimensions("emote_preview", COURTROOM_DESIGN_INI);
      if (l_emote_preview_size.width <= 0 || l_emote_preview_size.height <= 0)
      {
        l_emote_preview_size.width = 320;
        l_emote_preview_size.height = 192;
      }
      ui_emote_preview->resize(l_emote_preview_size.width, l_emote_preview_size.height);
      ui_emote_preview_background->set_theme_image("emote_preview.png");
      ui_emote_preview_character->set_size(QSizeF(l_emote_preview_size.width, l_emote_preview_size.height));
      TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Emote Preview");
    }
  }


  set_size_and_pos(ui_emote_dropdown, "emote_dropdown", COURTROOM_DESIGN_INI, ao_app);
  set_stylesheet(ui_emote_dropdown, "[EMOTE DROPDOWN]", COURTROOM_STYLESHEETS_CSS, ao_app);

  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Emote Drowndown");

  set_size_and_pos(ui_iniswap_dropdown, "iniswap_dropdown", COURTROOM_DESIGN_INI, ao_app);
  UpdateIniswapStylesheet();
  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Iniswap Dropdown");

  set_size_and_pos(p_DropdownMusicCategory, "category_dropdown", COURTROOM_DESIGN_INI, ao_app);
  set_stylesheet(p_DropdownMusicCategory, "[CATEGORY DROPDOWN]", COURTROOM_STYLESHEETS_CSS, ao_app);

  set_size_and_pos(p_DropdownPosition, "pos_dropdown", COURTROOM_DESIGN_INI, ao_app);
  set_stylesheet(p_DropdownPosition, "[POS DROPDOWN]", COURTROOM_STYLESHEETS_CSS, ao_app);


  setupWidgetElement(ui_defense_bar, "defense_bar", "defensebar" + QString::number(defense_bar_state) + ".png", true);
  setupWidgetElement(ui_prosecution_bar, "prosecution_bar", "prosecutionbar" + QString::number(prosecution_bar_state) + ".png", true);

  for (int i = 0; i < shout_names.size(); ++i)
  {
    set_size_and_pos(ui_shouts[i], shout_names[i], COURTROOM_DESIGN_INI, ao_app);
  }
  reset_shout_buttons();

  TimeDebugger::get().CheckpointTimer("Courtroom Setup", "Shout Buttons");


  // courtroom_config.ini necessary + check for crash
  if (ao_app->current_theme->read_config_bool("enable_single_shout") && ui_shouts.size() > 0)
  {
    for (auto &shout : ui_shouts)
      move_widget(shout, "bullet");

    set_shouts();

    ui_shout_up->show();
    ui_shout_down->show();
  }

  for (int i = 0; i < effect_names.size(); ++i)
  {
    set_size_and_pos(ui_effects[i], effect_names[i], COURTROOM_DESIGN_INI, ao_app);

    ThemeManager::get().RegisterWidgetGeneric(effect_names[i], ui_effects[i]);
  }
  reset_effect_buttons();



  if (ao_app->current_theme->read_config_bool("enable_single_effect")  && ui_effects.size() > 0 ) // check to prevent crashing
  {
    for (auto &effect : ui_effects)
      move_widget(effect, "effect");

    set_effects();

    ui_effect_up->show();
    ui_effect_down->show();
  }


  for (int i = 0; i < wtce_names.size(); ++i)
  {
    set_size_and_pos(ui_wtce[i], wtce_names[i], COURTROOM_DESIGN_INI, ao_app);
    ThemeManager::get().RegisterWidgetGeneric(wtce_names[i], ui_wtce[i]);
  }

  if (ao_app->current_theme->read_config_bool("enable_single_wtce")) // courtroom_config.ini necessary
  {
    for (auto &wtce : ui_wtce)
      move_widget(wtce, "wtce");
    qDebug() << "AA: single wtce";
  }
  set_judge_wtce();
  reset_wtce_buttons();

  VariableManager::get().setWatchlist({});

  for (DRStickerViewer *i_sticker : ui_free_blocks)
  {
    const QString l_name = i_sticker->objectName();
    set_size_and_pos(i_sticker, l_name, COURTROOM_DESIGN_INI, ao_app);
    QString l_path = ao_app->current_theme->getFreeblockImage(l_name);
    i_sticker->set_variable_string(l_path);
    QMap<QString, QString> l_variables = ao_app->current_theme->getFreeblockVariables(l_name);
    i_sticker->set_variable_map(l_variables);

    for(QString rKey : l_variables.values())
    {
      VariableManager::get().addWatchlist(l_name, rKey);
    }

    i_sticker->set_theme_image(l_path);
    set_sticker_play_once(i_sticker, l_name, COURTROOM_CONFIG_INI, ao_app);
    i_sticker->show();
  }




  // The config panel has a special property. If it is displayed beyond the right or lower limit of the window, it will
  // be moved to 0, 0 A similar behavior will occur if the button is resized to 0, 0 due to 'config_panel' not being
  // found in courtroom_design.ini This is to assist with people who switch to incompatible and/or smaller themes and
  // have the button disappear
  if (ui_config_panel->x() > width() || ui_config_panel->y() > height() || ui_config_panel->width() <= 0 || ui_config_panel->height() <= 0)
  {
    ui_config_panel->setVisible(true);
    ui_config_panel->move(0, 0);
    // Moreover, if the width or height is invalid, change it to some fixed
    // values
    if (ui_config_panel->width() <= 0 || ui_config_panel->height() <= 0)
      ui_config_panel->resize(64, 64);
  }

  set_size_and_pos(ui_pre, "pre", COURTROOM_DESIGN_INI, ao_app);
  ui_pre->setText("Pre");

  set_size_and_pos(ui_flip, "flip", COURTROOM_DESIGN_INI, ao_app);

  set_size_and_pos(ui_hide_character, "hidden", COURTROOM_DESIGN_INI, ao_app);

  for (int i = 0; i < ui_label_images.size(); ++i)
  {
    set_size_and_pos(ui_label_images[i], label_images[i].toLower() + "_image", COURTROOM_DESIGN_INI, ao_app);
    ThemeManager::get().RegisterWidgetGeneric(label_images[i].toLower() + "_image", ui_label_images[i]);
  }

  if (ao_app->current_theme->read_config_bool("enable_label_images"))
  {
    for (int i = 0; i < ui_checks.size(); ++i) // loop through checks
    {
      QString image = label_images[i].toLower() + ".png";
      ui_label_images[i]->set_theme_image(image);

      if (!ui_label_images[i]->get_image().isEmpty())
        ui_checks[i]->setText("");
      else
        ui_checks[i]->setText(label_images[i]);
    }

    for (int i = 0; i < ui_labels.size(); ++i) // now through labels..........
    {
      int j = i + ui_checks.size();
      QString image = label_images[j].toLower() + ".png";
      ui_label_images[j]->set_theme_image(image);

      if (!ui_label_images[j]->get_image().isEmpty())
        ui_labels[i]->setText("");
      else
        ui_labels[i]->setText(label_images[j]);
    }
  }
  else
  {
    for (int i = 0; i < ui_checks.size(); ++i) // same thing
    {
      ui_checks[i]->setText(label_images[i]);
      ui_label_images[i]->set_theme_image("");
    }

    for (int i = 0; i < ui_labels.size(); ++i) // same thing
    {
      int j = i + ui_checks.size();
      ui_labels[i]->setText(label_images[j]);
      ui_label_images[j]->set_theme_image("");
    }
  }




  ui_char_button_selector->set_theme_image("char_selector.png");
  ui_char_button_selector->hide();

  set_size_and_pos(ui_char_buttons, "char_buttons", COURTROOM_DESIGN_INI, ao_app);


  update_music_text_anim();

  set_size_and_pos(pNotifyPopup, "notify_popup", COURTROOM_DESIGN_INI, ao_app);
  set_size_and_pos(pUIPairOffsetSlider, "pair_offset", COURTROOM_DESIGN_INI, ao_app);

  set_size_and_pos(ui_set_notes, "set_notes_button", COURTROOM_DESIGN_INI, ao_app);
  ui_set_notes->set_image("set_notes.png");
  ui_note_area->m_layout->setSpacing(10);
  set_size_and_pos(ui_note_area, "note_area", COURTROOM_DESIGN_INI, ao_app);
  set_size_and_pos(ui_note_scroll_area, "note_area", COURTROOM_DESIGN_INI, ao_app);
  ui_note_scroll_area->setWidget(ui_note_area);

  ui_note_area->set_theme_image("note_area.png");
  ui_note_area->add_button->set_image("add_button.png");
  ui_note_area->add_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  ui_note_area->setLayout(ui_note_area->m_layout);
  ui_note_area->show();
  ui_note_scroll_area->hide();



  set_size_and_pos(wEvidenceList, "evidence_list", COURTROOM_DESIGN_INI, ao_app);
  set_size_and_pos(ui_player_list, "player_list", COURTROOM_DESIGN_INI, ao_app);


  list_note_files();

  if (!contains_add_button)
  {
    ui_note_area->m_layout->addWidget(ui_note_area->add_button);
    contains_add_button = true;
  }

  // This is used to force already existing notepicker elements to reset their image and theme setting
  for (AONotePicker *notepicker : ui_note_area->findChildren<AONotePicker *>())
  {
    for (AOButton *button : notepicker->findChildren<AOButton *>())
    {
      button->refresh_image();
    }
    QLineEdit *f_line = notepicker->findChild<QLineEdit *>();
    set_stylesheet(f_line, "[LINE EDIT]", COURTROOM_STYLESHEETS_CSS, ao_app);
  }

  adapt_numbered_items(ui_timers, "timer_number", "timer");
  set_fonts();

  Q_EMIT loaded_theme();

}

void Courtroom::setupWidgetElement(QWidget *widget, QString name, bool visible)
{
  set_size_and_pos(widget, name, COURTROOM_DESIGN_INI, ao_app);
  if(!visible) widget->hide();
}


void Courtroom::setupWidgetElement(AOImageDisplay *widget, QString name, QString image, bool visible)
{
  set_size_and_pos(widget, name, COURTROOM_DESIGN_INI, ao_app);

  widget->set_theme_image(ao_app->current_theme->get_widget_image(name, image, "courtroom"));

  if(!visible) widget->hide();
}

void Courtroom::setupWidgetElement(DRTextEdit *widget, QString name, QString defaultText, Qt::TextInteractionFlag flag, bool visible)
{
  set_size_and_pos(widget, name, COURTROOM_DESIGN_INI, ao_app);

  widget->setTextInteractionFlags(flag);
  if(mDefaultWidgetCSS.contains(name)) widget->setStyleSheet(mDefaultWidgetCSS[name]);

  if(!visible) widget->hide();
}

void Courtroom::move_widget(QWidget *p_widget, QString p_identifier)
{
  QString filename = COURTROOM_DESIGN_INI;

  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    // Don't hide, as some widgets don't have a built-in way of reappearing again.
    p_widget->move(0, 0);
    p_widget->resize(0, 0);
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
  }
}

template <typename T>
int Courtroom::adapt_numbered_items(QVector<T *> &item_vector, QString config_item_number, QString item_name)
{
  // &item_vector must be a vector of size at least 1!

  // Redraw the new correct number of items.
  int new_item_number = ao_app->current_theme->read_config_int(config_item_number);



  int current_item_number = item_vector.size();
  // Note we use the fact that, if config_item_number is not there,
  // read_theme_ini returns an empty string, which .toInt() would fail to
  // convert and by documentation transform to 0.

  // Decide what to do if the new theme has a different amount of items than the
  // old one
  if (new_item_number < current_item_number)
  {
    // Hide old items if there are any.
    for (int i = new_item_number; i < current_item_number; i++)
    {
      item_vector[i]->hide();
    }
  }
  else if (current_item_number < new_item_number)
  {
    // Create new items
    item_vector.resize(new_item_number);
    for (int i = current_item_number; i < new_item_number; i++)
    {
      item_vector[i] = new T(this);
      item_vector[i]->stackUnder(item_vector[i - 1]);
      // index i-1 exists as i >= current_item_number == item_vector.size() >= 1
    }
  }
  // Note that by design we do *not* destroy items (or similarly, the size of
  // item_vector is non-decreasing. This is because we want to allow for items
  // to, say, run in the background as invisible. With that said, we can now
  // properly format our new items
  for (int i = 0; i < new_item_number; i++)
  {
    item_vector[i]->show();
    set_size_and_pos(item_vector[i], item_name + "_" + QString::number(i), COURTROOM_DESIGN_INI, ao_app);
    // Note that show is deliberately placed before set_size_and_pos
    // That is because we want to retain the functionality set_size_and_pos
    // includes where hides a widget if it is unable to find a position for it,
    // and does not change its visibility otherwise.
  }
  return new_item_number;
}

void Courtroom::check_effects()
{
  // Asset lookup order
  // 1. In the character folder, look for
  // `effect_names.at(i)` + extensions in `exts` in order
  // 2. In the theme folder (gamemode-timeofday/main/default), look for
  // `effect_names.at(i)` + extensions in `exts` in order
  // Only enable buttons where a file was found

  for (int i = 0; i < ui_effects.size(); ++i)
  {
    QString path = ao_app->find_asset_path({ao_app->get_character_path(get_character_ini(), effect_names.at(i))}, animated_extensions());
    if (path.isEmpty())
      path = ao_app->find_theme_asset_path(effect_names.at(i), animated_extensions());

    if (path.isEmpty())
      path = ao_app->find_asset_path(ao_app->get_base_path() + "effects/default/" + effect_names.at(i), animated_extensions());

    effects_enabled[i] = (!path.isEmpty());
  }
}

void Courtroom::check_shouts()
{
  // Asset lookup order
  // 1. In the character folder, look for
  // `shout_names.at(i)` + extensions in `exts` in order
  // 2. In the theme folder (gamemode-timeofday/main/default), look for
  // `shout_names.at(i)` + extensions in `exts` in order
  // 3. In the shouts folder (base/shouts), look for
  // `shout_names.at(i)` + extensions in `exts` in order
  // Only enable buttons where a file was found

  for (int i = 0; i < ui_shouts.size(); ++i)
  {
    QString path = ao_app->find_asset_path({ao_app->get_character_path(get_character_ini(), shout_names.at(i))}, animated_extensions());

    if (path.isEmpty())
      path = ao_app->find_theme_asset_path(shout_names.at(i), animated_extensions());

    if (path.isEmpty())
      path = ao_app->find_asset_path(ao_app->get_base_path() + "shouts/default/" + shout_names.at(i), animated_extensions());

    shouts_enabled[i] = (!path.isEmpty());
  }
}

void Courtroom::check_wtce()
{
  // Asset lookup order
  // 1. In the character folder, look for
  // `wtce_names.at(i)` + extensions in `exts` in order
  // 2. In the theme folder (gamemode-timeofday/main/default), look for
  // `wtce_names.at(i)` + extensions in `exts` in order
  // Only enable buttons where a file was found

  for (int i = 0; i < ui_wtce.size(); ++i)
  {
    QString path = ao_app->find_asset_path({ao_app->get_character_path(get_character_ini(), wtce_names.at(i))}, animated_extensions());
    if (path.isEmpty())
      path = ao_app->find_theme_asset_path(wtce_names.at(i), animated_extensions());
    wtce_enabled[i] = (!path.isEmpty());
  }
}

void Courtroom::delete_widget(QWidget *p_widget)
{
  // remove the widget from recorded names
  QString l_widgetName = p_widget->objectName();
  widget_names.remove(l_widgetName);
  ThemeManager::get().UnregisterWidget(l_widgetName);

  // transfer the children to our grandparent since our parent is about to be deleted
  QWidget *grand_parent = p_widget->parentWidget();
  // if we don't have a grand parent, attach ourselves to courtroom
  if (!grand_parent)
    grand_parent = this;

  // set new parent
  for (QWidget *child : p_widget->findChildren<QWidget *>(nullptr, Qt::FindDirectChildrenOnly))
    child->setParent(grand_parent);

  // delete widget
  delete p_widget;
}

void Courtroom::load_effects()
{
  // Close any existing effects to prevent memory leaks
  for (QWidget *widget : qAsConst(ui_effects))
    delete_widget(widget);

  // And create new effects
  int effect_number = ao_app->current_theme->get_effects_count();
  effects_enabled.resize(effect_number);
  ui_effects.resize(effect_number);

  for (int i = 0; i < ui_effects.size(); ++i)
  {
    AOButton *l_button = new AOButton(this, ao_app);
    ui_effects.replace(i, l_button);
    l_button->setCheckable(true);
    l_button->setProperty("effect_id", i + 1);
    l_button->stackUnder(ui_effect_up);
    l_button->stackUnder(ui_effect_down);

    connect(l_button, SIGNAL(clicked(bool)), this, SLOT(on_effect_button_clicked(bool)));
    connect(l_button, SIGNAL(toggled(bool)), this, SLOT(on_effect_button_toggled(bool)));
  }

  // And add names
  effect_names.clear();
  for (int i = 1; i <= ui_effects.size(); ++i)
  {
    QStringList names = ao_app->get_effect(i);
    if (!names.isEmpty())
    {
      const QString l_name = names.at(0).trimmed();
      effect_names.append(l_name);
      AOButton *l_button = ui_effects.at(i - 1);
      l_button->setProperty("effect_name", l_name);
      Q_EMIT l_button->toggled(l_button->isChecked());
    }
  }
}

void Courtroom::load_free_blocks()
{
  for (QWidget *widget : qAsConst(ui_free_blocks))
    delete_widget(widget);

  ui_free_blocks.clear();
  const int l_block_count = ao_app->current_theme->get_free_block_count();
  for (int i = 0; i < l_block_count; ++i)
  {
    QString l_imagePath = "";
    QString l_name = "";
    if(ao_app->current_theme->m_jsonLoaded)
    {
      l_name = ao_app->current_theme->get_free_block(i);
      l_imagePath = ao_app->current_theme->getFreeblockImage(i);
    }
    else
    {
      l_name = ao_app->get_spbutton("[FREE BLOCKS]", i + 1).trimmed();
      l_imagePath = "free_block_" + l_name;
    }
    if (l_name.isEmpty())
    {
      qWarning() << "error: block index" << i << "has no block name.";
      continue;
    }
    const QString l_block_name = "free_block_" + l_name;
    auto *l_block = new DRStickerViewer(ao_app, this);
    l_block->setObjectName(l_block_name);
    ui_free_blocks.append(l_block);
    widget_names.insert(l_block_name, l_block);
    ThemeManager::get().RegisterWidgetGeneric(l_block_name, l_block);
  }
}

void Courtroom::load_shouts()
{
  for (QWidget *widget : qAsConst(ui_shouts))
    delete_widget(widget);

  // And create new shouts
  int shout_number = ao_app->current_theme->get_shouts_count();
  shouts_enabled.resize(shout_number);
  ui_shouts.resize(shout_number);

  shout_names.clear();
  for (int i = 0; i < ui_shouts.size(); ++i)
  {
    AOButton *l_button = new AOButton(this, ao_app);
    ui_shouts.replace(i, l_button);
    l_button->setCheckable(true);
    l_button->setProperty("shout_id", i + 1);
    l_button->stackUnder(ui_shout_up);
    l_button->stackUnder(ui_shout_down);

    connect(l_button, SIGNAL(clicked(bool)), this, SLOT(on_shout_button_clicked(bool)));
    connect(l_button, SIGNAL(toggled(bool)), this, SLOT(on_shout_button_toggled(bool)));

    QString shout_name = ao_app->current_theme->get_shout(i + 1);

    if(!shout_name.isEmpty())
    {
      shout_names.append(shout_name);
      AOButton *l_button = ui_shouts.at(i);
      widget_names.insert(shout_name, l_button);
      l_button->setObjectName(shout_name);
      l_button->setProperty("shout_name", shout_name);
      Q_EMIT l_button->toggled(l_button->isChecked());
    }
  }

}

void Courtroom::load_wtce()
{
  for (QWidget *widget : qAsConst(ui_wtce))
    delete_widget(widget);

  // And create new wtce buttons
  const int l_wtce_count = ao_app->current_theme->get_wtce_count();
  wtce_enabled.resize(l_wtce_count);

  ui_wtce.clear();
  wtce_names.clear();
  for (int i = 0; i < l_wtce_count; ++i)
  {
    AOButton *l_button = new AOButton(this, ao_app);
    ui_wtce.append(l_button);
    l_button->setProperty("wtce_id", i + 1);
    l_button->stackUnder(ui_wtce_up);
    l_button->stackUnder(ui_wtce_down);
    connect(l_button, SIGNAL(clicked(bool)), this, SLOT(on_wtce_clicked()));

    QString wtce_name = ao_app->current_theme->get_wtce(i + 1);

    if(!wtce_name.isEmpty())
    {
      wtce_names.append(wtce_name);
      widget_names[wtce_name] = ui_wtce[i];
      ui_wtce[i]->setObjectName(wtce_name);
    }

  }

}

/**
 * @brief Show the currently selected shout button, hide the remaining ones.
 * If no shouts exist, this method does nothing.
 */
void Courtroom::set_shouts()
{
  for (auto &shout : ui_shouts)
    shout->hide();
  if (ui_shouts.size() > 0)
    ui_shouts[m_shout_current]->show(); // check to prevent crashing
}

/**
 * @brief Show the currently selected effect button, hide the remaining ones.
 * If no effects exist, this method does nothing.
 */
void Courtroom::set_effects()
{
  for (auto &effect : ui_effects)
    effect->hide();

  // check to prevent crashing
  if (ui_effects.size() > 0)
    ui_effects[m_effect_current]->show();
}

void Courtroom::set_judge_enabled(bool p_enabled)
{
  is_judge = p_enabled;

  // set judge button visibility
  ui_defense_plus->setVisible(is_judge && ui_in_current_toggle("defense_plus"));
  ui_defense_minus->setVisible(is_judge && ui_in_current_toggle("defense_minus"));
  ui_prosecution_plus->setVisible(is_judge && ui_in_current_toggle("prosecution_plus"));
  ui_prosecution_minus->setVisible(is_judge && ui_in_current_toggle("prosecution_minus"));

  if(ui_in_current_toggle("player_list")) construct_playerlist_layout();

  set_judge_wtce();
}

/**
 * @brief Show the currently selected splash button, hide the remaining ones.
 * If no splashes exist, this method does nothing.
 */
void Courtroom::set_judge_wtce()
{
  // hide all wtce before enabling visibility
  for (auto &wtce : ui_wtce)
    wtce->hide();

  // check if we use a single wtce or multiple
  const bool is_single_wtce = ao_app->current_theme->read_config_bool("enable_single_wtce");

  // update visibility for next/previous
  ui_wtce_up->setVisible(is_judge && is_single_wtce && ui_in_current_toggle("wtce_up"));
  ui_wtce_down->setVisible(is_judge && is_single_wtce && ui_in_current_toggle("wtce_down"));

  // prevent going ahead if we have no wtce
  if (!is_judge || ui_wtce.length() == 0 || !ui_in_current_toggle("wtce"))
    return;

  // set visibility based off parameter
  if (is_single_wtce == true)
  {
    ui_wtce[m_wtce_current]->show();
  }
  else
  {
    for (AOButton *i_wtce : qAsConst(ui_wtce))
      i_wtce->show();
  }
}

void Courtroom::set_fonts()
{
  set_drtextedit_font(ui_vp_showname, "showname", COURTROOM_FONTS_INI, ao_app);
  ui_vp_showname->setPlainText(ui_vp_showname->toPlainText());
  set_drtextedit_font(ui_vp_message, "message", COURTROOM_FONTS_INI, ao_app);
  ui_vp_message->setPlainText(ui_vp_message->toPlainText());
  set_drtextedit_font(ui_ic_chatlog, "ic_chatlog", COURTROOM_FONTS_INI, ao_app);

  set_drtextedit_font(wEvidenceDescription, "evidence_description", COURTROOM_FONTS_INI, ao_app);
  wEvidenceDescription->setPlainText(wEvidenceDescription->toPlainText());

  set_drtextedit_font(ui_area_desc, "area_desc", COURTROOM_FONTS_INI, ao_app);
  ui_area_desc->setPlainText(ui_area_desc->toPlainText());


  // Chatlog does not support drtextedit because html
  set_font(ui_ooc_chatlog, "server_chatlog", COURTROOM_FONTS_INI, ao_app);

  ui_ooc_chatlog->reset_message_format();

  set_font(ui_music_list, "music_list", COURTROOM_FONTS_INI, ao_app);
  set_font(ui_area_list, "area_list", COURTROOM_FONTS_INI, ao_app);
  set_font(ui_sfx_list, "sfx_list", COURTROOM_FONTS_INI, ao_app);
  set_font(wCharaAnimList, "chara_animations", COURTROOM_FONTS_INI, ao_app);
  set_drtextedit_font(ui_vp_music_name, "music_name", COURTROOM_FONTS_INI, ao_app);
  ui_vp_music_name->setPlainText(ui_vp_music_name->toPlainText());
  set_drtextedit_font(ui_vp_notepad, "notepad", COURTROOM_FONTS_INI, ao_app);
  ui_vp_notepad->setPlainText(ui_vp_notepad->toPlainText());

  for (int i = 0; i < ui_timers.length(); ++i)
  {
    AOTimer *i_timer = ui_timers.at(i);
    set_drtextedit_font(i_timer, QString("timer_%1").arg(i), COURTROOM_FONTS_INI, ao_app);
  }
}

void Courtroom::setup_screenshake_anim(double message_offset)
{
  pos_size_type chatbox_res = ao_app->get_element_dimensions("ao2_chatbox", COURTROOM_DESIGN_INI);

  background_anim->setLoopCount(5);
  background_anim->setDuration(50);
  background_anim->setStartValue(QPoint(0 + 25, 0 + 20));
  background_anim->setKeyValueAt(0.30, QPoint(0, 0));
  background_anim->setKeyValueAt(0.60, QPoint(0 + -25, 0 + 20));
  background_anim->setEndValue(QPoint(0, 0));

  chatbox_anim->setLoopCount(5);
  chatbox_anim->setDuration(50);
  chatbox_anim->setStartValue(QPoint(chatbox_res.x + 25, chatbox_res.y + 20));
  chatbox_anim->setKeyValueAt(0.30, QPoint(chatbox_res.x, chatbox_res.y));
  chatbox_anim->setKeyValueAt(0.60, QPoint(chatbox_res.x + -25, chatbox_res.y + 20));
  chatbox_anim->setEndValue(QPoint(chatbox_res.x, chatbox_res.y));

  player_sprite_anim->setLoopCount(5);
  player_sprite_anim->setDuration(50);
  player_sprite_anim->setStartValue(QPoint(message_offset - 25, 0 + 20));
  player_sprite_anim->setKeyValueAt(0.30, QPoint(message_offset, 0));
  player_sprite_anim->setKeyValueAt(0.60, QPoint(message_offset + 25, 0 + 20));
  player_sprite_anim->setEndValue(QPoint(message_offset, 0));

}

void Courtroom::play_screenshake_anim()
{

  chatbox_anim->start();
  background_anim->start();
  player_sprite_anim->start();

}
