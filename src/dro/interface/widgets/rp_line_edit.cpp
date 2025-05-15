#include "rp_line_edit.h"
#include "commondefs.h"

QHash<QString, QString> s_defaultLineEditCSS
{
    {"message", "background-color: rgba(0, 0, 0, 0); color: white"},
    {"ooc_chat_name", "background-color: rgba(100, 100, 100, 255);"},
    {"ooc_chat_message", "background-color: rgba(100, 100, 100, 255);"},
    {"ic_chat_name", "background-color: rgba(100, 100, 100, 255);"}
};


DROLineEdit::DROLineEdit(const QString &name, const QString &placeholder, const QString &cssHeader, QWidget *parent) : QLineEdit(parent), m_app(AOApplication::getInstance()), m_friendlyName(name), m_cssHeader(cssHeader), m_targetScene("courtroom")
{
  if(s_defaultLineEditCSS.contains(m_friendlyName)) setDefaultCSS(s_defaultLineEditCSS[m_friendlyName]);
  setPlaceholderText(placeholder);
}

DROLineEdit::DROLineEdit(const QString &name, const QString &cssHeader, QWidget *parent) : QLineEdit(parent), m_app(AOApplication::getInstance()), m_friendlyName(name), m_cssHeader(cssHeader), m_targetScene("courtroom")
{
  if(s_defaultLineEditCSS.contains(m_friendlyName)) setDefaultCSS(s_defaultLineEditCSS[m_friendlyName]);
}

void DROLineEdit::set_theme_settings(QString name, QString cssHeader, QString scene)
{
  m_cssHeader = cssHeader;
  m_friendlyName = name;
  m_targetScene = scene;
}

void DROLineEdit::setDefaultCSS(QString css)
{
  m_fallbackCSS = css;
}

void DROLineEdit::refreshPosition()
{
  set_size_and_pos(this, m_friendlyName, COURTROOM_DESIGN_INI, m_app);
  set_text_alignment(this, m_friendlyName, COURTROOM_FONTS_INI, m_app);
}

void DROLineEdit::refreshCSS()
{
  if (!set_stylesheet(this, "[" + m_friendlyName + "]", COURTROOM_STYLESHEETS_CSS, m_app))
  {
    if (!set_stylesheet(this, m_cssHeader, COURTROOM_STYLESHEETS_CSS, m_app))
    {
      if(!m_fallbackCSS.isEmpty()) this->setStyleSheet(m_fallbackCSS);
    }
  }
}
