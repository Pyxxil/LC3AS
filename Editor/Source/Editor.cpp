#include <QFont>
#include <QtWidgets>

#include "Editor.hpp"

Editor::Editor(QWidget *parent) : QPlainTextEdit(parent) {
  lineNumberArea = new LineNumberArea(this);

  connect(this, SIGNAL(blockCountChanged(int)), this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect, int)), this,
          SLOT(updateLineNumberArea(QRect, int)));
  connect(this, SIGNAL(cursorPositionChanged()), this,
          SLOT(highlightCurrentLine()));
  connect(this, SIGNAL(modificationChanged(bool)), this,
          SLOT(textModified(bool)));

  setCursorWidth(9);

  QPalette p = palette();
  p.setColor(QPalette::Active, QPalette::Base, QColor(0x39, 0x3F, 0x45));
  p.setColor(QPalette::Inactive, QPalette::Base, QColor(0x39, 0x3F, 0x45));
  p.setColor(QPalette::Active, QPalette::Text, QColor(0xF3, 0xF4, 0xF5));
  p.setColor(QPalette::Inactive, QPalette::Text, QColor(0xF3, 0xF4, 0xF5));
  setPalette(p);

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

int Editor::lineNumberAreaWidth() {
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  const int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void Editor::updateLineNumberAreaWidth(int /* newBlockCount */) {
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void Editor::updateLineNumberArea(const QRect &rect, int dy) {
  if (dy) {
    lineNumberArea->scroll(0, dy);
  } else {
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    updateLineNumberAreaWidth(0);
  }
}

void Editor::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);

  const QRect cr = contentsRect();
  lineNumberArea->setGeometry(
      QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void Editor::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(0x74, 0x7C, 0x84);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event) {
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), QColor(0x56, 0x5E, 0x65));

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = static_cast<int>(
      blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + static_cast<int>(blockBoundingRect(block).height());

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      const QString number = QString::number(blockNumber + 1);
      painter.setPen(QColor(0xDF, 0xE2, 0xE5));
      painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + static_cast<int>(blockBoundingRect(block).height());
    ++blockNumber;
  }
}
