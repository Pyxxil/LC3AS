#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Console.hpp"
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
  bool save();
  bool saveAs(const QString& path = QString());
  void openFile(const QString& path = QString());

signals:
  void run();

private:
  void setupEditor();
  void setupConsole();
  void setupMenus();
  void setupFileMenu();
  void setupBuildMenu();
  void setupHelpMenu();

  void closeEvent(QCloseEvent* event) override;

  Editor* editor;
  Console* console;
  SyntaxHighlighter* highlighter;
};

#endif // MAINWINDOW_H
