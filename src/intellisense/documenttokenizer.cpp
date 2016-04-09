#include "documenttokenizer.h"

#include <QDebug>
#include <QStringRef>
#include <QTextDocument>

DocumentTokenizer::DocumentTokenizer(QTextDocument* doc) :
    mDoc(0)
{
    setDocument(doc);
    parse();
}

QTextDocument* DocumentTokenizer::document()
{
    return mDoc;
}

void DocumentTokenizer::setDocument(QTextDocument* doc)
{
    // Disconnect signals and slots from previous document
    if (mDoc) {
        disconnect(mDoc, SIGNAL(contentsChange(int,int,int)), this, SLOT(onDocumentContentsChanged(int,int,int)));
    }

    mDoc = doc;

    // Connect signals and slots to new documents
    if (mDoc) {
        connect(mDoc, SIGNAL(contentsChange(int,int,int)), this, SLOT(onDocumentContentsChanged(int,int,int)));
    }

    reset();
}

TokenList DocumentTokenizer::tokens()
{
    TokenList ret;
    TokenList::const_iterator i;
    for (i = mTokens.constBegin(); i != mTokens.constEnd(); ++i) {
        ret.append(*i);
    }
    return ret;
}

TokenList DocumentTokenizer::tokensInLine(int lineNumber) const
{
    TokenLineMap::const_iterator found = mTokensByLine.constBegin() + lineNumber;
    if (found != mTokensByLine.constEnd())
        return *found;
    else
        return TokenList();
}

int DocumentTokenizer::numTokens() const
{
    return mTokens.size();
}

int DocumentTokenizer::numLines() const
{
    return mTokensByLine.size();
}

const Token* DocumentTokenizer::addToken(const QString& value, Token::TokenType type, int line, int column)
{
    Token input;
    input.value = value;
    input.type = type;
    return addToken(input, line, column);
}

const Token* DocumentTokenizer::addToken(const Token& token, int line, int column)
{
    return NULL;
}

void DocumentTokenizer::addTokensToLine(const TokenList& tokens, int line)
{
    TokenList::const_iterator i;
    for (i = tokens.constBegin(); i != tokens.constEnd(); ++i) {
        TokenList::const_iterator addedToken = mTokens.insert(mTokens.end(), *i);
        while (mTokensByLine.size() <= line)
            mTokensByLine.push_back(TokenList());
        mTokensByLine[line].push_back(*addedToken);
    }
}

bool DocumentTokenizer::removeToken(const Token& token)
{
    return false;
}

bool DocumentTokenizer::removeToken(const Token* token)
{
    return removeToken(*token);
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
    if (mDoc) {
        QString document = mDoc->toPlainText();
        QString searchSpace = document.mid(beginPos, endPos - beginPos);

        // First split the document by lines
        QStringList lines = searchSpace.split(Token::REGEX[Token::Newline], QString::KeepEmptyParts);

        // Then parse each line
        for (int line = 0; line < lines.size(); ++line) {
            TokenList tokensInLine = parseLine(lines[line]);
            addTokensToLine(tokensInLine, line);
        }

        // Get rid of that trailing newline
        mTokens.pop_back();
        mTokensByLine[numLines() - 1].pop_back();
    }
}

TokenList DocumentTokenizer::parseLine(const QString& line)
{
    TokenList tokensInLine;

    // Watch out for comments!
    int indexOfComment = Token::REGEX[Token::Comment].indexIn(line);
    if (indexOfComment >= 0) {
        tokensInLine.push_back(parseWord(line));
    } else {
        // If not a comment, split the line into words and parse each word
        QStringList words = line.split(Token::REGEX[Token::Whitespace], QString::SkipEmptyParts);
        foreach (const QString& word, words) {
            tokensInLine.push_back(parseWord(word));
        }
    }
    Token newlineToken = {"\n", Token::Newline};
    tokensInLine.push_back(newlineToken);

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

void DocumentTokenizer::onDocumentContentsChanged(int position, int charsRemoved, int charsAdded)
{
    qDebug() << "Document contents changed:" <<
                " pos=" << position <<
                " numRemoved=" << charsRemoved <<
                " numAdded=" << charsAdded;
}
