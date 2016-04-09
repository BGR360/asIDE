#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QList>
#include <QMap>

#include "intellisense_global.h"
#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

typedef QList<Token> TokenList;
typedef QList<Token> ConstTokenList;
typedef QVector<ConstTokenList> TokenLineMap;

class INTELLISENSE_EXPORT DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc = 0);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    ConstTokenList tokens();
    ConstTokenList tokensInLine(int lineNumber) const;
    int numTokens() const;
    int numLines() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const ConstTokenList& tokens);
    void tokensRemoved(const ConstTokenList& tokens);

protected:
    const Token* addToken(const QString& value, Token::TokenType type, int line, int column);
    const Token* addToken(const Token& token, int line, int column);
    void addTokensToLine(const TokenList& tokens, int line);
    bool removeToken(const Token& token);
    bool removeToken(const Token* token);

    void reset();
    void parse();
    void parse(int beginPos, int endPos);
    TokenList parseLine(const QString& line);
    Token parseWord(const QString& word);

private slots:
    void onDocumentContentsChanged(int position, int charsRemoved,
                                   int charsAdded);

private:
    QTextDocument* mDoc;

    TokenList mTokens;
    TokenLineMap mTokensByLine;
};

#endif // DOCUMENTTOKENIZER_H
