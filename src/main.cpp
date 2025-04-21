#include "aoapplication.h"
#include "aoconfig.h"
#include "drmediatester.h"
#include "lobby.h"
#include "logger.h"
#include "version.h"

#include <QDebug>

int main(int argc, char *argv[])
{
#if defined(Q_OS_WINDOWS)
  qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "windowsmediafoundation");
#elif defined(Q_OS_MACOS)
  qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

  //qInstallMessageHandler(logger::log);
  qInfo() << "Starting Danganronpa Online...";

  bool l_dpi_scaling = false;
  for (int i = 0; i < argc; ++i)
  {
    const QString l_arg(argv[i]);

    if (l_arg == "-dpiscaling")
    {
      l_dpi_scaling = true;
    }
  }

  if (l_dpi_scaling)
  {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  }
  else
  {
    qputenv("QT_FONT_DPI", "96");
    qputenv("QT_SCALE_FACTOR", "1");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);
  }

  AOApplication app(argc, argv);

  int l_exit_code = 0;
  {
    AOConfig l_config;

    DRMediaTester *l_media_tester = new DRMediaTester(&app);
    QObject::connect(l_media_tester, SIGNAL(done()), l_media_tester, SLOT(deleteLater()));

    app.load_fonts();
    app.construct_lobby();
    app.get_lobby()->show();

    l_exit_code = app.exec();

    //logger::shutdown();

    if (l_config.autosave())
    {
      l_config.save_file();
    }
  }
  return l_exit_code;
}
