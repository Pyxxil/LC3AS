#include <QtWidgets>

#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupMenus();

  setupEditor();
  setupConsole();

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(editor);
  layout->setStretch(0, 1);
  layout->addWidget(console);
  layout->setContentsMargins(5, 5, 5, 5);

  QWidget* window = new QWidget();
  window->setLayout(layout);

  setCentralWidget(window);

  setGeometry(x(), y(), 640, 720);

  connect(this, &MainWindow::run, this, [this]() { console->run(editor); });

  setWindowTitle(tr("LC3Edit"));
}

void
MainWindow::about()
{
  QMessageBox::about(
    this,
    tr("About LC3Edit"),
    tr("<p><b>LC3Edit</b> is a simple editor for the toy assembly "
       "language that is run on the LC3 simulator</p>"));
}

void
MainWindow::setupMenus()
{
  setupFileMenu();
  setupBuildMenu();
  setupHelpMenu();
}

void
MainWindow::newFile()
{
  if (editor->hasBeenModified()) {
    QMessageBox msgBox;
    msgBox.setText("This document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Save:
        save();
      case QMessageBox::Discard:
        editor->open_file = QString();
        editor->clear();
        emit editor->modificationChanged(false);
        break;
      case QMessageBox::Cancel:
        break;
      default:
        break;
    }
  } else {
    editor->open_file = QString();
    editor->clear();
    console->clear();
  }
}

bool
MainWindow::save()
{
  if (!editor->open_file.isEmpty()) {
    QFile file(editor->open_file);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
      QTextStream out(&file);
      out << editor->document()->toPlainText();
      return true;
    }
  } else {
    return saveAs();
  }

  return false;
}

bool
MainWindow::saveAs(const QString& path)
{
  QString filename = path;

  if (filename.isNull()) {
    filename = QFileDialog::getSaveFileName(this, tr("Save File"));

    if (!filename.isEmpty()) {
      QFile file(filename);
      if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << editor->document()->toPlainText();
        editor->open_file = filename;
        return true;
      }
    }
  }

  return false;
}

void
MainWindow::openFile(const QString& path)
{
  if (editor->hasBeenModified()) {
    QMessageBox msgBox;
    msgBox.setText("This document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Save:
        save();
      case QMessageBox::Discard:
        editor->open_file = QString();
        break;
      case QMessageBox::Cancel:
        break;
      default:
        break;
    }
  }

  QString fileName = path;

  if (fileName.isNull()) {
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
  }

  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      editor->setPlainText(file.readAll());
      editor->open_file = fileName;
      console->clear();
    }
  }
}

void
MainWindow::setupEditor()
{
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(14);

  editor = new Editor;
  editor->setFont(font);

  highlighter = new SyntaxHighlighter(editor->document());
}

void
MainWindow::setupConsole()
{
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(14);

  console = new Console;
  console->setFont(font);

  connect(console, SIGNAL(saveFirst()), this, SLOT(save()));
}

void
MainWindow::setupFileMenu()
{
  QMenu* fileMenu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(fileMenu);

  fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
  fileMenu->addAction(
    tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
  fileMenu->addAction(tr("&Save"), this, SLOT(save()), QKeySequence::Save);
  fileMenu->addAction(
    tr("Save As"), this, SLOT(saveAs()), QKeySequence::SaveAs);

  QMenu* exitMenu = new QMenu(tr("Exit"), this);
  menuBar()->addMenu(exitMenu);
  exitMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);
}

void
MainWindow::setupBuildMenu()
{
  QMenu* buildMenu = new QMenu(tr("&Build"), this);
  menuBar()->addMenu(buildMenu);

  buildMenu->addAction(
    tr("&Build"), this, SIGNAL(run()), QKeySequence(Qt::CTRL + Qt::Key_B));
}

void
MainWindow::setupHelpMenu()
{
  QMenu* helpMenu = new QMenu(tr("&Help"), this);
  menuBar()->addMenu(helpMenu);

  helpMenu->addAction(tr("&About"), this, SLOT(about()));
  helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void
MainWindow::closeEvent(QCloseEvent* event)
{
  // Ensure that, if the user has changed something, that they really do want to
  // close the window. If they do, check if they want to save the file.
  if (editor->hasBeenModified()) {
    QMessageBox msgBox;
    msgBox.setText("This document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Save:
        save();
      case QMessageBox::Discard:
        event->accept();
        break;
      case QMessageBox::Cancel:
        event->ignore();
      default:
        break;
    }
  } else {
    event->accept();
  }
}
