#include <QTest>

#include "documenttokenizertest.h"
#include "documentlabelindextest.h"

int main(int argc, char* argv[])
{
    DocumentTokenizerTest tokenizerTest;
    QTest::qExec(&tokenizerTest, argc, argv);

    DocumentLabelIndexTest labelIndexTest;
    QTest::qExec(&labelIndexTest, argc, argv);

    return 0;
}
