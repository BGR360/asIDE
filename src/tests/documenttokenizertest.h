#ifndef DOCUMENTTOKENIZERTEST_H
#define DOCUMENTTOKENIZERTEST_H

#include <QObject>

class DocumentTokenizerTest : public QObject
{
    Q_OBJECT
public:
    DocumentTokenizerTest();

private slots:
    void testEmptyDocument();
    void testNoDocument();
    void documents_data();
    void documents();
};

#endif // DOCUMENTTOKENIZERTEST_H
