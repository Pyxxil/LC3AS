#include "Console.hpp"

#include <QApplication>
#include <QDebug>
#include <QProcess>

#include "Editor.hpp"

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent), process(new QProcess) {
  connect(process, SIGNAL(readyReadStandardOutput()), this,
          SLOT(updateOutput()));

  setReadOnly(true);
}

/*! Run the assembler on the currently open file (saving it first)
 *
 * If there is no currently open file (i.e. a new file has been created and not
 * saved yet), then we will get the user to 'Save As' it first.
 */
void Console::run(Editor *editor) {
  if (editor->open_file.isEmpty()) {
    // Can't assemble a file if there isn't one open.
    if (!emit saveFirst()) {
      return;
    }
  } else if (editor->hasBeenModified()) {
    // Save the currently open file
    emit saveFirst();
  }

  // For now, this is just how it has to be ... at some point I hope to actually
  // implement the assembler into this (maybe), or allow the user to specify the
  // path to the assembler (probably more likely).
  process->start("./LC3AS", QStringList()
                                << "--no-colour" << editor->open_file);
}

void Console::updateOutput() {
  document()->setPlainText(process->readAllStandardOutput());
}
