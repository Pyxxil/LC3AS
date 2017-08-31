#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <QObject>
#include <QPlainTextEdit>

class QProcess;

class Editor;

class Console : public QPlainTextEdit
{
  Q_OBJECT

public:
  Console(QWidget* parent = 0);

public slots:
  void run(Editor* editor);
  void updateOutput();

signals:
  bool saveFirst();

private:
  QProcess* process;
};

#endif
