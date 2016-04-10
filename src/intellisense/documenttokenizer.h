#ifndef DOCUMENTTOKENIZER_H
#define DOCUMENTTOKENIZER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTextCursor>

#include "intellisense_global.h"
#include "token.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

typedef QList<Token> TokenList;
typedef QVector<TokenList> TokenLineMap;

class INTELLISENSE_EXPORT DocumentTokenizer : public QObject
{
    Q_OBJECT

public:
    explicit DocumentTokenizer(QTextDocument* doc = 0);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    TokenList tokens();
    TokenList tokensInLine(int lineNumber) const;
    int numTokens() const;
    int numLines() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void tokensAdded(const TokenList& tokens, int lineNumber);
    void tokensRemoved(const TokenList& tokens, int lineNumber);

protected:
    void addLine();
    void removeLine(int lineNumber);
    void setLine(const TokenList& tokens, int line);

    void reset();
    void parse();
    void parse(int beginPos, int endPos);
    TokenList parseLine(const QString& line);
    Token parseWord(const QString& word);

private slots:
    void onDocumentContentsChanged();
    void onCursorPositionChanged(const QTextCursor& cursor);
    void onLineCountChange(int newLineCount);

private:
    QTextDocument* mDoc;
    QTextCursor mCursor;
    int mPrevCursorPos;
    int mNextCursorPos;
    int mPrevCursorLine;

    TokenList mTokens;
    TokenLineMap mTokensByLine;
};

#endif // DOCUMENTTOKENIZER_H
