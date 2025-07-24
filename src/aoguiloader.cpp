#include "aoguiloader.h"
#include "pch.h"

AOGuiLoader::AOGuiLoader(QObject *p_parent)
    : QUiLoader(p_parent)
{}

QWidget *AOGuiLoader::load_from_file(QString p_file_path, QWidget *p_parent)
{
  QWidget *r_widget = nullptr;

  QFile l_file(p_file_path);
  if (l_file.open(QIODevice::ReadOnly))
  {
    r_widget = load(&l_file, p_parent);

    // lazily replace the parent's layout with our own
    if (p_parent != nullptr)
    {
      QVBoxLayout *l_parent_layout = new QVBoxLayout(p_parent);
      l_parent_layout->addWidget(r_widget);
      p_parent->setLayout(l_parent_layout);
    }
  }

  return r_widget;
}
