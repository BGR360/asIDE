#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Create highlighting rules for all the keywords (instructions)
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywords;
    keywords << "halt" << "add" << "sub"
             << "mult" << "div" << "cp"
             << "and" << "or" << "not"
             << "sl" << "sr" << "cpfa"
             << "cpta" << "be" << "bne"
             << "blt" << "call" << "ret";
    foreach (const QString &pattern, keywords) {
        rule.pattern = QRegExp(QString("\\b%1\\b").arg(pattern));
        rule.format = keywordFormat;
        highlightingRules.append(rule);

        // Also add the uppercase form
        rule.pattern = QRegExp(QString("\\b%1\\b").arg(pattern.toUpper()));
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Create a highlighting rule for single-quoted characters
    quotationFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("'.'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for #include statements
    includeFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("^#include");
    rule.format = includeFormat;
    highlightingRules.append(rule);


    // Create a highlighting rule for numbers (decimal and hexidecimal)
    numberFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b0x[0-9a-fA-F]+\\b|\\b[0-9]+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);
    includeFileFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("[\\./\\w]+\\.e");
    rule.format = includeFileFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for E100 labels
    labelFormat.setForeground(Qt::darkRed);
    labelFormat.setFontItalic(true);
    labelFormat.setFontWeight(QFont::DemiBold);
    labelExpression = QRegExp("^[A-Za-z]\\w*\\s");
    rule.pattern = labelExpression;
    rule.format = labelFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for single-line comments
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            if (length == 0)
                break;
            setFormat(index, length, rule.format);

            index = expression.indexIn(text, index + length);
        }
    }
}

