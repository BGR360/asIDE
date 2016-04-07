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

QSet<Token*> DocumentTokenizer::tokens()
{
    return QSet<Token*>();
}

bool DocumentTokenizer::hasToken(Token* token) const
{
    return false;
}

bool DocumentTokenizer::hasToken(const Token& token) const
{
    return false;
}

QVector<Token*> DocumentTokenizer::getTokensInLine(int lineNumber) const
{
    return QVector<Token*>();
}

Token* DocumentTokenizer::addToken(const QString& value, Token::TokenType type, int line, int column)
{
    return NULL;
}

Token* DocumentTokenizer::addToken(const Token& token)
{
    return NULL;
}

bool DocumentTokenizer::removeToken(const QString& value, int line)
{
    return false;
}

bool DocumentTokenizer::removeToken(const Token& token)
{
    return false;
}

bool DocumentTokenizer::removeToken(const Token* token)
{
    return false;
}

void DocumentTokenizer::onDocumentContentsChanged(int position, int charsRemoved, int charsAdded)
{

}
