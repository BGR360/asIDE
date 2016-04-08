#include "documenttokenizer.h"

#include <QTextDocument>

DocumentTokenizer::DocumentTokenizer(QTextDocument* doc) :
    mDoc(doc)
{
    reset();
}

QTextDocument* DocumentTokenizer::document()
{
    return mDoc;
}

void DocumentTokenizer::setDocument(QTextDocument* doc)
{
    // Disconnect signals and slots from previous document

    mDoc = doc;

    // Connect signals and slots to new documents

}

ConstTokenList DocumentTokenizer::tokens()
{
    ConstTokenList ret;
    TokenList::const_iterator i;
    for (i = mTokens.constBegin(); i != mTokens.constEnd(); ++i) {
        ret.append(&(*i));
    }
    return ret;
}

ConstTokenList DocumentTokenizer::tokensInLine(int lineNumber) const
{
    TokenLineMap::const_iterator found = mTokensByLine.find(lineNumber);
    if (found != mTokensByLine.constEnd())
        return *found;
    else
        return ConstTokenList();
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

void DocumentTokenizer::onDocumentContentsChanged(int position, int charsRemoved, int charsAdded)
{

}
