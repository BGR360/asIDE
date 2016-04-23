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

#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTextCursor>

#include "intellisense_global.h"
#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

typedef QList<Token> TokenList;
typedef QVector<TokenList> TokenLineMap;

class INTELLISENSE_EXPORT DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc = 0);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    TokenList tokens();
    TokenList tokensInLine(int lineNumber) const;
    int numTokens() const;
    int numLines() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const TokenList& tokens, int lineNumber);
    void tokensRemoved(const TokenList& tokens, int lineNumber);
    void lineAdded(int afterLine);
    void lineRemoved(int lineNumber);

protected:
    void addLine(int afterLine);
    void removeLine(int lineNumber);
    void setLine(const TokenList& tokens, int line);

    void reset();
    void parse();
    void parse(int beginPos, int endPos);
    void parseLines(int beginLine, int endLine);
    TokenList parseLine(const QString& line);
    Token parseWord(const QString& word);

private slots:
    void onDocumentContentsChanged();
    void onDocumentContentsChanged(int position, int charsRemoved, int charsAdded);
    void onCursorPositionChanged(const QTextCursor& cursor);
    void onLineCountChange(int newLineCount);

private:
    QTextDocument* mDoc;
    QTextCursor mCursor;
    int mCursorPos;
    bool mReceivedLongDocumentChange;

    TokenList mTokens;
    TokenLineMap mTokensByLine;

    int getLineNumberOfPosition(int pos) const;
    int getStartPosOfLine(int line) const;
    int getEndPosOfLine(int line) const;
};

#endif // DOCUMENTTOKENIZER_H
