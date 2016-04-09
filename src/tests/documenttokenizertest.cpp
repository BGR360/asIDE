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
                                                             {"label", Token::Label},
                                                             {"  ", Token::Whitespace}
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
                                     "a\t0x0012" <<
                                     QVector<Token>({
                                                        {"// here we do a thing", Token::Comment},
                                                        {"\n",  Token::Newline},
                                                        {"function_thing", Token::Label},
                                                        {"\t",  Token::Whitespace},
                                                        {"add", Token::Keyword},
                                                        {"\t",  Token::Whitespace},
                                                        {"a",   Token::Label},
                                                        {"\t",  Token::Whitespace},
                                                        {"b",   Token::Label},
                                                        {"\t",  Token::Whitespace},
                                                        {"c",   Token::Label},
                                                        {"\n",  Token::Newline},
                                                        {"\t\t",Token::Whitespace},
                                                        {"ret", Token::Keyword},
                                                        {"\t",  Token::Whitespace},
                                                        {"func_ra", Token::Label},
                                                        {"a",   Token::Label},
                                                        {"\t",  Token::Whitespace},
                                                        {"0x0012",  Token::Literal}
                                                    });
}

void DocumentTokenizerTest::documents()
{
    QFETCH(QString, docText);
    QFETCH(QVector<Token>, expectedTokens);

    QTextDocument doc(docText);
    ConstTokenList tokens = DocumentTokenizer(&doc).tokens();

    QVERIFY2(tokens.size() == expectedTokens.size(), "Unequal number of tokens");

    ConstTokenList::const_iterator i = tokens.constBegin();
    QVector<Token>::const_iterator e = expectedTokens.constBegin();
    for (; i != tokens.end(); ++i, ++e) {
        QVERIFY(**i == *e);
    }
}
