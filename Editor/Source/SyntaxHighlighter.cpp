#include "SyntaxHighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;
  /* Maybe at some later point.
    labelFormat.setForeground(Qt::darkGreen);
    labelFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\.?[\\da-zA-Z_]+\\b");
    rule.format = labelFormat;
    highlightingRules.append(rule);
    */

  keywordFormat.setForeground(Qt::magenta);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\b[aA][dD]{2}\\b"
                  << "\\b[aA][nN][dD]\\b"
                  << "\\b[nN][oO][tT]\\b"
                  << "\\b[tT][rR][aA][pP]\\b"
                  << "\\b[jJ][sS][rR]{1,2}\\b"
                  << "\\b[rR][eE][tT]\\b"
                  << "\\b[jJ][mM][pP]\\b"
                  << "\\b[lL][dD][iIrR]?\\b"
                  << "\\b[lL][eE][aA]\\b"
                  << "\\b[sS][tT][rRiI]?\\b"
                  << "\\b[bB][rR][nNzZpP]{0,3}"
                  << "(?=\\W)\\.[oO][rR][iI][gG]\\b"
                  << "(?=\\W)\\.[eE][nN][dD]\\b"
                  << "(?=\\W)\\.[bB][lL][kK][wW]\\b"
                  << "(?=\\W)\\.[fF][iI][lL]{2}\\b"
                  << "(?=\\W)\\.[sS][tT][rR][iI][nN][gG][zZ]\\b"
#ifdef INCLUDE_ADDONS
                  << "(?=\\W)\\.[sS][eE][tT]\\b"
                  << "(?=\\W)\\.[lL][sS][hH][iI][fF][tT]\\b"
                  << "(?=\\W)\\.[iI][nN][cC][lL][uU][dD][eE]\\b"
                  << "(?=\\W)\\.[nN][eE][gG]\\b"
                  << "(?=\\W)\\.[sS][uU][bB]\\b"
#endif
    ;

  foreach (const QString& pattern, keywordPatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  QStringList immediateValuePatterns;
  immediateValuePatterns << "#?-?\\d+"
                         << "0?[xX][\\da-fA-F]+"
                         << "0?[bB][01]+"
#ifdef INCLUDE_ADDONS
                         << "\\\\\\d+"
#endif
    ;

  immediateValueFormat.setForeground(Qt::red);
  foreach (const QString& pattern, immediateValuePatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = immediateValueFormat;
    highlightingRules.append(rule);
  }

  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegularExpression("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
#ifdef INCLUDE_ADDONS
  rule.pattern = QRegularExpression("-?'\\\\?.'");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
#endif

  registerFormat.setForeground(Qt::blue);
  rule.pattern = QRegularExpression("[rR][0-7]");
  rule.format = registerFormat;
  highlightingRules.append(rule);

  // This needs to be last, otherwise if a match occurs inside a comment it'll
  // be coloured incorrectly.
  singleLineCommentFormat.setForeground(Qt::darkGray);
  rule.pattern = QRegularExpression("(//|;)[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);
}

void
SyntaxHighlighter::highlightBlock(const QString& text)
{
  foreach (const HighlightingRule& rule, highlightingRules) {
    QRegularExpressionMatchIterator matchIterator =
      rule.pattern.globalMatch(text);
    while (matchIterator.hasNext()) {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
  setCurrentBlockState(0);
}