#include "documentlabelindextest.h"

#include <QSignalSpy>
#include <QTest>
#include <QTextDocument>
#include <QVector>

#include "documentlabelindex.h"

void DocumentLabelIndexTest::testDocuments_data()
{
    QTest::addColumn<QString>("docText");
    QTest::addColumn<QVector<QString> >("labels");
    QTest::addColumn<QVector<bool> >("expectedBools");
    QTest::addColumn<QVector<int> >("expectedLines");

    QTest::newRow("one declared label") << "label\t0" <<
                                           QVector<QString>({"label", "0"}) <<
                                           QVector<bool>({true, false}) <<
                                           QVector<int>({0, -1});

    QTest::newRow("label after instruction") << "\t\tbne\tlabel" <<
                                                QVector<QString>({"bne", "label"}) <<
                                                QVector<bool>({false, false}) <<
                                                QVector<int>({-1, -1});

    QTest::newRow("label after tab") << "\t\tlabel\t0" <<
                                        QVector<QString>({"label", "0"}) <<
                                        QVector<bool>({false, false}) <<
                                        QVector<int>({-1, -1});

    QTest::newRow("three declared labels") << "label1\t0\n"
                                              "label2\t0\n"
                                              "label3\t0\n" <<
                                              QVector<QString>({"label1", "label2", "label3", "0"}) <<
                                              QVector<bool>({true, true, true, false}) <<
                                              QVector<int>({0, 1, 2, -1});

    QTest::newRow("label gets address of other") << "label\t0\n"
                                                    "ptr\tlabel" <<
                                                    QVector<QString>({"label", "0", "ptr"}) <<
                                                    QVector<bool>({true, false, true}) <<
                                                    QVector<int>({0, -1, 1});
}

void DocumentLabelIndexTest::testDocuments()
{
    QFETCH(QString, docText);
    QFETCH(QVector<QString>, labels);
    QFETCH(QVector<bool>, expectedBools);
    QFETCH(QVector<int>, expectedLines);

    QTextDocument doc(docText);
    DocumentLabelIndex index(&doc);

    const int numLabels = labels.size();
    for (int i = 0; i < numLabels; ++i) {
        QString label = labels[i];

        bool expectedBool = expectedBools[i];
        bool resultBool = index.hasLabel(label);
        const char* message = QString("index.hasLabel(%1) should not be %2")
                .arg(label)
                .arg(resultBool)
                .toStdString().c_str();
        QVERIFY2(expectedBool == resultBool, message);

        int expectedLine = expectedLines[i];
        int resultLine = index.lineNumberOfLabel(label);
        message = QString("Line number of %1 should be %2, not %3")
                .arg(label)
                .arg(expectedLine)
                .arg(resultLine)
                .toStdString().c_str();
        QVERIFY2(expectedLine == resultLine, message);
    }
}

typedef QVector<QVector<QVariant> >  ParamList;

void DocumentLabelIndexTest::testSignals_data()
{
    QTest::addColumn<QString>("initialDocText");
    QTest::addColumn<QString>("finalDocText");
    QTest::addColumn<ParamList>("expectedAdded");
    QTest::addColumn<ParamList>("expectedRemoved");

    QTest::newRow("blank -> label") << "" << "label\t0" <<
                                       ParamList({
                                                     {"label", 0}
                                                 }) <<
                                       ParamList();

    QTest::newRow("blank -> labels") << "" <<
                                        "label1 0\n"
                                        "label2 0\n"
                                        "label3 0" <<
                                        ParamList({
                                                      {"label1", 0},
                                                      {"label2", 1},
                                                      {"label3", 2}
                                                  }) <<
                                        ParamList();

    QTest::newRow("remove one label") << "label1 0\n"
                                         "label2 0\n"
                                         "label3 0" <<

                                         "label1 0\n"
                                         "label3 0" <<
                                         ParamList({
                                                       {"label3", 1}
                                                   }) <<
                                         ParamList({
                                                       {"label2", 1},
                                                       {"label3", 2}
                                                   });
    QTest::newRow("remove all labels") << "label1 0\n"
                                         "label2 0\n"
                                         "label3 0" << "" <<
                                         ParamList() <<
                                         ParamList({
                                                       {"label1", 0},
                                                       {"label2", 1},
                                                       {"label3", 2}
                                                   });

    QTest::newRow("add label to empty line") << "label1 0\n"
                                                "\n"
                                                "label3 0" <<

                                                "label1 0\n"
                                                "label2 0\n"
                                                "label3 0" <<
                                                ParamList({
                                                              {"label2", 1}
                                                          }) <<
                                                ParamList();
}

void DocumentLabelIndexTest::testSignals()
{
    QFETCH(QString, initialDocText);
    QFETCH(QString, finalDocText);
    QFETCH(ParamList, expectedAdded);
    QFETCH(ParamList, expectedRemoved);

    QTextDocument doc(initialDocText);
    DocumentLabelIndex index(&doc);

    QSignalSpy addedSpy(&index, SIGNAL(labelAdded(QString,int)));
    QSignalSpy removedSpy(&index, SIGNAL(labelRemoved(QString,int)));

    doc.setPlainText(finalDocText);

    QCOMPARE(addedSpy.count(), expectedAdded.size());
    QCOMPARE(removedSpy.count(), expectedRemoved.size());

    QSignalSpy::iterator e = addedSpy.begin();
    ParamList::iterator i = expectedAdded.begin();
    for (; e != addedSpy.end(); ++e, ++i) {
        QString expectedLabel = e->at(0).toString();
        QString resultLabel = i->at(0).toString();
        QCOMPARE(resultLabel, expectedLabel);

        int expectedLine = e->at(1).toInt();
        int resultLine = i->at(1).toInt();
        QCOMPARE(resultLine, expectedLine);
    }

    e = removedSpy.begin();
    i = expectedRemoved.begin();
    for (; e != addedSpy.end(); ++e, ++i) {
        QString expectedLabel = e->at(0).toString();
        QString resultLabel = i->at(0).toString();
        QCOMPARE(resultLabel, expectedLabel);

        int expectedLine = e->at(1).toInt();
        int resultLine = i->at(1).toInt();
        QCOMPARE(resultLine, expectedLine);
    }
}
