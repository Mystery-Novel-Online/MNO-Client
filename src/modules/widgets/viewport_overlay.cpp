#include "viewport_overlay.h"
#include "drtheme.h"
#include <AOApplication.h>
#include <QMenu>
#include <aoimagedisplay.h>
#include <modules/theme/thememanager.h>

ViewportOverlay::ViewportOverlay(QWidget *parent)
    : QWidget{parent}
{

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &ViewportOverlay::displayContextMenu);
}

QAction *ViewportOverlay::createResizeAction(int t_width, int t_height)
{
  QAction *l_resizeAction = new QAction(QString::number(t_width) + " x " + QString::number(t_height));
  QObject::connect(l_resizeAction, &QAction::triggered, [this, t_width, t_height](){resizeViewport(t_width, t_height);});
  return l_resizeAction;
}

void ViewportOverlay::resizeWidgetAndChildren(QString t_widget, int t_width, int t_height)
{
  
  QWidget* l_widget = ThemeManager::get().getWidget(t_widget);
  if(l_widget == nullptr) return;

  int l_viewportNativeHeight = ThemeManager::get().mCurrentThemeReader.getWidgetPosition(COURTROOM, "viewport").height;
  pos_size_type l_widgetSize = ThemeManager::get().mCurrentThemeReader.getWidgetPosition(COURTROOM, t_widget);

  double l_scalingFactor = static_cast<double>(t_height) / l_viewportNativeHeight;

  int l_scaledWidth = static_cast<int>(l_widgetSize.width * l_scalingFactor);
  int l_scaledHeight = static_cast<int>(l_widgetSize.height * l_scalingFactor);
  int l_scaledX = static_cast<int>(l_widgetSize.x * l_scalingFactor);
  int l_scaledY = static_cast<int>(l_widgetSize.y * l_scalingFactor);

  ThemeManager::get().setViewporResize(l_scalingFactor);

  if(t_widget.startsWith("viewport"))
  {
    l_scaledX = 0;
    l_scaledY = 0;
    l_scaledWidth = t_width;
    l_scaledHeight = t_height;
  }

  l_widget->resize(l_scaledWidth, l_scaledHeight);
  l_widget->move(l_scaledX, l_scaledY);

  if (dynamic_cast<AOImageDisplay*>(l_widget) != nullptr)
  {
    AOImageDisplay* l_imageDisplay = dynamic_cast<AOImageDisplay*>(l_widget);
    l_imageDisplay->refreshImage();
  }

  QStringList l_viewportChildren = ThemeManager::get().mCurrentThemeReader.getLayerChildren(t_widget);
  for(QString l_child : l_viewportChildren)
  {
    resizeWidgetAndChildren(l_child, t_width, t_height);
  }

}

void ViewportOverlay::displayContextMenu(QPoint t_position)
{
  if(AOApplication::getInstance()->current_theme->read_config_bool("detatchable_viewport"))
  {
    QMenu *l_menu = new QMenu(this);

    //Create the detachWindowAction
    QAction *l_detachWindowAction = new QAction("Detach from Window");
    QObject::connect(l_detachWindowAction, &QAction::triggered, [this](){detatchViewport();});
    l_menu->addAction(l_detachWindowAction);


    //Create the resize Actions
    QMenu *l_scaleMenu = l_menu->addMenu("Scale (1.76:1)");

    l_scaleMenu->addAction(createResizeAction(480, 272));
    l_scaleMenu->addAction(createResizeAction(960, 544));
    l_scaleMenu->addAction(createResizeAction(1280, 725));
    l_scaleMenu->addAction(createResizeAction(1440, 816));
    l_scaleMenu->addAction(createResizeAction(1920, 1088));

    l_menu->popup(this->mapToGlobal(t_position));
  }
}

void ViewportOverlay::detatchViewport()
{
  QWidget* l_viewport = ThemeManager::get().getWidget("viewport");

  l_viewport->setParent(nullptr);
  l_viewport->show();
  l_viewport->move(20, 20);
}

void ViewportOverlay::resizeViewport(int t_width, int t_height)
{
  QWidget* l_viewport = ThemeManager::get().getWidget("viewport");
  l_viewport->resize(t_width, t_height);

  QStringList l_viewportChildren = ThemeManager::get().mCurrentThemeReader.getLayerChildren("viewport");
  for(QString l_child : l_viewportChildren)
  {
    resizeWidgetAndChildren(l_child, t_width, t_height);
  }

}
