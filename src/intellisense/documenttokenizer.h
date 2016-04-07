#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>

#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

typedef QSet<Token> TokenCollection;
typedef QList<Token*> TokenList;
typedef QList<const Token*> ConstTokenList;

class DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    TokenCollection tokens();

    bool hasToken(const Token* token) const;
    bool hasToken(const Token& token) const;

    ConstTokenList getTokensInLine(int lineNumber) const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const TokenList& tokens);
    void tokensRemoved(const TokenList& tokens);

protected:
    const Token* addToken(const QString& value, Token::TokenType type, int line, int column);
    const Token* addToken(const Token& token);
    bool removeToken(const Token& token);
    bool removeToken(const Token* token);

private slots:
    void onDocumentContentsChanged(int position, int charsRemoved,
                                   int charsAdded);

private:
    QTextDocument* mDoc;

    ConstTokenList mTokens;
    QMap<int, TokenList> mTokensByLine;
};

#endif // DOCUMENTTOKENIZER_H
