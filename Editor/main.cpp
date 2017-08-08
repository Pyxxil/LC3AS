
#include <QtWidgets>

#include "Editor/Includes/Editor.hpp"

int
main(int argc, char** argv)
{
  QApplication app(argc, argv);
  Editor editor;
  editor.setWindowTitle(QObject::tr("Editor Example"));
  editor.show();

  return app.exec();
}