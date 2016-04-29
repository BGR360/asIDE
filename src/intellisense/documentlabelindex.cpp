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
    mTokenizer(NULL)
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
        disconnect(mTokenizer, SIGNAL(lineAdded(int)), this, SLOT(onLineAdded(int)));
        disconnect(mTokenizer, SIGNAL(lineRemoved(int)), this, SLOT(onLineRemoved(int)));

        delete mTokenizer;
    }

    // Create a new tokenizer from the new document
    mTokenizer = new DocumentTokenizer(doc);
    readFromTokenizer();

    // Connect the new tokenizer
    if (mTokenizer) {
        connect(mTokenizer, SIGNAL(tokensAdded(TokenList,int)), this, SLOT(onTokensAdded(TokenList,int)));
        connect(mTokenizer, SIGNAL(tokensRemoved(TokenList,int)), this, SLOT(onTokensRemoved(TokenList,int)));
        connect(mTokenizer, SIGNAL(lineAdded(int)), this, SLOT(onLineAdded(int)));
        connect(mTokenizer, SIGNAL(lineRemoved(int)), this, SLOT(onLineRemoved(int)));
    } else {
        qDebug() << "Could not allocate new DocumentTokenizer";
    }

    emit documentChanged(doc);
}

DocumentTokenizer* DocumentLabelIndex::tokenizer()
{
    return mTokenizer;
}

bool DocumentLabelIndex::hasLabel(const QString& label) const
{
    return mLinesByLabel.contains(label);
}

bool DocumentLabelIndex::hasLabelAtLine(int line) const
{
    foreach (LabelInfo l, mLinesByLabel) {
        if (l.lineNumber == line)
            return true;
    }
    return false;
}

bool DocumentLabelIndex::hasLabelAtLine(const QString& label, int line) const
{
    if (!hasLabel(label))
        return false;
    if (!hasLabelAtLine(line))
        return false;
    return mLinesByLabel[label].lineNumber == line;
}

bool DocumentLabelIndex::isFunctionLabel(const QString& label) const
{
    if (!hasLabel(label))
        return false;
    return mLinesByLabel[label].type == FunctionLabel;
}

bool DocumentLabelIndex::isVariableLabel(const QString& label) const
{
    if (!hasLabel(label))
        return false;
    return mLinesByLabel[label].type == VariableLabel;
}

int DocumentLabelIndex::lineNumberOfLabel(const QString& label) const
{
    if (!hasLabel(label))
        return -1;
    return mLinesByLabel[label].lineNumber;
}

QString DocumentLabelIndex::labelAtLine(int line) const
{
    QMap<QString, LabelInfo>::const_iterator i;
    for (i = mLinesByLabel.begin(); i != mLinesByLabel.end(); ++i) {
        if (i.value().lineNumber == line)
            return i.key();
    }
    return QString();
}

QList<QString> DocumentLabelIndex::labels() const
{
    QList<QString> labels;
    QMap<QString, LabelInfo>::const_iterator i;
    for (i = mLinesByLabel.constBegin(); i != mLinesByLabel.constEnd(); ++i) {
        labels.push_back(i.key());
    }
    return labels;
}

void DocumentLabelIndex::reset()
{
    mLinesByLabel.clear();
}

void DocumentLabelIndex::readFromTokenizer()
{
    if (mTokenizer) {
        const int numLines = mTokenizer->numLines();
        for (int i = 0; i < numLines; ++i) {
            readLabelsFromLine(mTokenizer->tokensInLine(i), i);
        }
    }
}

void DocumentLabelIndex::readLabelsFromLine(const TokenList& tokensInLine, int line)
{
    if (!tokensInLine.empty()) {
        Token firstToken = tokensInLine.first();
        if (firstToken.type == Token::Label) {
            QString newLabel = firstToken.value;
            LabelInfo newLabelInfo = {line, VariableLabel};

            if (tokensInLine.size() >= 2) {
                Token secondToken = tokensInLine.at(1);
                if (secondToken.type == Token::Instruction)
                    newLabelInfo.type = FunctionLabel;
            }

            addLabel(newLabel, newLabelInfo.lineNumber, newLabelInfo.type);
        }
    }
}

void DocumentLabelIndex::addLabel(const QString& label, int line, LabelType type)
{
    qDebug() << "adding label:" << label << "at line:" << line;

    // If we already have this label at this line, make sure it has the right type
    if (hasLabelAtLine(label, line)) {
        mLinesByLabel[label].type = type;
        return;
    }

    // Add this label to our data structures
    LabelInfo newLabelInfo = {line, type};
    mLinesByLabel[label] = newLabelInfo;

    emit labelAdded(label, line);
}

void DocumentLabelIndex::removeLabel(const QString& label, int line)
{
    qDebug() << "removing label:" << label << "at line:" << line;

    if (!hasLabelAtLine(label, line))
        return;
    if (labelAtLine(line) != label)
        return;

    // Remove the label from our data structures
    mLinesByLabel.remove(label);

    qDebug() << "removed";
    emit labelRemoved(label, line);
}

void DocumentLabelIndex::onTokensAdded(const TokenList& tokens, int line)
{
    Q_UNUSED(tokens);
    readLabelsFromLine(mTokenizer->tokensInLine(line), line);
}

void DocumentLabelIndex::onTokensRemoved(const TokenList& tokens, int line)
{
    foreach (const Token& token, tokens) {
        if (hasLabelAtLine(token.value, line)) {
            removeLabel(token.value, line);
        }
    }
}

void DocumentLabelIndex::onLineAdded(int afterLine)
{
    // Shift the line numbers of all the labels that come after afterLine
    qDebug() << "Shifting labels forward after line" << afterLine;
    const int lastLine = tokenizer()->numLines() - 1;
    for (int i = lastLine; i > afterLine; --i) {
        if (hasLabelAtLine(i)) {
            // Shift the label one line up
            QString label = labelAtLine(i);
            mLinesByLabel[label].lineNumber += 1;
        }
    }
    emit lineAdded(afterLine);
}

void DocumentLabelIndex::onLineRemoved(int lineNumber)
{
    // The last line that we used to have should be one after the actual last line
    const int lastLine = tokenizer()->numLines();
    qDebug() << "Shifting labels backward after line" << lineNumber - 1 << "until line" << lastLine;
    for (int i = lineNumber + 1; i <= lastLine; ++i) {
        if (hasLabelAtLine(i)) {
            // Shift the label one line down
            QString label = labelAtLine(i);
            qDebug() << "shifting" << label << "down from line" << i << "to line" << i - 1;
            mLinesByLabel[label].lineNumber -= 1;
        }
    }
    emit lineRemoved(lineNumber);
}
