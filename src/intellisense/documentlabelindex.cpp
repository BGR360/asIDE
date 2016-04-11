#include "documentlabelindex.h"

#include "documenttokenizer.h"

DocumentLabelIndex::DocumentLabelIndex(QTextDocument* doc) :
    mTokenizer(0)
{
    setDocument(doc);
}

DocumentLabelIndex::~DocumentLabelIndex()
{
    if (mTokenizer)
        delete mTokenizer;
}

QTextDocument* DocumentLabelIndex::document()
{
    if (mTokenizer)
        return mTokenizer->document();
    else
        return NULL;
}

void DocumentLabelIndex::setDocument(QTextDocument* doc)
{
    if (mTokenizer)
        delete mTokenizer;

    mTokenizer = new DocumentTokenizer(doc);
    emit documentChanged(doc);
}

DocumentTokenizer* DocumentLabelIndex::tokenizer()
{
    return mTokenizer;
}

bool DocumentLabelIndex::hasLabel(const QString& label) const
{
    return false;
}

int DocumentLabelIndex::lineNumberOfLabel(const QString& label) const
{
    return 0;
}

QList<QString> DocumentLabelIndex::labels() const
{
    return QList<QString>();
}

void DocumentLabelIndex::reset()
{
    mLabels.clear();
    mLabelsByLine.clear();
    mLinesByLabel.clear();
}
