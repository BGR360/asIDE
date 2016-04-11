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

typedef QSet<QString> LabelList;

class INTELLISENSE_EXPORT DocumentLabelIndex : public QObject
{
public:
    DocumentLabelIndex(QTextDocument* doc = 0);

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    bool hasLabel(const QString& label) const;
    int lineNumberOfLabel(const QString& label) const;
    QVector<QString> labels() const;
    QVector<QString> labelsInLine(int lineNumber) const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void labelAdded(const QString& label, int line);
    void labelRemoved(const QString& label, int line);

private:
    QTextDocument* mDoc;

    QMap<int, LabelList> mLabelsByLine;
};

#endif // DOCUMENTLABELINDEX_H
