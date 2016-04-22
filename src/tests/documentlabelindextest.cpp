//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//  The MIT License (MIT)                                                                                   //
//  Copyright (c) 2016 Benjamin Reeves                                                                      //
//                                                                                                          //
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software           //
//  and associated documentation files (the "Software"), to deal in the Software without restriction,       //
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,   //
//  subject to the following conditions:                                                                    //
//                                                                                                          //
//  The above copyright notice and this permission notice shall be included in all copies or substantial    //
//  portions of the Software.                                                                               //
//                                                                                                          //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT   //
//  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, //
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                  //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "documentlabelindextest.h"

#include <QSet>
#include <QSignalSpy>
#include <QTest>
#include <QTextCursor>
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
                                        QVector<bool>({true, false}) <<
                                        QVector<int>({0, -1});

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

typedef QList<QList<QVariant> >  ParamList;

void DocumentLabelIndexTest::testSignals_data()
{
    QTest::addColumn<QString>("initialDocText");
    QTest::addColumn<int>("cursorStart");
    QTest::addColumn<int>("cursorEnd");
    QTest::addColumn<QString>("insertedText");
    QTest::addColumn<ParamList>("expectedAdded");
    QTest::addColumn<ParamList>("expectedRemoved");

    QTest::newRow("blank -> label") << "" <<
                                       0 << 0 <<
                                       "label\t0" <<
                                       ParamList({
                                                     {"label", 0}
                                                 }) <<
                                       ParamList();

    QTest::newRow("blank -> labels") << "" <<
                                        0 << 0 <<
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
                                         9 << 18 << " " <<
                                         ParamList({
                                                       {"label3", 1}
                                                   }) <<
                                         ParamList({
                                                       {"label2", 1},
                                                       {"label3", 2}
                                                   });
    QTest::newRow("remove all labels") << "label1 0\n"
                                          "label2 0\n"
                                          "label3 0" <<
                                          0 << 26 << " " <<
                                         ParamList() <<
                                         ParamList({
                                                       {"label1", 0},
                                                       {"label2", 1},
                                                       {"label3", 2}
                                                   });

    QTest::newRow("add label to empty line") << "label1 0\n"
                                                "\n"
                                                "label3 0" <<
                                                9 << 9 <<
                                                "label2 0" <<
                                                ParamList({
                                                              {"label2", 1}
                                                          }) <<
                                                ParamList();
}

void DocumentLabelIndexTest::testSignals()
{
    QFETCH(QString, initialDocText);
    QFETCH(int, cursorStart);
    QFETCH(int, cursorEnd);
    QFETCH(QString, insertedText);
    QFETCH(ParamList, expectedAdded);
    QFETCH(ParamList, expectedRemoved);

    QTextDocument doc(initialDocText);
    QTextCursor cursor(&doc);
    DocumentLabelIndex index(&doc);

    QSignalSpy addedSpy(&index, SIGNAL(labelAdded(QString,int)));
    QSignalSpy removedSpy(&index, SIGNAL(labelRemoved(QString,int)));

    cursor.setPosition(cursorStart);
    if (cursorEnd != cursorStart) {
        cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
    }
    cursor.insertText(insertedText);

    qDebug() << "new text:" << doc.toPlainText();

    QCOMPARE(addedSpy.count(), expectedAdded.size());
    QCOMPARE(removedSpy.count(), expectedRemoved.size());

    ParamList::iterator e = expectedAdded.begin();
    for (; e != expectedAdded.end(); ++e) {
        QVERIFY(addedSpy.contains(*e));
    }

    e = expectedRemoved.begin();
    for (; e != expectedRemoved.end(); ++e) {
        QVERIFY(removedSpy.contains(*e));
    }
}
