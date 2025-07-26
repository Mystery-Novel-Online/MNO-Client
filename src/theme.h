#pragma once

// src
class DRStickerViewer;
class RPTextEdit;

// qt
class QLineEdit;
class QString;
class QWidget;

void set_text_alignment(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void set_text_alignment(QWidget *widget, widgetFontStruct font_data, AOApplication *ao_app);
void set_font(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app = nullptr);
void set_drtextedit_font(RPTextEdit *widget, QString identifier, QString ini_file, AOApplication *ao_app);

void setThemeFont(QWidget *widget, const widgetFontStruct& font_data);
void setShownameFont(RPTextEdit *widget, QString identifier, QString align);

bool set_stylesheet(QWidget *widget, QString identifier, QString ini_file, AOApplication *ao_app);
void center_widget_to_screen(QWidget *widget);
void set_sticker_play_once(DRStickerViewer *sticker, QString identifier, QString ini_file, AOApplication *ao_app);
