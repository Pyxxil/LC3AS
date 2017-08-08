#include "Editor/Includes/Editor.hpp"

#include <QFont>
#include <QtWidgets>

#include "Editor/Includes/ColourScheme.hpp"

Editor::Editor(QWidget* parent)
  : QPlainTextEdit(parent)
  , lineNumberArea(new LineNumbering(this))
  , m_highlighter(new ColourScheme(this))
{
  connect(this,
          SIGNAL(blockCountChanged(int)),
          this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this,
          SIGNAL(updateRequest(QRect, int)),
          this,
          SLOT(updateLineNumberArea(QRect, int)));

  updateLineNumberAreaWidth(0);
  setFont(
    QFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family(), 14));
}

int
Editor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  const int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return qMax(space, minWidth());
}

void
Editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
Editor::updateLineNumberArea(const QRect& rect, int dy)
{
  if (0 != dy) {
    lineNumberArea->scroll(0, dy);
  } else {
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    updateLineNumberAreaWidth(0);
  }
}

void
Editor::resizeEvent(QResizeEvent* e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(
    QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void
Editor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void
Editor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = static_cast<int>(
    blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + static_cast<int>(blockBoundingRect(block).height());

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      const QString number = QString("%1").arg(
        blockNumber + 1, lineNumberArea->width(), 10, QChar(' '));
      painter.setPen(Qt::black);
      painter.drawText(0,
                       top,
                       lineNumberArea->width(),
                       fontMetrics().height(),
                       Qt::AlignRight,
                       number);
    }

    block = block.next();
    top = bottom;
    bottom = top + static_cast<int>(blockBoundingRect(block).height());
    ++blockNumber;
  }
}
