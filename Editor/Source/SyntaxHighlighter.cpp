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

  keywordFormat.setForeground(QColor(0xC7, 0x95, 0xAE));
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

  trapKeywordFormat.setForeground(QColor(0xAE, 0x95, 0xC7));
  QStringList trapPatterns;
  trapPatterns << "\\b[pP][uU][tT][sScC]\\b"
               << "\\b[gG][eE][tT][cC]\\b"
               << "\\b[oO][uU][tT]\\b"
               << "\\b[iI][nN]\\b"
               << "\\b[hH][aA][lL][tT]\\b";

  foreach (const QString& pattern, trapPatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = trapKeywordFormat;
    highlightingRules.append(rule);
  }

  QStringList immediateValuePatterns;
  immediateValuePatterns << "(\\s|,)#?-?\\d+"
                         << "(\\s|,)0?[xX][\\da-fA-F]+"
                         << "(\\s|,)0?[bB][01]+"
#ifdef INCLUDE_ADDONS
                         << "\\\\\\d+"
#endif
    ;

  immediateValueFormat.setForeground(QColor(0xC7, 0xAE, 0x95));
  foreach (const QString& pattern, immediateValuePatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = immediateValueFormat;
    highlightingRules.append(rule);
  }

  quotationFormat.setForeground(QColor(0x95, 0xC7, 0xAE));
  rule.pattern = QRegularExpression("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
#ifdef INCLUDE_ADDONS
  rule.pattern = QRegularExpression("-?'\\\\?.'");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
#endif

  registerFormat.setForeground(QColor(0x95, 0xAE, 0xC7));
  rule.pattern = QRegularExpression("(\\s|,)[rR][0-7]");
  rule.format = registerFormat;
  highlightingRules.append(rule);

  // This needs to be last, otherwise if a match occurs inside a comment it'll
  // be coloured incorrectly.
  singleLineCommentFormat.setForeground(QColor(0xAD, 0xB3, 0xBA));
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
