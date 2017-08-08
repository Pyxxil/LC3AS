#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <QObject>
#include <QPlainTextEdit>

class QWidget;
class LineNumbering;
class ColourScheme;

class Editor : public QPlainTextEdit
{

  Q_OBJECT

public:
  Editor(QWidget* parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent* event);
  int lineNumberAreaWidth();

protected:
  void resizeEvent(QResizeEvent* event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect&, int);

  int minWidth() const { return fontMetrics().width(QLatin1Char('9')) * 5; }

private:
  LineNumbering* lineNumberArea;
  ColourScheme* m_highlighter;
};

class LineNumbering : public QWidget
{
public:
  LineNumbering(Editor* t_editor)
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