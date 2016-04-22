//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//  The MIT License (MIT)                                                                                   //
//  Copyright (c) 2016 Benjamin Reeves                                                                      //
//                                                                                                          //
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software           //
//  and associated documentation files (the "Software"), to deal in the Software without restriction,       //
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,   //
//  subject to the following conditions:                                                                    //
//                                                                                                          //
//  The above copyright notice and this permission notice shall be included in all copies or substantial    //
//  portions of the Software.                                                                               //
//                                                                                                          //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT   //
//  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, //
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                  //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    QTextCharFormat badLabelFormat;
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
