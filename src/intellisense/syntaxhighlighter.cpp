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

#include "syntaxhighlighter.h"

#include "documentlabelindex.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent, DocumentLabelIndex* labelIndex)
    : QSyntaxHighlighter(parent),
      labelIndex(labelIndex)
{
    HighlightingRule rule;

    // Create a highlighting rule for numbers (decimal and hexidecimal)
    numberFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b0x[0-9a-fA-F]+\\b|-[0-9]+\\b|\\b[0-9]+\\b");
    //rule.pattern = Token::REGEX[Token::IntLiteral];
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for E100 labels that mark instructions
    functionLabelFormat.setForeground(Qt::darkRed);
    functionLabelFormat.setFontItalic(true);
    functionLabelFormat.setFontWeight(QFont::DemiBold);
    //labelExpression = QRegExp("^[A-Za-z]\\w*\\s");
    labelExpression = Token::REGEX[Token::Label];
    rule.pattern = labelExpression;
    rule.format = functionLabelFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for E100 labels that are variables
    variableLabelFormat.setFontItalic(false);
    rule.pattern = labelExpression;
    rule.format = variableLabelFormat;
    //highlightingRules.append(rule);

    // Create a highlighting rule for bad or erroneous E100 labels
    badLabelFormat.setFontUnderline(true);
    badLabelFormat.setUnderlineColor(Qt::red);
    badLabelFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    rule.pattern = labelExpression;
    rule.format = badLabelFormat;
    //highlightingRules.append(rule);

    // Create a highlighting rule for all the keywords (instructions)
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp(QString("\\b(%1)\\b").arg(Token::REGEX[Token::Instruction].pattern()));
    rule.format = keywordFormat;
    highlightingRules.append(rule);


    // Create a highlighting rule for single-quoted characters
    quotationFormat.setForeground(Qt::darkYellow);
    rule.pattern = Token::REGEX[Token::CharLiteral];
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for #include statements
    includeFormat.setForeground(Qt::darkBlue);
    rule.pattern = Token::REGEX[Token::Include];
    rule.format = includeFormat;
    highlightingRules.append(rule);

    // Now for included files
    includeFileFormat.setForeground(Qt::darkYellow);
    rule.pattern = Token::REGEX[Token::IncludeFile];
    rule.format = includeFileFormat;
    highlightingRules.append(rule);

    // Create a highlighting rule for single-line comments
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = Token::REGEX[Token::Comment];
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

            if (expression == labelExpression) {
                QString matchedLabel = text.mid(index, length);
                if (isFunctionLabel(matchedLabel))
                    setFormat(index, length, functionLabelFormat);
                else if (isVariableLabel(matchedLabel))
                    setFormat(index, length, variableLabelFormat);
                //else
                    //setFormat(index, length, badLabelFormat);
            } else {
                setFormat(index, length, rule.format);
            }

            index = expression.indexIn(text, index + length);
        }
    }
}

bool SyntaxHighlighter::isValidLabel(const QString& label) const
{
    if (!labelIndex)
        return false;
    if (!labelIndex->hasLabel(label))
        return false;
    return true;
}

Token SyntaxHighlighter::getStuffAfterLabel(const QString& label) const
{
    Token defaultToken = {QString(), Token::Unrecognized};

    if (!isValidLabel(label))
        return defaultToken;
    const DocumentTokenizer* tokenizer = labelIndex->tokenizer();
    const TokenList tokensInDeclarationLine = tokenizer->tokensInLine(labelIndex->lineNumberOfLabel(label));
    Token searchToken = {label, Token::Label};
    const int indexOfLabelInLine = tokensInDeclarationLine.indexOf(searchToken);
    if (indexOfLabelInLine == -1 || tokensInDeclarationLine.size() < indexOfLabelInLine + 2)
        return defaultToken;

    return tokensInDeclarationLine.at(indexOfLabelInLine + 1);
}

bool SyntaxHighlighter::isFunctionLabel(const QString& label) const
{
    if (!isValidLabel(label))
        return false;
    return getStuffAfterLabel(label).type == Token::Instruction;
}

bool SyntaxHighlighter::isVariableLabel(const QString& label) const
{
    if (!isValidLabel(label))
        return false;
    Token stuffAfterLabel = getStuffAfterLabel(label);
    return stuffAfterLabel.type == Token::IntLiteral
            || stuffAfterLabel.type == Token::CharLiteral;
            //|| (stuffAfterLabel.type == Token::Label && isVariableLabel(stuffAfterLabel.value));
}

