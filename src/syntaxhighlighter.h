#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument* parent = 0);

protected:
    void highlightBlock(const QString& text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat labelFormat;
    QRegExp labelExpression;
    QMap<QString, qint32> labels;

    QTextCharFormat keywordFormat;
    QTextCharFormat includeFormat;
    QTextCharFormat includeFileFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
};

#endif // SYNTAXHIGHLIGHTER_H
