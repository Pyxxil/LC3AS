#ifndef SYNTAX_HIGHLIGHTER_HPP
#define SYNTAX_HIGHLIGHTER_HPP

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QVector<HighlightingRule> highlightingRules;

  QRegularExpression commentStartExpression;
  QRegularExpression commentEndExpression;

  QTextCharFormat instructionFormat;
  QTextCharFormat directiveFormat;
  QTextCharFormat trapKeywordFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat registerFormat;
  QTextCharFormat labelFormat;
  QTextCharFormat immediateValueFormat;
};

#endif // HIGHLIGHTER_H
