#include "documenttokenizer.h"

#include <QDebug>
#include <QSet>
#include <QStringRef>
#include <QTextBlock>
#include <QTextDocument>

DocumentTokenizer::DocumentTokenizer(QTextDocument* doc) :
    mDoc(0),
    mPrevCursorPos(0),
    mNextCursorPos(0),
    mPrevCursorLine(0)
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
    mPrevCursorPos = 0;
    mNextCursorPos = 0;

    // Connect signals and slots to new documents
    if (mDoc) {
        connect(mDoc, SIGNAL(contentsChanged()), this, SLOT(onDocumentContentsChanged()));
        connect(mDoc, SIGNAL(contentsChange(int, int, int)), this,
                SLOT(onDocumentContentsChanged(int, int, int)));
        connect(mDoc, SIGNAL(cursorPositionChanged(QTextCursor)), this,
                SLOT(onCursorPositionChanged(QTextCursor)));
        connect(mDoc, SIGNAL(blockCountChanged(int)), this, SLOT(onLineCountChange(int)));
    }

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
    Q_ASSERT(lineNumber < numLines());

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

void DocumentTokenizer::addLine()
{
    // Add a newline token to the current last line (if there are any lines)
    if (numLines() > 0) {
        Token newline = {"\n", Token::Newline};
        mTokensByLine[numLines() - 1].push_back(newline);

        TokenList added;
        added.push_back(newline);
        emit tokensAdded(added, numLines() - 1);
    }

    // Add a new line of tokens
    mTokensByLine.push_back(TokenList());
}

void DocumentTokenizer::removeLine(int lineNumber)
{
    Q_ASSERT(lineNumber >= 0);
    Q_ASSERT(lineNumber < numLines());

    bool isRemovingLastLine = (lineNumber == numLines() - 1);
    TokenList removedTokens = mTokensByLine[lineNumber];

    mTokensByLine.erase(mTokensByLine.begin() + lineNumber);
    emit tokensRemoved(removedTokens, lineNumber);

    // Remove the newline token from the previous line if the previous line is now the last line
    if (isRemovingLastLine) {
        removedTokens.clear();
        removedTokens.push_back({"\n", Token::Newline});
        mTokensByLine[lineNumber - 1].pop_back();
        emit tokensRemoved(removedTokens, lineNumber - 1);
    }
}

void DocumentTokenizer::setLine(const TokenList& tokens, int line)
{
    Q_ASSERT(line >= 0);

    // Add new lines until we have at least line + 1 of them
    while (line >= numLines()) {
        addLine();
    }

    // Set the tokens in that line to the TokenList passed in
    QSet<Token> oldTokens = QSet<Token>::fromList(mTokensByLine[line]);
    mTokensByLine[line] = tokens;

    // If that line is the last line, remove the trailing newline token
    if (line == numLines() - 1) {
        mTokensByLine[line].pop_back();
    }

    // Report which tokens were added/removed
    QSet<Token> newTokens = QSet<Token>::fromList(mTokensByLine[line]);

    QSet<Token> tokensThatStayed = oldTokens;
    tokensThatStayed.intersect(newTokens);

    TokenList addedTokens, removedTokens;
    addedTokens = newTokens.subtract(tokensThatStayed).toList();
    removedTokens = oldTokens.subtract(tokensThatStayed).toList();

    if (removedTokens.size() > 0)
        emit tokensRemoved(removedTokens, line);
    if (addedTokens.size() > 0)
        emit tokensAdded(addedTokens, line);
}

void DocumentTokenizer::reset()
{
    mTokens.clear();
    mTokensByLine.clear();
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
    reset();
    if (mDoc) {
        QString document = mDoc->toPlainText();
        QString searchSpace = document.mid(beginPos, endPos - beginPos);

        if (searchSpace.length() > 0) {
            // First split the document by lines
            QStringList lines = searchSpace.split(Token::REGEX[Token::Newline], QString::KeepEmptyParts);
            const int numLines = lines.size();
            qDebug() << "numLines:" << numLines;

            // Then parse each line
            for (int line = 0; line < numLines; ++line) {
                TokenList tokensInLine = parseLine(lines[line]);
                setLine(tokensInLine, line);
            }
        }
    }
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
    int line = 0;
    line = mCursor.block().firstLineNumber();
    QString lineText = mCursor.block().text();

    if (line >= 0) {
        TokenList newTokensInLine = parseLine(lineText);
        setLine(newTokensInLine, line);
    }

    mPrevCursorPos = mNextCursorPos;
}

void DocumentTokenizer::onCursorPositionChanged(const QTextCursor& cursor)
{
    int line = cursor.block().firstLineNumber();

    mNextCursorPos = cursor.position();
    mPrevCursorLine = line;
    mCursor = cursor;
}

void DocumentTokenizer::onLineCountChange(int newLineCount)
{
    int difference = numLines() - newLineCount;
    // Remove lines if line count was decreased
    if (difference > 0) {
        int lineToRemove = mCursor.block().firstLineNumber() + 1;
        for (int i = 0; i < difference; ++i) {
            removeLine(lineToRemove);
        }
    }
}
