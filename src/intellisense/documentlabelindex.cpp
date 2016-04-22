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

#include "documentlabelindex.h"

#include <QDebug>

DocumentLabelIndex::DocumentLabelIndex(QTextDocument* doc) :
    mTokenizer(0)
{
    setDocument(doc);
}

DocumentLabelIndex::~DocumentLabelIndex()
{
    if (mTokenizer)
        delete mTokenizer;
}

QTextDocument* DocumentLabelIndex::document()
{
    if (mTokenizer)
        return mTokenizer->document();
    else
        return NULL;
}

void DocumentLabelIndex::setDocument(QTextDocument* doc)
{
    // Delete and disconnect the previous tokenizer
    if (mTokenizer) {
        disconnect(mTokenizer, SIGNAL(tokensAdded(TokenList,int)), this, SLOT(onTokensAdded(TokenList,int)));
        disconnect(mTokenizer, SIGNAL(tokensRemoved(TokenList,int)), this, SLOT(onTokensRemoved(TokenList,int)));

        delete mTokenizer;
    }

    // Create a new tokenizer from the new document
    mTokenizer = new DocumentTokenizer(doc);
    readFromTokenizer();

    // Connect the new tokenizer
    connect(mTokenizer, SIGNAL(tokensAdded(TokenList,int)), this, SLOT(onTokensAdded(TokenList,int)));
    connect(mTokenizer, SIGNAL(tokensRemoved(TokenList,int)), this, SLOT(onTokensRemoved(TokenList,int)));

    emit documentChanged(doc);
}

DocumentTokenizer* DocumentLabelIndex::tokenizer()
{
    return mTokenizer;
}

bool DocumentLabelIndex::hasLabel(const QString& label) const
{
    qDebug() << "Checking for label:" << label;
    return mLabels.contains(label);
}

bool DocumentLabelIndex::hasLabelAtLine(const QString& label, int line) const
{
    if (!hasLine(line))
        return false;
    const QString* labelAtLine = mLabelsByLine[line];
    return QStringRef(labelAtLine) == label;
}

bool DocumentLabelIndex::hasLine(int line) const
{
    return mLabelsByLine.contains(line);
}

int DocumentLabelIndex::lineNumberOfLabel(const QString& label) const
{
    if (!hasLabel(label))
        return -1;

    const QString& existingLabel = findLabelRef(label);
    return mLinesByLabel[&existingLabel];
}

QString DocumentLabelIndex::labelAtLine(int line) const
{
    if (!hasLine(line))
        return QString();
    return *(mLabelsByLine[line]);
}

QList<QString> DocumentLabelIndex::labels() const
{
    QList<QString> labels;
    foreach (const QString* label, mLabelsByLine) {
        labels.push_back(*label);
    }
    return labels;
}

void DocumentLabelIndex::reset()
{
    mLabels.clear();
    mLabelsByLine.clear();
    mLinesByLabel.clear();
}

void DocumentLabelIndex::readFromTokenizer()
{
    if (mTokenizer) {
        const int numLines = mTokenizer->numLines();
        for (int i = 0; i < numLines; ++i) {
            const TokenList& tokensInLine = mTokenizer->tokensInLine(i);
            if (!tokensInLine.empty()) {
                QString labelInLine = readLabelsFromLine(tokensInLine);
                if (!labelInLine.isEmpty())
                    addLabel(labelInLine, i);
            }
        }
    }
}

QString DocumentLabelIndex::readLabelsFromLine(const TokenList& tokensInLine)
{
    if (tokensInLine.size() > 0) {
        const Token& firstToken = tokensInLine.at(0);
        if (firstToken.type == Token::Label) {
            return firstToken.value;
        } else {
            return QString();
        }
    }
    return QString();
}

void DocumentLabelIndex::addLabel(const QString& label, int line)
{
    qDebug() << "adding label:" << label << "at line:" << line;

    // If we already have this label at this line, don't do anything
    if (hasLabelAtLine(label, line))
        return;

    // Add this label to our data structures
    QSet<QString>::iterator inserted = mLabels.insert(label);
    const QString* newLabel = &(*inserted);
    mLabelsByLine[line] = newLabel;
    mLinesByLabel[newLabel] = line;

    emit labelAdded(label, line);
}

void DocumentLabelIndex::removeLabel(const QString& label, int line)
{
    qDebug() << "removing label:" << label << "at line:" << line;

    // If we don't have this label, don't do anything
    if (!hasLabel(label))
        return;

    // If we don't have anything on the same line as line, don't do anything
    if (!hasLine(line))
        return;

    // If the label on the same line as line is not equal to label, don't do anything
    const QString* labelAtLine = mLabelsByLine[line];
    if (QStringRef(labelAtLine) != label)
        return;

    // Remove the label from our data structures
    mLabelsByLine.remove(line);
    mLinesByLabel.remove(labelAtLine);
    mLabels.remove(label);

    emit labelRemoved(label, line);
}

void DocumentLabelIndex::onTokensAdded(const TokenList& tokens, int line)
{
    // IMPORTANT: tokens is NOT guaranteed to be in order
    // So we must look at the first token in the tokenizer's line, and see if tokens
    // CONTAINS that label
    QString labelInLine = readLabelsFromLine(mTokenizer->tokensInLine(line));
    Token searchToken = {labelInLine, Token::Label};
    const int indexOfLabelInTokens = tokens.indexOf(searchToken);
    if (indexOfLabelInTokens >= 0)
        addLabel(labelInLine, line);
}

void DocumentLabelIndex::onTokensRemoved(const TokenList& tokens, int line)
{
    foreach (const Token& token, tokens) {
        if (token.type == Token::Label) {
            removeLabel(token.value, line);
        }
    }
}

const QString& DocumentLabelIndex::findLabelRef(const QString& label) const
{
    return *(mLabels.find(label));
}
