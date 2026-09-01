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
  src/animation/keyframe_channel.h
  src/animation/keyframe_sequence.h
  src/fs/fs_mounting.h
  src/fs/fs_reading.h
  src/fs/fs_writing.h
  src/interface/courtroom_layout.h
  src/interface/lobby_layout.h
  src/interface/menus/area_menu.h
  src/interface/menus/bgm_menu.h
  src/interface/menus/char_menu.h
  src/interface/menus/emote_menu.h
  src/interface/scenes/replay_window.h
  src/interface/widgets/bgm_filter.h
  src/interface/widgets/char_button.h
  src/interface/widgets/character_select.h
  src/interface/widgets/chat_log.h
  src/interface/widgets/emotion_button.h
  src/interface/widgets/health_bar.h
  src/interface/widgets/image_display.h


  src/interface/widgets/rp_button.h
  src/interface/widgets/rp_combo_box.h
  src/interface/widgets/rp_hover_widget.h
  src/interface/widgets/rp_label.h
  src/interface/widgets/rp_line_edit.h
  src/interface/widgets/rp_slider.h
  src/interface/widgets/rp_text_edit.h
  src/interface/widgets/rp_typewriter.h
  src/interface/widgets/rp_widget.h
  src/interface/widgets/screenshot_button.h
  src/interface/widgets/emotion_selector.h
  src/interface/widgets/choice_dialog.h
  src/interface/widgets/sticker_viewer.h
  src/interface/widgets/viewport_overlay.h
  src/interface/widgets/tab_toggle_button.h
  src/interface/widgets/aotimer.h
  src/interface/widgets/viewports/legacy_viewport.h
  src/interface/widgets/viewports/rp_viewport.h
  src/network/json_packet.h
  src/network/metadata/area_metadata.h
  src/network/metadata/message_metadata.h
  src/network/metadata/user_metadata.h
  src/network/metadata/server_metadata.h
  src/network/metadata/tracklist_metadata.h
  src/param/animation_reader.h
  src/param/replay_reader.h
  src/system/animation.h
  src/system/audio.h
  src/system/audio/ambience_player.h
  src/system/audio/blip_player.h
  src/system/audio/music_player.h
  src/system/audio/sfx_player.h
  src/system/audio/shout_player.h
  src/system/audio/system_player.h
  src/system/effects.h
  src/system/localization.h
  src/system/replay_playback.h
  src/system/runtime_loop.h
  src/system/text_encoding.h
  src/system/theme_scripting.h
  src/system/debug/time_debugger.h
  src/themes/theme_scene.h
  src/tools/button_maker.h
  src/param/json_reader.h
  src/param/localization_reader.h
  src/param/theme_mode_reader.h
  src/param/theme_module_reader.h
  src/param/theme_reader.h
  src/param/actor/actor_loader.h


  src/param/background/legacy_background_reader.h
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
  src/modules/theme/legacythememanager.h
  src/theme.h
  src/utils.h
  src/version.h
)

set(SOURCES
  src/animation/keyframe_channel.cpp
  src/animation/keyframe_sequence.cpp
  src/fs/fs_mounting.cpp
  src/fs/fs_reading.cpp
  src/fs/fs_writing.cpp
  src/interface/courtroom_layout.cpp
  src/interface/lobby_layout.cpp
  src/interface/menus/area_menu.cpp
  src/interface/menus/bgm_menu.cpp
  src/interface/menus/char_menu.cpp
  src/interface/menus/emote_menu.cpp
  src/interface/scenes/replay_window.cpp
  src/interface/widgets/bgm_filter.cpp
  src/interface/widgets/char_button.cpp
  src/interface/widgets/character_select.cpp
  src/interface/widgets/chat_log.cpp
  src/interface/widgets/emotion_button.cpp
  src/interface/widgets/health_bar.cpp
  src/interface/widgets/image_display.cpp


  src/interface/widgets/rp_button.cpp
  src/interface/widgets/rp_combo_box.cpp
  src/interface/widgets/rp_hover_widget.cpp
  src/interface/widgets/rp_label.cpp
  src/interface/widgets/rp_line_edit.cpp
  src/interface/widgets/rp_slider.cpp
  src/interface/widgets/rp_text_edit.cpp
  src/interface/widgets/rp_typewriter.cpp
  src/interface/widgets/rp_widget.cpp
  src/interface/widgets/screenshot_button.cpp
  src/interface/widgets/emotion_selector.cpp
  src/interface/widgets/choice_dialog.cpp
  src/interface/widgets/sticker_viewer.cpp
  src/interface/widgets/viewport_overlay.cpp
  src/interface/widgets/tab_toggle_button.cpp
  src/interface/widgets/aotimer.cpp
  src/interface/widgets/viewports/legacy_viewport.cpp
  src/interface/widgets/viewports/rp_viewport.cpp
  src/network/json_packet.cpp
  src/network/metadata/area_metadata.cpp
  src/network/metadata/message_metadata.cpp
  src/network/metadata/user_metadata.cpp
  src/network/metadata/server_metadata.cpp
  src/network/metadata/tracklist_metadata.cpp
  src/param/animation_reader.cpp
  src/param/replay_reader.cpp
  src/system/animation.cpp
  src/system/audio.cpp
  src/system/audio/ambience_player.cpp
  src/system/audio/blip_player.cpp
  src/system/audio/music_player.cpp
  src/system/audio/sfx_player.cpp
  src/system/audio/shout_player.cpp
  src/system/audio/system_player.cpp
  src/system/effects.cpp
  src/system/localization.cpp
  src/system/replay_playback.cpp
  src/system/runtime_loop.cpp
  src/system/text_encoding.cpp
  src/system/theme_scripting.cpp
  src/system/debug/time_debugger.cpp
  src/themes/theme_scene.cpp
  src/tools/button_maker.cpp
  src/param/json_reader.cpp
  src/param/localization_reader.cpp
  src/param/theme_mode_reader.cpp
  src/param/theme_module_reader.cpp
  src/param/theme_reader.cpp
  src/param/actor/actor_loader.cpp

  src/param/background/legacy_background_reader.cpp
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
  src/modules/theme/legacythememanager.cpp
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
  src/param/actor_repository.h src/param/actor_repository.cpp
  src/system/runtime_values.h src/system/runtime_values.cpp
  src/system/theme.h src/system/theme.cpp
  src/interface/widgets/scene_widget.h src/interface/widgets/scene_widget.cpp
  src/interface/widgets/rp_list_widget.h src/interface/widgets/rp_list_widget.cpp
  src/interface/widgets/rp_message_input.h src/interface/widgets/rp_message_input.cpp
  src/encoding/binary_encoding.h src/encoding/binary_encoding.cpp
  src/pch.h
  src/fs/fs_characters.h src/fs/fs_characters.cpp
  src/rolechat_config.h src/rolechat_config.cpp src/rolechat_config.ui
  src/config_tabs/config_tab_theme.h src/config_tabs/config_tab_theme.cpp src/config_tabs/config_tab_theme.ui
  src/system/config_manager.h src/system/config_manager.cpp
  src/widgets/workshop/WorkshopListWidget.h src/widgets/workshop/WorkshopListWidget.cpp
  src/widgets/workshop/WorkshopEntry.h src/widgets/workshop/WorkshopEntry.cpp
  src/interface/scenes/downloader_prompt.h src/interface/scenes/downloader_prompt.cpp
  src/interface/scenes/workshop_uploader.h src/interface/scenes/workshop_uploader.cpp
  src/workshop/workshop_cache.h src/workshop/workshop_cache.cpp
  src/discord/workshop_discord.h src/discord/workshop_discord.cpp
  src/animation/widget_animator.h src/animation/widget_animator.cpp
  src/widgets/lobby/ServerSelectList.h src/widgets/lobby/ServerSelectList.cpp
  src/widgets/lobby/ServerSelectEntry.h src/widgets/lobby/ServerSelectEntry.cpp
  src/interface/widgets/icon_grid_widget.h src/interface/widgets/icon_grid_widget.cpp
  src/interface/menus/screenshot_menu.h src/interface/menus/screenshot_menu.cpp
  src/system/user_database.h src/system/user_database.cpp
  src/widgets/courtroom/LayerSelectionPanel.h src/widgets/courtroom/LayerSelectionPanel.cpp
  src/network/api_manager.h src/network/api_manager.cpp
  src/system/audio/loop_detection.h src/system/audio/loop_detection.cpp
  src/system/client_worker.h src/system/client_worker.cpp
  src/interface/scenes/collection_creation.h src/interface/scenes/collection_creation.cpp
  src/config_tabs/config_tab_mounting.h src/config_tabs/config_tab_mounting.cpp src/config_tabs/config_tab_mounting.ui
  src/widgets/workshop/WorkshopTags.h src/widgets/workshop/WorkshopTags.cpp
  src/widgets/workshop/UploaderTagTable.h src/widgets/workshop/UploaderTagTable.cpp
  src/config_tabs/config_tab_blips.h src/config_tabs/config_tab_blips.cpp src/config_tabs/config_tab_blips.ui
  src/config_tabs/config_tab_callwords.h src/config_tabs/config_tab_callwords.cpp src/config_tabs/config_tab_callwords.ui
  src/utils/file_hash_util.h src/utils/file_hash_util.cpp
  src/network/workshop/workshop_parser.h src/network/workshop/workshop_parser.cpp
  src/network/workshop/workshop_models.h
  src/config_tabs/config_tab_discord.h src/config_tabs/config_tab_discord.cpp src/config_tabs/config_tab_discord.ui
  src/dialogue/DialogueCues.h
  src/dialogue/CueParser.h src/dialogue/CueParser.cpp
  src/scripting/lua/LuaSyncedVariable.h src/scripting/lua/LuaSyncedVariable.cpp
  src/networking/SyncedVariable.h src/networking/SyncedVariable.cpp
  src/dialogs/RenameContentDialog.h src/dialogs/RenameContentDialog.cpp
  src/network/workshop/WorkshopPackage.h src/network/workshop/WorkshopPackage.cpp
  src/network/workshop/ContentDownloader.h src/network/workshop/ContentDownloader.cpp
  src/interface/menus/WorkshopContextMenu.h src/interface/menus/WorkshopContextMenu.cpp
  src/interface/widgets/fennec_widget.h src/interface/widgets/fennec_widget.cpp
  src/fennec/windows/ThemeWindow.h src/fennec/windows/ThemeWindow.cpp
  src/fennec/windows/NetworkWindow.h src/fennec/windows/NetworkWindow.cpp
  src/networking/NetworkTraffic.h src/networking/NetworkTraffic.cpp
  src/fennec/windows/DebugWindow.h src/fennec/windows/DebugWindow.cpp
  src/widgets/workshop/WorkshopTagLabel.h src/widgets/workshop/WorkshopTagLabel.cpp
  src/networking/HubUploadManager.h src/networking/HubUploadManager.cpp
  src/networking/WorkshopDownloadManager.h src/networking/WorkshopDownloadManager.cpp
  src/fennec/windows/DownloadWindow.h src/fennec/windows/DownloadWindow.cpp
  src/widgets/courtroom/AreaPlayerList.h src/widgets/courtroom/AreaPlayerList.cpp
  src/widgets/courtroom/AreaPlayerEntry.h src/widgets/courtroom/AreaPlayerEntry.cpp
  src/widgets/courtroom/ActorSelection.h src/widgets/courtroom/ActorSelection.cpp
  src/dialogs/TransferOwnershipDialog.h src/dialogs/TransferOwnershipDialog.cpp

)
target_precompile_headers(rp-engine PRIVATE src/pch.h)

target_include_directories(rp-engine PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/FennEngine/engine/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/FennEngine/external/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/FennEngine/external/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/FennEngine/external/imgui/backends
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
add_subdirectory(3rd/FennEngine)

target_compile_definitions(FennEngine PRIVATE
    FENN_QT
)

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
  FennEngine
  imgui
)


if(WIN32)
    set_target_properties(rp-engine PROPERTIES WIN32_EXECUTABLE TRUE)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/icons/logo-client.rc")
    target_sources(rp-engine PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()
