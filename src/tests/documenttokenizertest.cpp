#include "documenttokenizertest.h"

#include <QTest>

#include <QTextDocument>
#include <QVector>
#include <documenttokenizer.h>
#include <token.h>

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
                                                    {"\n", Token::Newline},
                                                    {"label2", Token::Label}
                                                });
    QTest::newRow("4 full lines") << "// here we do a thing\n"
                                     "function_thing\tadd\ta\tb\tc\n"
                                     "\t\tret\tfunc_ra\n"
                                     "a\t0x0012\t?" <<
                                     QVector<Token>({
                                                        {"// here we do a thing", Token::Comment},
                                                        {"\n",  Token::Newline},
                                                        {"function_thing", Token::Label},
                                                        {"add", Token::Instruction},
                                                        {"a",   Token::Label},
                                                        {"b",   Token::Label},
                                                        {"c",   Token::Label},
                                                        {"\n",  Token::Newline},
                                                        {"ret", Token::Instruction},
                                                        {"func_ra", Token::Label},
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
    ConstTokenList tokens = DocumentTokenizer(&doc).tokens();

    QCOMPARE(tokens.size(), expectedTokens.size());

    ConstTokenList::const_iterator i = tokens.constBegin();
    QVector<Token>::const_iterator e = expectedTokens.constBegin();
    for (; i != tokens.end(); ++i, ++e) {
        QCOMPARE(**i, *e);
    }
}
