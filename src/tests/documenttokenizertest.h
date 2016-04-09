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
    void testLines_data();
    void testLines();
    void documents_data();
    void documents();
    void testInsert_data();
    void testInsert();
};

#endif // DOCUMENTTOKENIZERTEST_H
