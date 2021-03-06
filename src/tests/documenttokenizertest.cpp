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

#include "documenttokenizertest.h"

#include <QTest>

#include <QSignalSpy>
#include <QTextCursor>
#include <QTextDocument>
#include <QVector>

#include <documenttokenizer.h>
#include <token.h>

static const Token NEWLINE = {"\n", Token::Newline};

void compareTokenLists(const TokenList& actual, const QVector<Token>& expected)
{
    if (actual.size() != expected.size()) {
        qDebug() << "Number of tokens differs";
        qDebug() << "Actual:";
        foreach (const Token& token, actual) {
            qDebug() << "   " << token;
        }
        qDebug() << "Expected:";
        foreach (const Token& token, expected) {
            qDebug() << "   " << token;
        }
        QFAIL("Number of tokens differs");
    }

    TokenList::const_iterator i = actual.constBegin();
    QVector<Token>::const_iterator e = expected.constBegin();
    for (; i != actual.constEnd() && e != expected.constEnd(); ++i, ++e) {
        const char* failure = QString("Token values differ:\n"
                                      "Actual:   %1: %2\n"
                                      "Expected: %3: %4\n")
                .arg(Token::TYPE_NAMES[i->type]).arg(i->value)
                .arg(Token::TYPE_NAMES[e->type]).arg(e->value).toStdString().c_str();

        QVERIFY2(i->value == e->value, failure);
        QVERIFY2(i->type == e->type, failure);
    }
}

DocumentTokenizerTest::DocumentTokenizerTest()
{

}

void DocumentTokenizerTest::testEmptyDocument()
{
    QTextDocument doc("");
    DocumentTokenizer tokenizer(&doc);
    QVERIFY(tokenizer.numTokens() == 0);
    QVERIFY(tokenizer.tokens().size() == 0);
}

void DocumentTokenizerTest::testNoDocument()
{
    DocumentTokenizer tokenizer;
    QVERIFY(tokenizer.numTokens() == 0);
}

void DocumentTokenizerTest::testLines_data()
{
    QTest::addColumn<QString>("docText");
    QTest::addColumn<int>("numLines");

    QTest::newRow("1 line") << "label" << 1;
    QTest::newRow("2 lines (2nd empty)") << "label\n" << 2;
    QTest::newRow("3 lines") << "label1\n"
                                "label2\n"
                                "label3" << 3;
    QTest::newRow("6 lines misc") << "a\tb\n"
                                     "  \n"
                                     "\n"
                                     "hello\r\n"
                                     "adsfj;vn;sakjdl;v ans;dkjfvv  \n"
                                     "1" << 6;
}

void DocumentTokenizerTest::testLines()
{
    QFETCH(QString, docText);
    QFETCH(int, numLines);

    QTextDocument doc(docText);
    int result = DocumentTokenizer(&doc).numLines();

    QCOMPARE(result, numLines);
}

void DocumentTokenizerTest::documents_data()
{
    QTest::addColumn<QString>("docText");
    QTest::addColumn<QVector<Token> >("expectedTokens");

    QTest::newRow("One label") << "label" <<
                                  QVector<Token>({
                                                     {"label", Token::Label}
                                                 });

    QTest::newRow("Label with spaces") << "label  " <<
                                          QVector<Token>({
                                                             {"label", Token::Label}
                                                         });

    QTest::newRow("2 labels") << "label1\n"
                                 "label2" <<
                                 QVector<Token>({
                                                    {"label1", Token::Label},
                                                    NEWLINE,
                                                    {"label2", Token::Label}
                                                });

    QTest::newRow("comment") << "// this is a comment" <<
                                QVector<Token>({
                                                   {"// this is a comment", Token::Comment}
                                               });

    QTest::newRow("comment and line") << "// spooky comment  \n"
                                         "line" <<
                                         QVector<Token>({
                                                            {"// spooky comment  ", Token::Comment},
                                                            NEWLINE,
                                                            {"line", Token::Label}
                                                        });

    QTest::newRow("4 full lines") << "// here we do a thing\n"
                                     "function_thing\tadd\ta\tb\tc\n"
                                     "\t\tret\tfunc_ra\n"
                                     "a\t0x12\t?" <<
                                     QVector<Token>({
                                                        {"// here we do a thing", Token::Comment},
                                                        NEWLINE,
                                                        {"function_thing", Token::Label},
                                                        {"add", Token::Instruction},
                                                        {"a",   Token::Label},
                                                        {"b",   Token::Label},
                                                        {"c",   Token::Label},
                                                        NEWLINE,
                                                        {"ret", Token::Instruction},
                                                        {"func_ra", Token::Label},
                                                        NEWLINE,
                                                        {"a",   Token::Label},
                                                        {"0x12",  Token::IntLiteral},
                                                        {"?",   Token::Unrecognized}
                                                    });
}

void DocumentTokenizerTest::documents()
{
    QFETCH(QString, docText);
    QFETCH(QVector<Token>, expectedTokens);

    QTextDocument doc(docText);
    DocumentTokenizer tokenizer(&doc);
    TokenList tokens = tokenizer.tokens();

    compareTokenLists(tokens, expectedTokens);
}

void DocumentTokenizerTest::testInsert_data()
{
    QTest::addColumn<QString>("initialText");
    QTest::addColumn<QVector<Token> >("expInitialTokens");
    QTest::addColumn<int>("anchorPosition");
    QTest::addColumn<int>("position");
    QTest::addColumn<QString>("insertedText");
    QTest::addColumn<QVector<Token> >("expFinalTokens");

    QTest::newRow("empty->label") << "" <<
                                     QVector<Token>() <<
                                     0 << 0 << "label" <<
                                     QVector<Token>({{"label", Token::Label}});

    QTest::newRow("empty->labels") << "" <<
                                      QVector<Token>() <<
                                      0 << 0 <<
                                      "label1 0\n"
                                      "label2 0\n"
                                      "label3 0" <<
                                      QVector<Token>({
                                                         {"label1", Token::Label},
                                                         {"0", Token::IntLiteral},
                                                         NEWLINE,
                                                         {"label2", Token::Label},
                                                         {"0", Token::IntLiteral},
                                                         NEWLINE,
                                                         {"label3", Token::Label},
                                                         {"0", Token::IntLiteral}
                                                     });

    QTest::newRow("label->newline") << "label" <<
                                       QVector<Token>({{"label", Token::Label}}) <<
                                       5 << 5 << "\nnew_label" <<
                                       QVector<Token>({
                                                          {"label", Token::Label},
                                                          NEWLINE,
                                                          {"new_label", Token::Label}
                                                      });

    QTest::newRow("label in middle") << "label1 label2" <<
                                        QVector<Token>({
                                                           {"label1", Token::Label},
                                                           {"label2", Token::Label}
                                                       }) <<
                                        6 << 6 << " middle_label" <<
                                        QVector<Token>({
                                                           {"label1", Token::Label},
                                                           {"middle_label", Token::Label},
                                                           {"label2", Token::Label}
                                                       });

    QTest::newRow("bn->bne") << "bn" <<
                                QVector<Token>({{"bn", Token::Label}}) <<
                                2 << 2 << "e" <<
                                QVector<Token>({{"bne", Token::Instruction}});

    QTest::newRow("label->label69") << "label" <<
                                       QVector<Token>({{"label", Token::Label}}) <<
                                       5 << 5 << "69" <<
                                       QVector<Token>({{"label69", Token::Label}});

    QTest::newRow("comment out") << "whole lotta labels" <<
                                    QVector<Token>({
                                                       {"whole", Token::Label},
                                                       {"lotta", Token::Label},
                                                       {"labels", Token::Label}
                                                   }) <<
                                    5 << 5 << " //" <<
                                    QVector<Token>({
                                                       {"whole", Token::Label},
                                                       {"// lotta labels", Token::Comment}
                                                   });

    QTest::newRow("split label") << "big_label" <<
                                    QVector<Token>({{"big_label", Token::Label}}) <<
                                    7 << 7 << " " <<
                                    QVector<Token>({
                                                       {"big_lab", Token::Label},
                                                       {"el", Token::Label}
                                                   });



    QTest::newRow("label-->instruction") << "label" <<
                                            QVector<Token>({{"label", Token::Label}}) <<
                                            0 << 5 << "call" <<
                                            QVector<Token>({{"call", Token::Instruction}});

    QTest::newRow("lines --> one line") << "label1\n"
                                           "label2\n"
                                           "\n"
                                           "hi" <<
                                           QVector<Token>({
                                                              {"label1", Token::Label},
                                                              NEWLINE,
                                                              {"label2", Token::Label},
                                                              NEWLINE,
                                                              NEWLINE,
                                                              {"hi", Token::Label}
                                                          }) <<
                                           3 << 17 << "\tcpfa" <<
                                           QVector<Token>({
                                                              {"lab", Token::Label},
                                                              {"cpfa", Token::Instruction}
                                                          });
}

void DocumentTokenizerTest::testInsert()
{
    QFETCH(QString, initialText);
    QFETCH(QVector<Token>, expInitialTokens);
    QFETCH(int, anchorPosition);
    QFETCH(int, position);
    QFETCH(QString, insertedText);
    QFETCH(QVector<Token>, expFinalTokens);

    QTextDocument doc(initialText);
    QTextCursor cursor(&doc);
    DocumentTokenizer tokenizer(&doc);

    QSignalSpy spy(&doc, SIGNAL(contentsChange(int, int, int)));

    // Compare the initial list of tokens
    TokenList initialTokens = tokenizer.tokens();
    compareTokenLists(initialTokens, expInitialTokens);

    // Insert the text

    qDebug() << doc.toPlainText();

    cursor.setPosition(anchorPosition);
    QTextCursor::MoveMode moveMode = (anchorPosition == position) ?
                QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
    cursor.setPosition(position, moveMode);
    cursor.insertText(insertedText);

    qDebug() << doc.toPlainText();

    qDebug() << spy.count();

    // Compare the final list of tokens
    TokenList finalTokens = tokenizer.tokens();
    compareTokenLists(finalTokens, expFinalTokens);
}

void DocumentTokenizerTest::testSignalSpy()
{
    QTextDocument* doc = new QTextDocument("");
    QTextCursor* cursor = new QTextCursor(doc);
    DocumentTokenizer tokenizer(doc);

    QSignalSpy spy1(doc, SIGNAL(contentsChange(int, int, int)));
    QSignalSpy spy2(doc, SIGNAL(contentsChanged()));

    // Compare the initial list of tokens
    doc->setPlainText("ello love");

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    doc->setPlainText("hi");

    QCOMPARE(spy1.count(), 2);
    QCOMPARE(spy2.count(), 2);

    delete doc;
    delete cursor;
}

void DocumentTokenizerTest::testCursorMove()
{
    QTextDocument* doc = new QTextDocument("ello love");
    QTextCursor* cursor = new QTextCursor(doc);
    DocumentTokenizer tokenizer(doc);

    cursor->setPosition(4);
    cursor->insertText(" ");
    qDebug() << cursor;
    cursor->deleteChar();
    cursor->insertText("gov ");

    delete doc;
    delete cursor;
}

//QTEST_MAIN(DocumentTokenizerTest)
