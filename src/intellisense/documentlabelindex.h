#ifndef DOCUMENTLABELINDEX_H
#define DOCUMENTLABELINDEX_H

#include "intellisense_global.h"

#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

#include "documenttokenizer.h"

class INTELLISENSE_EXPORT DocumentLabelIndex : public QObject
{
    Q_OBJECT

public:
    DocumentLabelIndex(QTextDocument* doc = 0);
    virtual ~DocumentLabelIndex();

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    DocumentTokenizer* tokenizer();

    bool hasLabel(const QString& label) const;
    bool hasLabelAtLine(const QString& label, int line) const;
    bool hasLine(int line) const;
    int lineNumberOfLabel(const QString& label) const;
    QString labelAtLine(int line) const;
    QList<QString> labels() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void labelAdded(const QString& label, int line);
    void labelRemoved(const QString& label, int line);

protected:
    void reset();
    void readFromTokenizer();
    QString readLabelsFromLine(const TokenList& tokensInLine);
    void addLabel(const QString& label, int line);
    void removeLabel(const QString& label, int line);

private slots:
    void onTokensAdded(const TokenList& tokens, int line);
    void onTokensRemoved(const TokenList& tokens, int line);

private:
    DocumentTokenizer* mTokenizer;

    QSet<QString> mLabels;
    QMap<int, const QString*> mLabelsByLine;
    QMap<const QString*, int> mLinesByLabel;

    const QString& findLabelRef(const QString& label) const;
};

#endif // DOCUMENTLABELINDEX_H
