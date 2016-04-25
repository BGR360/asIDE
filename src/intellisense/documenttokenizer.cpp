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

#include "documenttokenizer.h"

#include <QDebug>
#include <QSet>
#include <QStringRef>
#include <QTextBlock>
#include <QTextDocument>

DocumentTokenizer::DocumentTokenizer(QTextDocument* doc) :
    mDoc(0),
    mCursorPos(0),
    mReceivedLongDocumentChange(false)
{
    setDocument(doc);
}

QTextDocument* DocumentTokenizer::document()
{
    return mDoc;
}

void DocumentTokenizer::setDocument(QTextDocument* doc)
{
    // Disconnect signals and slots from previous document
    if (mDoc) {
        disconnect(mDoc, SIGNAL(contentsChanged()), this, SLOT(onDocumentContentsChanged()));
        disconnect(mDoc, SIGNAL(contentsChange(int, int, int)), this,
                   SLOT(onDocumentContentsChanged(int, int, int)));
        disconnect(mDoc, SIGNAL(cursorPositionChanged(QTextCursor)), this,
                   SLOT(onCursorPositionChanged(QTextCursor)));
        disconnect(mDoc, SIGNAL(blockCountChanged(int)), this, SLOT(onLineCountChange(int)));
    }

    mDoc = doc;
    mCursorPos = 0;

    // Connect signals and slots to new document
    if (mDoc) {
        connect(mDoc, SIGNAL(contentsChanged()), this, SLOT(onDocumentContentsChanged()));
        connect(mDoc, SIGNAL(contentsChange(int, int, int)), this,
                SLOT(onDocumentContentsChanged(int, int, int)));
        connect(mDoc, SIGNAL(cursorPositionChanged(QTextCursor)), this,
                SLOT(onCursorPositionChanged(QTextCursor)));
        connect(mDoc, SIGNAL(blockCountChanged(int)), this, SLOT(onLineCountChange(int)));
    }

    reset();
    parse();

    emit documentChanged(mDoc);
}

TokenList DocumentTokenizer::tokens()
{
    TokenList ret;

    // For each line
    foreach (const TokenList& tokensInLine, mTokensByLine) {
        // For each token in the line
        foreach(const Token& token, tokensInLine) {
            ret.push_back(token);
        }
    }
    return ret;
}

TokenList DocumentTokenizer::tokensInLine(int lineNumber) const
{
    Q_ASSERT(lineNumber >= 0);
    Q_ASSERT(lineNumber == 0 || lineNumber < numLines());

    return mTokensByLine[lineNumber];
}

int DocumentTokenizer::numTokens() const
{
    return mTokens.size();
}

int DocumentTokenizer::numLines() const
{
    return mTokensByLine.size();
}

void DocumentTokenizer::addLine(int afterLine)
{
    qDebug() << "adding line after line" << afterLine;
    bool isAddingLineToEndOfDoc = afterLine == numLines() - 1;

    // Make sure we have at least afterLine number of lines
    while (numLines() <= afterLine) {
        mTokensByLine.push_back(TokenList());
    }

    TokenLineMap::iterator whereToInsertLine = mTokensByLine.begin() + afterLine + 1;
    mTokensByLine.insert(whereToInsertLine, TokenList());

    // If the new line that we added was added to the end of the document, add
    // a newline token to the second to last line.
    // Otherwise add a newline token to the newly added line
    Token newline = {"\n", Token::Newline};
    TokenList added = {newline};
    const int whereToAddNewlineToken = (isAddingLineToEndOfDoc) ? afterLine : afterLine + 1;
    if (whereToAddNewlineToken >= 0) {
        mTokensByLine[whereToAddNewlineToken].push_back(newline);
        emit tokensAdded(added, whereToAddNewlineToken);
    }
    emit lineAdded(afterLine);
}

void DocumentTokenizer::removeLine(int lineNumber)
{
    Q_ASSERT(lineNumber >= 0);
    Q_ASSERT(lineNumber < numLines());

    qDebug() << "removing line" << lineNumber;

    bool isRemovingLastLine = (lineNumber == numLines() - 1);
    TokenList removedTokens = mTokensByLine[lineNumber];

    mTokensByLine.erase(mTokensByLine.begin() + lineNumber);

    // Remove the newline token from the previous line if the previous line is now the last line
    if (isRemovingLastLine) {
        emit tokensRemoved(removedTokens, lineNumber);
        removedTokens.clear();
        removedTokens.push_back({"\n", Token::Newline});
        mTokensByLine[lineNumber - 1].pop_back();
        emit tokensRemoved(removedTokens, lineNumber - 1);
    }
    emit lineRemoved(lineNumber);
}

void DocumentTokenizer::setLine(const TokenList& tokens, int line)
{
    Q_ASSERT(line >= 0);

    qDebug() << "setting line" << line;

    // Add new lines until we have at least line + 1 of them
    while (line >= numLines()) {
        addLine(numLines() - 1);
    }

    // Set the tokens in that line to the TokenList passed in
    TokenList oldTokens = mTokensByLine[line];
    mTokensByLine[line] = tokens;

    // If that line is the last line, remove the trailing newline token
    if (line == numLines() - 1) {
        mTokensByLine[line].pop_back();
    }

    // Report which tokens were added/removed
    const TokenList& newTokens = tokens;

    TokenList removedTokens;
    foreach (const Token& token, oldTokens) {
        if (!newTokens.contains(token))
            removedTokens.push_back(token);
    }

    TokenList addedTokens;
    foreach (const Token& token, newTokens) {
        if (!oldTokens.contains(token))
            addedTokens.push_back(token);
    }

    if (removedTokens.size() > 0) {
        qDebug() << "Removed tokens:" << removedTokens;
        emit tokensRemoved(removedTokens, line);
    } else {
        qDebug() << "None removed";
    }

    if (addedTokens.size() > 0) {
        qDebug() << "Added tokens:" << addedTokens;
        emit tokensAdded(addedTokens, line);
    } else {
        qDebug() << "None added";
    }
}

void DocumentTokenizer::reset()
{
    TokenLineMap oldTokens = mTokensByLine;
    mTokensByLine.clear();
    int line = 0;
    foreach (const TokenList& tokens, oldTokens) {
        emit tokensRemoved(tokens, line);
        line++;
    }
}

void DocumentTokenizer::parse()
{
    if (mDoc) {
        int beginPos = 0;
        int endPos = mDoc->characterCount();
        parse(beginPos, endPos);
    }
}

void DocumentTokenizer::parse(int beginPos, int endPos)
{
    if (mDoc) {
        QString document = mDoc->toPlainText();
        QString searchSpace = document.mid(beginPos, endPos - beginPos);

        int beginLine = getLineNumberOfPosition(beginPos);

        if (searchSpace.length() > 0) {
            // First split the document by lines
            QStringList lines = searchSpace.split(Token::REGEX[Token::Newline], QString::KeepEmptyParts);
            const int numLines = lines.size();
            qDebug() << "numLines:" << numLines;

            // Then parse each line
            for (int line = 0; line < numLines; ++line) {
                TokenList tokensInLine = parseLine(lines[line]);
                setLine(tokensInLine, beginLine + line);
            }
        } else if (beginPos == 0){
            reset();
        }
    }
}

void DocumentTokenizer::parseLines(int beginLine, int endLine)
{
    const int beginPos = getStartPosOfLine(beginLine);
    const int endPos = getEndPosOfLine(endLine) - 1;
    parse(beginPos, endPos);
}

TokenList DocumentTokenizer::parseLine(const QString& line)
{
    TokenList tokensInLine;

    // Watch out for comments!
    int indexOfComment = Token::REGEX[Token::Comment].indexIn(line);
    if (indexOfComment >= 0)
    {
        qDebug() << "comment found at index" << indexOfComment;

        // Parse the part of the line that comes before the comment
        QString beginningOfLine = line.left(indexOfComment);
        qDebug() << "parsing" << beginningOfLine;
        tokensInLine = parseLine(beginningOfLine);

        // Remove newline token from that parsed list
        tokensInLine.pop_back();

        // Add the comment token
        QString commentString = line.right(line.length() - indexOfComment);
        tokensInLine.push_back(parseWord(commentString));
    }
    else
    {
        // If not a comment, split the line into words and parse each word
        QStringList words = line.split(Token::REGEX[Token::Whitespace], QString::SkipEmptyParts);
        qDebug() << "Split line into words: " << words;
        foreach (const QString& word, words) {
            tokensInLine.push_back(parseWord(word));
        }
    }

    // Add a newline token to the end of every line
    Token newline = {"\n", Token::Newline};
    tokensInLine.push_back(newline);

    return tokensInLine;
}

Token DocumentTokenizer::parseWord(const QString& word)
{
    Token token;
    token.value = word;

    for (int i = 0; i < Token::NUM_TOKEN_TYPES - 1; ++i) {
        if (Token::REGEX[i].exactMatch(word)) {
            token.type = static_cast<Token::TokenType>(i);
            return token;
        }
    }
    token.type = Token::Unrecognized;

    return token;
}

void DocumentTokenizer::onDocumentContentsChanged()
{
    qDebug() << "Document contents changed";
    if (!mReceivedLongDocumentChange) {
        parse();
    }
    mReceivedLongDocumentChange = false;
}

void DocumentTokenizer::onDocumentContentsChanged(int position, int charsRemoved, int charsAdded)
{
    qDebug() << "contents changed" <<
                "pos=" << position <<
                "removed=" << charsRemoved <<
                "added=" << charsAdded;

    mCursorPos = position + charsAdded - charsRemoved;
    if (mCursorPos < 0) mCursorPos = 0;

    const int startLine = getLineNumberOfPosition(position);
    const int endLineOfAdded = getLineNumberOfPosition(position + charsAdded);

    qDebug() << "startLine:" << startLine;

    qDebug() << "reparsing" << "startLine=" << startLine << "endLine=" << endLineOfAdded;
    if (endLineOfAdded - startLine == 0)
        parseLines(startLine, endLineOfAdded);

    mReceivedLongDocumentChange = true;
}

void DocumentTokenizer::onCursorPositionChanged(const QTextCursor& cursor)
{
    qDebug() << "cursor pos changed:" << cursor.position();
    mCursor = cursor;
    if (!mReceivedLongDocumentChange)
        mCursorPos = cursor.position();
}

void DocumentTokenizer::onLineCountChange(int newLineCount)
{
    int difference = numLines() - newLineCount;
    int cursorPos = mCursorPos;

    qDebug() << "cursor pos:" << cursorPos;

    // Remove lines if line count was decreased
    // Also, reparse to account for removed lines
    if (difference > 0) {
        qDebug() << difference << "lines removed";
        int lineToRemove = getLineNumberOfPosition(cursorPos) + 1;
        for (int i = lineToRemove + difference - 1; i >= lineToRemove; --i) {
            removeLine(i);
        }
        parseLines(lineToRemove - 1, lineToRemove + 1);
    }

    // Add lines if line count was increased
    // Also, reparse to include the new lines
    if (difference < 0) {
        difference *= -1;
        qDebug() << difference << "lines added";
        const int finalLineToAdd = getLineNumberOfPosition(cursorPos);
        const int firstLineToAdd = finalLineToAdd - difference + 1;
        for (int i = firstLineToAdd; i <= finalLineToAdd; ++i) {
            addLine(i);
        }
        parseLines(firstLineToAdd, finalLineToAdd);
    }
}

int DocumentTokenizer::getLineNumberOfPosition(int pos) const
{
    QTextCursor cursor(mDoc);
    if (pos >= mDoc->characterCount())
        pos = mDoc->characterCount() - 1;
    cursor.setPosition(pos);
    return cursor.block().blockNumber();
}

int DocumentTokenizer::getStartPosOfLine(int line) const
{
    return mDoc->findBlockByNumber(line).position();
}

int DocumentTokenizer::getEndPosOfLine(int line) const
{
    return getStartPosOfLine(line) + mDoc->findBlockByNumber(line).length();
}
