#include <QtWidgets>

#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupMenus();

  setupEditor();

  setCentralWidget(editor);
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
        // Don't Save was clicked
        break;
      case QMessageBox::Cancel:
        // Cancel was clicked
        break;
      default:
        // should never be reached
        break;
    }
  } else {
    editor->open_file = QString();
    editor->clear();
  }
}

void
MainWindow::save()
{
  if (!editor->open_file.isEmpty()) {
    QFile file(editor->open_file);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
      QTextStream out(&file);
      out << editor->document()->toPlainText();
    }
  } else {
    saveAs();
  }
}

void
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
      }
    }
  }
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
        // Don't Save was clicked
        break;
      case QMessageBox::Cancel:
        // Cancel was clicked
        break;
      default:
        // should never be reached
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
    }
  }
}

void
MainWindow::setupEditor()
{
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);

  editor = new Editor;
  editor->setFont(font);

  highlighter = new SyntaxHighlighter(editor->document());

  QFile file("mainwindow.h");
  if (file.open(QFile::ReadOnly | QFile::Text))
    editor->setPlainText(file.readAll());
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
  fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence::Quit);
}

void
MainWindow::setupHelpMenu()
{
  QMenu* helpMenu = new QMenu(tr("&Help"), this);
  menuBar()->addMenu(helpMenu);

  helpMenu->addAction(tr("&About"), this, SLOT(about()));
  helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}