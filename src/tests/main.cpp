#include <QTest>

#include "documenttokenizertest.h"

int main(int argc, char** argv)
{
    DocumentTokenizerTest tokenizerTest;
    QTest::qExec(&tokenizerTest, argc, argv);

    return 0;
}
