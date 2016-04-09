#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QList>
#include <QMap>

#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

typedef QList<Token> TokenList;
typedef QList<const Token*> ConstTokenList;
typedef QMap<int, ConstTokenList> TokenLineMap;

class DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc = 0);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    ConstTokenList tokens();
    ConstTokenList tokensInLine(int lineNumber) const;
    int numTokens() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const ConstTokenList& tokens);
    void tokensRemoved(const ConstTokenList& tokens);

protected:
    const Token* addToken(const QString& value, Token::TokenType type, int line, int column);
    const Token* addToken(const Token& token, int line, int column);
    bool removeToken(const Token& token);
    bool removeToken(const Token* token);

    void reset();

private slots:
    void onDocumentContentsChanged(int position, int charsRemoved,
                                   int charsAdded);

private:
    QTextDocument* mDoc;

    TokenList mTokens;
    TokenLineMap mTokensByLine;
};

#endif // DOCUMENTTOKENIZER_H
