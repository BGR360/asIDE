#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "intellisense_global.h"

#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class DocumentLabelIndex;

class INTELLISENSE_EXPORT SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument* parent, DocumentLabelIndex* labelIndex = 0);

protected:
    void highlightBlock(const QString& text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat functionLabelFormat;
    QTextCharFormat variableLabelFormat;
    QRegExp labelExpression;
    DocumentLabelIndex* labelIndex;

    QTextCharFormat keywordFormat;
    QTextCharFormat includeFormat;
    QTextCharFormat includeFileFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;

    bool isValidLabel(const QString& label) const;
    Token getStuffAfterLabel(const QString& label) const;
    bool isFunctionLabel(const QString& label) const;
    bool isVariableLabel(const QString& label) const;
};

#endif // SYNTAXHIGHLIGHTER_H
