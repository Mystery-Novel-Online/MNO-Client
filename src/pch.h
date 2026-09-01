#ifndef PCH_H
#define PCH_H

#include <glad/glad.h>

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

#include "system/config_manager.h"

#include "aoapplication.h"

#include "widgets/sticker_viewer.h"
#include "widgets/rp_widget.h"
#include "widgets/rp_label.h"
#include "widgets/chat_log.h"
#include "widgets/image_display.h"
#include "widgets/rp_button.h"
#include "widgets/courtroom/AreaPlayerEntry.h"
#include "widgets/rp_combo_box.h"
#include "widgets/rp_line_edit.h"
#include "widgets/rp_hover_widget.h"
#include "widgets/bgm_filter.h"
#include "widgets/rp_list_widget.h"
#include "widgets/rp_message_input.h"
#include "widgets/screenshot_button.h"
#include "widgets/health_bar.h"
#include "widgets/rp_slider.h"

//Text Edit Widgets
#include "widgets/rp_text_edit.h"
#include "widgets/rp_typewriter.h"
#include "widgets/aotimer.h"

#include "widgets/emotion_selector.h"
#include "widgets/choice_dialog.h"
#include "widgets/viewport_overlay.h"
#include "widgets/char_button.h"
#include "widgets/character_select.h"
#include "widgets/emotion_button.h"
#include "widgets/tab_toggle_button.h"
#include "widgets/viewports/legacy_viewport.h"

#include "courtroom.h"
#include "drgraphicscene.h"
#include "drcharactermovie.h"

using namespace rolechat;

#endif // PCH_H
