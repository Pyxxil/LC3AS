
#include <QtWidgets>

#include "MainWindow.hpp"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  QCommandLineParser parser;
  parser.setApplicationDescription("An editor for the LC3 Assembly Language");
  parser.addHelpOption();
  parser.addPositionalArgument("file", "Open this file on startup");

  parser.process(app);

  QStringList positionalArgs = parser.positionalArguments();
  if (positionalArgs.size() > 1) {
    qDebug() << "Can't open more than one file at the moment...";
  } else if (positionalArgs.size() == 1) {
    window.openFile(positionalArgs[0]);
  }

  return app.exec();
}
