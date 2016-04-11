#ifndef DOCUMENTLABELINDEXTEST_H
#define DOCUMENTLABELINDEXTEST_H

#include <QObject>

class DocumentLabelIndexTest : public QObject
{
    Q_OBJECT

private slots:
    void testDocuments_data();
    void testDocuments();
    void testSignals_data();
    void testSignals();
};

#endif // DOCUMENTLABELINDEXTEST_H
