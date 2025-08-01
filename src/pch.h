#ifndef PCH_H
#define PCH_H

#include <bass/bass.h>
#include <bass/bassopus.h>
#include <bass/bass_fx.h>

#include <rolechat/dialogue/DialogueTypes.h>
#include <rolechat/actor/ActorEmote.h>
#include <rolechat/actor/ActorLayer.h>
#include <rolechat/actor/ActorScalingMode.h>
#include <rolechat/actor/ActorScalingPreset.h>
#include <rolechat/theme/ThemeHighlight.h>
#include <rolechat/theme/ThemeScene.h>
#include <rolechat/math/RPRect.h>
#include <rolechat/actor/ActorSelectEntry.h>
#include <rolechat/config/ConfigUserSettings.h>

using ActorSelectEntry = rolechat::actor::ActorSelectEntry;
using DialogueType = rolechat::dialogue::DialogueType;
using ThemeSceneType = rolechat::theme::ThemeSceneType;
using ThemeHighlight = rolechat::theme::ThemeHighlight;

#include <QGraphicsScene>
#include <QOpenGLContext>
#include <QVector3D>
#include <QGroupBox>
#include <QCompleter>
#include <QRadioButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QApplication>
#include <QString>
#include <QMainWindow>
#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsBlurEffect>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>
#include <QTime>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>
#include <QMap>
#include <QModelIndex>
#include <QQueue>
#include <QSharedPointer>
#include <QSlider>
#include <QStack>
#include <QTextCharFormat>
#include <QAction>
#include <QCollator>
#include <QFontDatabase>
#include <QIcon>
#include <QPixmap>
#include <QProgressBar>
#include <QScopedPointer>
#include <QSettings>
#include <QSignalMapper>
#include <QtMath>
#include <QHBoxLayout>
#include <QFuture>
#include <QScreen>
#include <QtConcurrent/QtConcurrent>
#include <QUrl>
#include <QDesktopServices>
#include <QClipboard>
#include <QVector>
#include <QImageReader>
#include <QProcess>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFileDialog>
#include <QMetaType>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QHelpEvent>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QGraphicsOpacityEffect>
#include <QGraphicsProxyWidget>
#include <QTextBlock>


#include <functional>
#include <optional>
#include <utility>

#include "commondefs.h"
#include "datatypes.h"

#include "engine/system/config_manager.h"

#include "aoapplication.h"

#include "engine/interface/widgets/sticker_viewer.h"
#include "engine/interface/widgets/rp_widget.h"
#include "engine/interface/widgets/rp_label.h"
#include "engine/interface/widgets/chat_log.h"
#include "engine/interface/widgets/image_display.h"
#include "engine/interface/widgets/note_area.h"
#include "engine/interface/widgets/note_picker.h"
#include "engine/interface/widgets/rp_button.h"
#include "engine/interface/widgets/player_list_slot.h"
#include "engine/interface/widgets/rp_combo_box.h"
#include "engine/interface/widgets/rp_line_edit.h"
#include "engine/interface/widgets/rp_hover_widget.h"
#include "engine/interface/widgets/bgm_filter.h"
#include "engine/interface/widgets/rp_list_widget.h"
#include "engine/interface/widgets/rp_message_input.h"
#include "engine/interface/widgets/screenshot_button.h"
#include "engine/interface/widgets/health_bar.h"
#include "engine/interface/widgets/rp_slider.h"

//Text Edit Widgets
#include "engine/interface/widgets/rp_text_edit.h"
#include "engine/interface/widgets/rp_typewriter.h"
#include "engine/interface/widgets/aotimer.h"

#include "engine/interface/widgets/emotion_selector.h"
#include "engine/interface/widgets/choice_dialog.h"
#include "engine/interface/widgets/viewport_overlay.h"
#include "engine/interface/widgets/char_button.h"
#include "engine/interface/widgets/character_select.h"
#include "engine/interface/widgets/emotion_button.h"
#include "engine/interface/widgets/tab_toggle_button.h"
#include "engine/interface/widgets/viewports/legacy_viewport.h"

#include "courtroom.h"
#include "drgraphicscene.h"
#include "drcharactermovie.h"

using namespace rolechat;

#endif // PCH_H
