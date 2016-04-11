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

class DocumentTokenizer;

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
    int lineNumberOfLabel(const QString& label) const;
    QList<QString> labels() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void labelAdded(const QString& label, int line);
    void labelRemoved(const QString& label, int line);

private:
    DocumentTokenizer* mTokenizer;

    QSet<QString> mLabels;
    QMap<int, const QString*> mLabelsByLine;
    QMap<const QString*, int> mLinesByLabel;

    void reset();
};

#endif // DOCUMENTLABELINDEX_H
