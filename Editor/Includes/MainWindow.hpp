#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Editor.hpp"
#include "SyntaxHighlighter.hpp"

#include <QMainWindow>

class QTextEdit;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0);

public slots:
  void about();
  void newFile();
  void save();
  void saveAs(const QString& path = QString());
  void openFile(const QString& path = QString());

private:
  void setupEditor();
  void setupMenus();
  void setupFileMenu();
  void setupHelpMenu();

  Editor* editor;
  SyntaxHighlighter* highlighter;
};

#endif // MAINWINDOW_H