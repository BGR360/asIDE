#include "documenttokenizer.h"

#include <QTextDocument>

DocumentTokenizer::DocumentTokenizer(QTextDocument* doc) :
    mDoc(0)
{

}

QTextDocument* DocumentTokenizer::document()
{
    return NULL;
}

void DocumentTokenizer::setDocument(QTextDocument* doc)
{

}

TokenCollection DocumentTokenizer::tokens()
{
    return TokenCollection();
}

bool DocumentTokenizer::hasToken(const Token* token) const
{
    return hasToken(*token);
}

bool DocumentTokenizer::hasToken(const Token& token) const
{
    return false;
}

ConstTokenList DocumentTokenizer::getTokensInLine(int lineNumber) const
{
    return ConstTokenList();
}

const Token* DocumentTokenizer::addToken(const QString& value, Token::TokenType type, int line, int column)
{
    Token input;
    input.value = value;
    input.type = type;
    input.location.lineNumber = line;
    input.location.columnNumber = column;
    return addToken(input);
}

const Token* DocumentTokenizer::addToken(const Token& token)
{
    typedef TokenCollection::iterator setIter;
    typedef TokenList::iterator vecIter;

    setIter foundToken = mTokens.find(token);
    if (foundToken == mTokens.end()) {
        setIter insertedToken = mTokens.insert(token);
        Token* pNewToken = &(*insertedToken);

        // Add the new token to all of our data structures

        // First, insert it into the proper position in the line
        int line = token.location.lineNumber;
        if (mTokensByLine.contains(line)) {
            TokenList& tokensInLine = mTokensByLine[line];
            vecIter it = tokensInLine.begin();
            vecIter whereToInsert = it;
            int column = token.location.columnNumber;
            for(++it; it != tokensInLine.end(); ++it) {
                if (column < (*it)->location.columnNumber) {
                    whereToInsert = it;
                    break;
                }
            }

            tokensInLine.insert();

            // Adjust the column values of all tokens that come after
            // the newly inserted one
            int length = token.value.length();
            for (it = whereToInsert; it != tokensInLine.end(); ++it) {
                (*it)->location.columnNumber += length;
            }
        }

        return pNewToken;
    } else {
        return &(*foundToken);
    }
}

bool DocumentTokenizer::removeToken(const Token& token)
{
    typedef TokenCollection::iterator iterator;

    iterator foundToken = mTokens.find(token);
    if (foundToken != mTokens.end()) {
        mTokens.erase(foundToken);
        return true;
    }

    return false;
}

bool DocumentTokenizer::removeToken(const Token* token)
{
    return removeToken(*token);
}

void DocumentTokenizer::onDocumentContentsChanged(int position, int charsRemoved, int charsAdded)
{

}
