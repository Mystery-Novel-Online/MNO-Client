#pragma once

// src
#include "datatypes.h"
class AOApplication;
class DRStickerViewer;
class DRTextEdit;

// qt
class QLineEdit;
class QString;
class QWidget;

void set_size_and_pos(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void set_text_alignment(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void set_text_alignment(QWidget *widget, widgetFontStruct font_data, AOApplication *ao_app);
void set_font(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void setThemeFont(QWidget *widget, widgetFontStruct font_data, AOApplication *ao_app);
void set_drtextedit_font(DRTextEdit *widget, QString identifier, QString ini_file, AOApplication *ao_app);

void setShownameFont(DRTextEdit *widget, QString identifier, QString align, AOApplication *ao_app);

bool set_stylesheet(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void center_widget_to_screen(QWidget *widget);
void set_sticker_play_once(DRStickerViewer *sticker, QString identifier, QString ini_file, AOApplication *ao_app);
