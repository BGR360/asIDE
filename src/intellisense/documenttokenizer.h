#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>

#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    QSet<Token*> tokens();

    bool hasToken(Token* token) const;
    bool hasToken(const Token& token) const;

    QVector<Token*> getTokensInLine(int lineNumber) const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const QVector<Token*>& tokens);
    void tokensRemoved(const QVector<Token*>& tokens);

protected:
    Token* addToken(const QString& value, Token::TokenType type, int line, int column);
    Token* addToken(const Token& token);
    bool removeToken(const QString& value, int line);
    bool removeToken(const Token& token);
    bool removeToken(const Token* token);

private slots:
    void onDocumentContentsChanged(int position, int charsRemoved,
                                   int charsAdded);

private:
    QTextDocument* mDoc;

    QSet<Token> mTokens;
    QMap<int, QVector<Token*> > mTokensByLine;
};

#endif // DOCUMENTTOKENIZER_H
