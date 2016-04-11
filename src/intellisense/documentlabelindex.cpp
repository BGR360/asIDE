#include "documentlabelindex.h"

DocumentLabelIndex::DocumentLabelIndex(QTextDocument* doc) :
    mDoc(0)
{
    setDocument(doc);
}

QTextDocument* DocumentLabelIndex::document()
{
    return mDoc;
}

void DocumentLabelIndex::setDocument(QTextDocument* doc)
{
    mDoc = doc;
    emit documentChanged(doc);
}

bool DocumentLabelIndex::hasLabel(const QString& label) const
{
    return false;
}

int DocumentLabelIndex::lineNumberOfLabel(const QString& label) const
{
    return 0;
}

QVector<QString> DocumentLabelIndex::labels() const
{
    return QVector<QString>();
}

QVector<QString> DocumentLabelIndex::labelsInLine(int lineNumber) const
{
    return QVector<QString>();
}
