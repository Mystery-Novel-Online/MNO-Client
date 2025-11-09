cmake_minimum_required(VERSION 3.16)

project(rp-engine VERSION 1.7.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS core gui widgets uitools network multimedia multimediawidgets)
find_package(Qt5 REQUIRED COMPONENTS core gui widgets uitools network multimedia multimediawidgets)

set(HEADERS
  src/engine/animation/keyframe_channel.h
  src/engine/animation/keyframe_sequence.h
  src/engine/fs/fs_mounting.h
  src/engine/fs/fs_reading.h
  src/engine/fs/fs_writing.h
  src/engine/interface/courtroom_layout.h
  src/engine/interface/lobby_layout.h
  src/engine/interface/menus/area_menu.h
  src/engine/interface/menus/bgm_menu.h
  src/engine/interface/menus/char_menu.h
  src/engine/interface/menus/emote_menu.h
  src/engine/interface/scenes/replay_window.h
  src/engine/interface/widgets/bgm_filter.h
  src/engine/interface/widgets/char_button.h
  src/engine/interface/widgets/character_select.h
  src/engine/interface/widgets/chat_log.h
  src/engine/interface/widgets/emotion_button.h
  src/engine/interface/widgets/health_bar.h
  src/engine/interface/widgets/image_display.h
  src/engine/interface/widgets/note_area.h
  src/engine/interface/widgets/note_picker.h
  src/engine/interface/widgets/player_list_slot.h
  src/engine/interface/widgets/rp_button.h
  src/engine/interface/widgets/rp_combo_box.h
  src/engine/interface/widgets/rp_hover_widget.h
  src/engine/interface/widgets/rp_label.h
  src/engine/interface/widgets/rp_line_edit.h
  src/engine/interface/widgets/rp_slider.h
  src/engine/interface/widgets/rp_text_edit.h
  src/engine/interface/widgets/rp_typewriter.h
  src/engine/interface/widgets/rp_widget.h
  src/engine/interface/widgets/screenshot_button.h
  src/engine/interface/widgets/emotion_selector.h
  src/engine/interface/widgets/choice_dialog.h
  src/engine/interface/widgets/sticker_viewer.h
  src/engine/interface/widgets/viewport_overlay.h
  src/engine/interface/widgets/tab_toggle_button.h
  src/engine/interface/widgets/aotimer.h
  src/engine/interface/widgets/viewports/legacy_viewport.h
  src/engine/interface/widgets/viewports/rp_viewport.h
  src/engine/network/json_packet.h
  src/engine/network/metadata/area_metadata.h
  src/engine/network/metadata/message_metadata.h
  src/engine/network/metadata/user_metadata.h
  src/engine/network/metadata/server_metadata.h
  src/engine/network/metadata/tracklist_metadata.h
  src/engine/param/animation_reader.h
  src/engine/param/replay_reader.h
  src/engine/system/animation.h
  src/engine/system/audio.h
  src/engine/system/audio/ambience_player.h
  src/engine/system/audio/blip_player.h
  src/engine/system/audio/music_player.h
  src/engine/system/audio/sfx_player.h
  src/engine/system/audio/shout_player.h
  src/engine/system/audio/system_player.h
  src/engine/system/effects.h
  src/engine/system/localization.h
  src/engine/system/replay_playback.h
  src/engine/system/runtime_loop.h
  src/engine/system/text_encoding.h
  src/engine/system/theme_scripting.h
  src/engine/system/debug/time_debugger.h
  src/engine/themes/theme_scene.h
  src/engine/tools/button_maker.h
  src/engine/param/json_reader.h
  src/engine/param/localization_reader.h
  src/engine/param/theme_mode_reader.h
  src/engine/param/theme_module_reader.h
  src/engine/param/theme_reader.h
  src/engine/param/actor/actor_loader.h


  src/engine/param/background/legacy_background_reader.h
  src/aoapplication.h
  src/aoconfig.h
  src/aoconfigpanel.h
  src/aoguiloader.h
  src/aoobject.h
  src/aopixmap.h
  src/commondefs.h
  src/courtroom.h
  src/datatypes.h
  src/debug_functions.h
  src/draudio.h
  src/draudiodevice.h
  src/draudioengine.h
  src/draudioengine_p.h
  src/draudioerror.h
  src/draudiostream.h
  src/draudiostreamfamily.h
  src/draudiotrackmetadata.h
  src/drcharactermovie.h

  src/dreffectmovie.h
  src/drgraphicscene.h
  src/drmasterclient.h
  src/drmediatester.h
  src/drmovie.h
  src/drpacket.h
  src/drposition.h
  src/drscenemovie.h
  src/drserverinfoeditor.h
  src/drserversocket.h
  src/drshoutmovie.h
  src/drsplashmovie.h
  src/drtheme.h
  src/drthememovie.h
  src/hardware_functions.h
  src/lobby.h
  src/logger.h
  src/mk2/drplayer.h
  src/mk2/graphicsspriteitem.h
  src/mk2/graphicsvideoscreen.h
  src/mk2/spritecachingreader.h
  src/mk2/spritedynamicreader.h
  src/mk2/spriteplayer.h
  src/mk2/spritereader.h
  src/mk2/spritereadersynchronizer.h
  src/mk2/spriteseekingreader.h
  src/mk2/spriteviewer.h

  src/modules/managers/notify_manager.h

  src/modules/managers/scene_manager.h
  src/modules/theme/thememanager.h
  src/theme.h
  src/utils.h
  src/version.h
)

set(SOURCES
  src/engine/animation/keyframe_channel.cpp
  src/engine/animation/keyframe_sequence.cpp
  src/engine/fs/fs_mounting.cpp
  src/engine/fs/fs_reading.cpp
  src/engine/fs/fs_writing.cpp
  src/engine/interface/courtroom_layout.cpp
  src/engine/interface/lobby_layout.cpp
  src/engine/interface/menus/area_menu.cpp
  src/engine/interface/menus/bgm_menu.cpp
  src/engine/interface/menus/char_menu.cpp
  src/engine/interface/menus/emote_menu.cpp
  src/engine/interface/scenes/replay_window.cpp
  src/engine/interface/widgets/bgm_filter.cpp
  src/engine/interface/widgets/char_button.cpp
  src/engine/interface/widgets/character_select.cpp
  src/engine/interface/widgets/chat_log.cpp
  src/engine/interface/widgets/emotion_button.cpp
  src/engine/interface/widgets/health_bar.cpp
  src/engine/interface/widgets/image_display.cpp
  src/engine/interface/widgets/note_area.cpp
  src/engine/interface/widgets/note_picker.cpp
  src/engine/interface/widgets/player_list_slot.cpp
  src/engine/interface/widgets/rp_button.cpp
  src/engine/interface/widgets/rp_combo_box.cpp
  src/engine/interface/widgets/rp_hover_widget.cpp
  src/engine/interface/widgets/rp_label.cpp
  src/engine/interface/widgets/rp_line_edit.cpp
  src/engine/interface/widgets/rp_slider.cpp
  src/engine/interface/widgets/rp_text_edit.cpp
  src/engine/interface/widgets/rp_typewriter.cpp
  src/engine/interface/widgets/rp_widget.cpp
  src/engine/interface/widgets/screenshot_button.cpp
  src/engine/interface/widgets/emotion_selector.cpp
  src/engine/interface/widgets/choice_dialog.cpp
  src/engine/interface/widgets/sticker_viewer.cpp
  src/engine/interface/widgets/viewport_overlay.cpp
  src/engine/interface/widgets/tab_toggle_button.cpp
  src/engine/interface/widgets/aotimer.cpp
  src/engine/interface/widgets/viewports/legacy_viewport.cpp
  src/engine/interface/widgets/viewports/rp_viewport.cpp
  src/engine/network/json_packet.cpp
  src/engine/network/metadata/area_metadata.cpp
  src/engine/network/metadata/message_metadata.cpp
  src/engine/network/metadata/user_metadata.cpp
  src/engine/network/metadata/server_metadata.cpp
  src/engine/network/metadata/tracklist_metadata.cpp
  src/engine/param/animation_reader.cpp
  src/engine/param/replay_reader.cpp
  src/engine/system/animation.cpp
  src/engine/system/audio.cpp
  src/engine/system/audio/ambience_player.cpp
  src/engine/system/audio/blip_player.cpp
  src/engine/system/audio/music_player.cpp
  src/engine/system/audio/sfx_player.cpp
  src/engine/system/audio/shout_player.cpp
  src/engine/system/audio/system_player.cpp
  src/engine/system/effects.cpp
  src/engine/system/localization.cpp
  src/engine/system/replay_playback.cpp
  src/engine/system/runtime_loop.cpp
  src/engine/system/text_encoding.cpp
  src/engine/system/theme_scripting.cpp
  src/engine/system/debug/time_debugger.cpp
  src/engine/themes/theme_scene.cpp
  src/engine/tools/button_maker.cpp
  src/engine/param/json_reader.cpp
  src/engine/param/localization_reader.cpp
  src/engine/param/theme_mode_reader.cpp
  src/engine/param/theme_module_reader.cpp
  src/engine/param/theme_reader.cpp
  src/engine/param/actor/actor_loader.cpp

  src/engine/param/background/legacy_background_reader.cpp
  src/aoapplication.cpp
  src/aoconfig.cpp
  src/aoconfigpanel.cpp
  src/aoguiloader.cpp
  src/aoobject.cpp
  src/aopixmap.cpp
  src/charselect.cpp
  src/commondefs.cpp
  src/courtroom.cpp
  src/courtroom_character.cpp
  src/courtroom_sfx.cpp
  src/courtroom_widgets.cpp
  src/datatypes.cpp
  src/debug_functions.cpp
  src/draudio.cpp
  src/draudiodevice.cpp
  src/draudioengine.cpp
  src/draudioengine_p.cpp
  src/draudioerror.cpp
  src/draudiostream.cpp
  src/draudiostreamfamily.cpp
  src/draudiotrackmetadata.cpp
  src/drcharactermovie.cpp
  src/dreffectmovie.cpp
  src/drgraphicscene.cpp
  src/drmasterclient.cpp
  src/drmediatester.cpp
  src/drmovie.cpp
  src/drpacket.cpp
  src/drposition.cpp
  src/drscenemovie.cpp
  src/drserverinfoeditor.cpp
  src/drserversocket.cpp
  src/drshoutmovie.cpp
  src/drsplashmovie.cpp

  src/drtheme.cpp
  src/drthememovie.cpp
  src/emotes.cpp
  src/hardware_functions.cpp
  src/lobby.cpp
  src/logger.cpp
  src/main.cpp
  src/mk2/drplayer.cpp
  src/mk2/graphicsspriteitem.cpp
  src/mk2/graphicsvideoscreen.cpp
  src/mk2/spritecachingreader.cpp
  src/mk2/spritedynamicreader.cpp
  src/mk2/spriteplayer.cpp
  src/mk2/spriteseekingreader.cpp

  src/modules/managers/notify_manager.cpp

  src/modules/managers/scene_manager.cpp
  src/modules/theme/thememanager.cpp
  src/path_functions.cpp
  src/server_socket.cpp
  src/mk2/spritereader.cpp
  src/mk2/spritereadersynchronizer.cpp
  src/mk2/spriteviewer.cpp
  src/text_file_functions.cpp
  src/theme.cpp
  src/utils.cpp
  src/version.cpp
)

set(RESOURCES
  res.qrc
)

add_executable(rp-engine
  ${HEADERS}
  ${SOURCES}
  ${RESOURCES}
  src/engine/param/actor_repository.h src/engine/param/actor_repository.cpp
  src/engine/system/runtime_values.h src/engine/system/runtime_values.cpp
  src/engine/system/theme.h src/engine/system/theme.cpp
  src/engine/interface/widgets/scene_widget.h src/engine/interface/widgets/scene_widget.cpp
  src/engine/interface/widgets/rp_list_widget.h src/engine/interface/widgets/rp_list_widget.cpp
  src/engine/interface/widgets/rp_message_input.h src/engine/interface/widgets/rp_message_input.cpp
  src/engine/encoding/binary_encoding.h src/engine/encoding/binary_encoding.cpp
  src/pch.h
  src/engine/fs/fs_characters.h src/engine/fs/fs_characters.cpp
  src/rolechat_config.h src/rolechat_config.cpp src/rolechat_config.ui
  src/config_tabs/config_tab_theme.h src/config_tabs/config_tab_theme.cpp src/config_tabs/config_tab_theme.ui
  src/engine/system/config_manager.h src/engine/system/config_manager.cpp
  src/engine/interface/widgets/workshop_list.h src/engine/interface/widgets/workshop_list.cpp
  src/engine/interface/widgets/workshop_entry.h src/engine/interface/widgets/workshop_entry.cpp
  src/engine/interface/scenes/downloader_prompt.h src/engine/interface/scenes/downloader_prompt.cpp
  src/engine/interface/scenes/workshop_uploader.h src/engine/interface/scenes/workshop_uploader.cpp
  src/engine/workshop/workshop_cache.h src/engine/workshop/workshop_cache.cpp
  src/engine/discord/workshop_discord.h src/engine/discord/workshop_discord.cpp
  src/engine/animation/widget_animator.h src/engine/animation/widget_animator.cpp
  src/engine/interface/widgets/serverselectlist.h src/engine/interface/widgets/serverselectlist.cpp
  src/engine/interface/widgets/server_select_entry.h src/engine/interface/widgets/server_select_entry.cpp
  src/engine/interface/widgets/icon_grid_widget.h src/engine/interface/widgets/icon_grid_widget.cpp
  src/engine/interface/menus/screenshot_menu.h src/engine/interface/menus/screenshot_menu.cpp
  src/engine/system/user_database.h src/engine/system/user_database.cpp

)
target_precompile_headers(rp-engine PRIVATE src/pch.h)

target_include_directories(rp-engine PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua
)

set_target_properties(rp-engine PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
        RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
)

target_link_directories(rp-engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/bass
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/discord_social_sdk
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua
)

add_subdirectory(3rd/rolechat-lib)

target_link_libraries(rp-engine
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::UiTools
  Qt5::Network
  Qt5::Multimedia
  Qt5::MultimediaWidgets
  bass
  bassopus
  bass_fx
  discord_partner_sdk
  lua54
  rolechat-lib
)

if(WIN32)
    set_target_properties(rp-engine PROPERTIES WIN32_EXECUTABLE TRUE)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/icons/logo-client.rc")
    target_sources(rp-engine PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()
