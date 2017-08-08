#ifndef COLOUR_SCHEME_HPP
#define COLOUR_SCHEME_HPP

#include "Editor/Includes/Editor.hpp"

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class ColourScheme : public QSyntaxHighlighter
{
public:
  ColourScheme(Editor* t_editor);

public slots:
  void changed() { highlightBlock(editor->toPlainText()); }

protected:
  void highlightBlock(const QString& text) override;

private:
  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QVector<HighlightingRule> highlightingRules;

  QRegularExpression commentStartExpression;
  QRegularExpression commentEndExpression;

  QTextCharFormat keywordFormat;
  QTextCharFormat classFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat multiLineCommentFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat functionFormat;

  Editor* editor;
};

#endif
