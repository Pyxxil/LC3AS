#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <QObject>
#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class Editor : public QPlainTextEdit
{
  Q_OBJECT

public:
  Editor(QWidget* parent = 0);

  void lineNumberAreaPaintEvent(QPaintEvent* event);
  int lineNumberAreaWidth();

  QString open_file;

  bool hasBeenModified() const { return modified; }

protected:
  void resizeEvent(QResizeEvent* event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect&, int);
  void textModified(bool mod) { modified = mod; };

private:
  QWidget* lineNumberArea;
  bool modified;
};

class LineNumberArea : public QWidget
{
public:
  LineNumberArea(Editor* t_editor)
    : QWidget(t_editor)
    , editor(t_editor)
  {}

  QSize sizeHint() const override
  {
    return QSize(editor->lineNumberAreaWidth(), 0);
  }

protected:
  void paintEvent(QPaintEvent* event) override
  {
    editor->lineNumberAreaPaintEvent(event);
  }

private:
  Editor* editor;
};

#endif
