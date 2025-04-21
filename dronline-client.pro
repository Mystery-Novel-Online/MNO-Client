QT += core gui widgets uitools network multimedia multimediawidgets

CONFIG += c++17

TEMPLATE = app
VERSION = 1.6.0
TARGET = dro-client

RC_ICONS = icon.ico

INCLUDEPATH += $$PWD/include $$PWD/src $$PWD/3rd
DEPENDPATH += $$PWD/include $$PWD/src $$PWD/3rd

HEADERS += \
  src/aoapplication.h \
  src/aoblipplayer.h \
  src/aobutton.h \
  src/aocharbutton.h \
  src/aoconfig.h \
  src/aoconfigpanel.h \
  src/aoemotebutton.h \
  src/aoguiloader.h \
  src/aoimagedisplay.h \
  src/aolabel.h \
  src/aolineedit.h \
  src/aomusicplayer.h \
  src/aonotearea.h \
  src/aonotepad.h \
  src/aonotepicker.h \
  src/aoobject.h \
  src/aopixmap.h \
  src/aosfxplayer.h \
  src/aoshoutplayer.h \
  src/aosystemplayer.h \
  src/aotimer.h \
  src/commondefs.h \
  src/courtroom.h \
  src/datatypes.h \
  src/debug_functions.h \
  src/draudio.h \
  src/draudiodevice.h \
  src/draudioengine.h \
  src/draudioengine_p.h \
  src/draudioerror.h \
  src/draudiostream.h \
  src/draudiostreamfamily.h \
  src/draudiotrackmetadata.h \
  src/drcharactermovie.h \
  src/drchatlog.h \
  src/drdiscord.h \
  src/dreffectmovie.h \
  src/drgraphicscene.h \
  src/drmasterclient.h \
  src/drmediatester.h \
  src/drmovie.h \
  src/drpacket.h \
  src/drpather.h \
  src/drplayerlistentry.h \
  src/drposition.h \
  src/drscenemovie.h \
  src/drserverinfoeditor.h \
  src/drserversocket.h \
  src/drshoutmovie.h \
  src/drsplashmovie.h \
  src/drstickerviewer.h \
  src/drtextedit.h \
  src/drtheme.h \
  src/drthememovie.h \
  src/file_functions.h \
  src/hardware_functions.h \
  src/lobby.h \
  src/logger.h \
  src/misc_functions.h \
  src/mk2/drplayer.h \
  src/mk2/graphicsspriteitem.h \
  src/mk2/graphicsvideoscreen.h \
  src/mk2/spritecachingreader.h \
  src/mk2/spritedynamicreader.h \
  src/mk2/spriteplayer.h \
  src/mk2/spritereader.h \
  src/mk2/spritereadersynchronizer.h \
  src/mk2/spriteseekingreader.h \
  src/mk2/spriteviewer.h \
  src/modules/background/background_data.h \
  src/modules/background/background_reader.h \
  src/modules/background/legacy_background_reader.h \
  src/modules/character/character_data.h \
  src/modules/character/character_data_reader.h \
  src/modules/character/legacy_character_reader.h \
  src/modules/character/outfit_reader.h \
  src/modules/debug/time_debugger.h \
  src/modules/files/image_loader.h \
  src/modules/json/json_reader.h \
  src/modules/json/localization_reader.h \
  src/modules/json/theme_mode_reader.h \
  src/modules/json/theme_module_reader.h \
  src/modules/json/theme_reader.h \
  src/modules/managers/character_manager.h \
  src/modules/managers/emotion_manager.h \
  src/modules/managers/localization_manager.h \
  src/modules/managers/notify_manager.h \
  src/modules/managers/pair_manager.h \
  src/modules/managers/scene_manager.h \
  src/modules/networking/json_packet.h \
  src/modules/theme/theme_scene.h \
  src/modules/theme/thememanager.h \
  src/modules/theme/widgets/characterselectwidget.h \
  src/modules/theme/widgets/dro_combo_box.h \
  src/modules/theme/widgets/dro_line_edit.h \
  src/modules/theme/widgets/droemotebuttons.h \
  src/modules/theme/widgets/tab_toggle_button.h \
  src/modules/widgets/rpnotifymenu.h \
  src/modules/widgets/tabgroupingwidget.h \
  src/modules/widgets/viewport_overlay.h \
  src/theme.h \
  src/utils.h \
  src/version.h

SOURCES += \
  src/aoapplication.cpp \
  src/aoblipplayer.cpp \
  src/aobutton.cpp \
  src/aocharbutton.cpp \
  src/aoconfig.cpp \
  src/aoconfigpanel.cpp \
  src/aoemotebutton.cpp \
  src/aoguiloader.cpp \
  src/aoimagedisplay.cpp \
  src/aolabel.cpp \
  src/aolineedit.cpp \
  src/aomusicplayer.cpp \
  src/aonotearea.cpp \
  src/aonotepad.cpp \
  src/aonotepicker.cpp \
  src/aoobject.cpp \
  src/aopixmap.cpp \
  src/aosfxplayer.cpp \
  src/aoshoutplayer.cpp \
  src/aosystemplayer.cpp \
  src/aotimer.cpp \
  src/audio_functions.cpp \
  src/charselect.cpp \
  src/commondefs.cpp \
  src/courtroom.cpp \
  src/courtroom_character.cpp \
  src/courtroom_sfx.cpp \
  src/courtroom_widgets.cpp \
  src/datatypes.cpp \
  src/debug_functions.cpp \
  src/draudio.cpp \
  src/draudiodevice.cpp \
  src/draudioengine.cpp \
  src/draudioengine_p.cpp \
  src/draudioerror.cpp \
  src/draudiostream.cpp \
  src/draudiostreamfamily.cpp \
  src/draudiotrackmetadata.cpp \
  src/drcharactermovie.cpp \
  src/drchatlog.cpp \
  src/dreffectmovie.cpp \
  src/drgraphicscene.cpp \
  src/drmasterclient.cpp \
  src/drmediatester.cpp \
  src/drmovie.cpp \
  src/drpacket.cpp \
  src/drpather.cpp \
  src/drplayerlistentry.cpp \
  src/drposition.cpp \
  src/drscenemovie.cpp \
  src/drserverinfoeditor.cpp \
  src/drserversocket.cpp \
  src/drshoutmovie.cpp \
  src/drsplashmovie.cpp \
  src/drstickerviewer.cpp \
  src/drtextedit.cpp \
  src/drdiscord.cpp \
  src/drtheme.cpp \
  src/drthememovie.cpp \
  src/emotes.cpp \
  src/file_functions.cpp \
  src/hardware_functions.cpp \
  src/lobby.cpp \
  src/logger.cpp \
  src/main.cpp \
  src/misc_functions.cpp \
  src/mk2/drplayer.cpp \
  src/mk2/graphicsspriteitem.cpp \
  src/mk2/graphicsvideoscreen.cpp \
  src/mk2/spritecachingreader.cpp \
  src/mk2/spritedynamicreader.cpp \
  src/mk2/spriteplayer.cpp \
  src/mk2/spriteseekingreader.cpp \
  src/modules/background/background_data.cpp \
  src/modules/background/background_reader.cpp \
  src/modules/background/legacy_background_reader.cpp \
  src/modules/character/character_data.cpp \
  src/modules/character/character_data_reader.cpp \
  src/modules/character/legacy_character_reader.cpp \
  src/modules/character/outfit_reader.cpp \
  src/modules/debug/time_debugger.cpp \
  src/modules/files/image_loader.cpp \
  src/modules/json/json_reader.cpp \
  src/modules/json/localization_reader.cpp \
  src/modules/json/theme_mode_reader.cpp \
  src/modules/json/theme_module_reader.cpp \
  src/modules/json/theme_reader.cpp \
  src/modules/managers/character_manager.cpp \
  src/modules/managers/emotion_manager.cpp \
  src/modules/managers/localization_manager.cpp \
  src/modules/managers/notify_manager.cpp \
  src/modules/managers/pair_manager.cpp \
  src/modules/managers/scene_manager.cpp \
  src/modules/networking/json_packet.cpp \
  src/modules/theme/theme_scene.cpp \
  src/modules/theme/thememanager.cpp \
  src/modules/theme/widgets/characterselectwidget.cpp \
  src/modules/theme/widgets/dro_combo_box.cpp \
  src/modules/theme/widgets/dro_line_edit.cpp \
  src/modules/theme/widgets/droemotebuttons.cpp \
  src/modules/theme/widgets/tab_toggle_button.cpp \
  src/modules/widgets/rpnotifymenu.cpp \
  src/modules/widgets/tabgroupingwidget.cpp \
  src/modules/widgets/viewport_overlay.cpp \
  src/path_functions.cpp \
  src/server_socket.cpp \
  src/mk2/spritereader.cpp \
  src/mk2/spritereadersynchronizer.cpp \
  src/mk2/spriteviewer.cpp \
  src/text_file_functions.cpp \
  src/theme.cpp \
  src/utils.cpp \
  src/version.cpp

# 1. You need to get BASS and put the x86 bass DLL/headers in the project root folder
#    AND the compilation output folder. If you want a static link, you'll probably
#    need the .lib file too. MinGW-GCC is really finicky finding BASS, it seems.
# 2. You need to compile the Discord Rich Presence SDK separately and add the lib/headers
#    in the same way as BASS. Discord RPC uses CMake, which does not play nicely with
#    QMake, so this step must be manual.
LIBS += -L$$PWD/3rd -lbass -lbassopus -ldiscord-rpc

RESOURCES += \
  res.qrc

DISTFILES +=

FORMS += \
  res/ui/config_panel.ui \
  src/drserverinfoeditor.ui

# Mac stuff
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
ICON = icon.icns
