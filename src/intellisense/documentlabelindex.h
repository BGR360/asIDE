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

#ifndef DOCUMENTLABELINDEX_H
#define DOCUMENTLABELINDEX_H

#include "intellisense_global.h"

#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

#include "documenttokenizer.h"

class INTELLISENSE_EXPORT DocumentLabelIndex : public QObject
{
    Q_OBJECT

public:
    DocumentLabelIndex(QTextDocument* doc = 0);
    virtual ~DocumentLabelIndex();

    QTextDocument* document();
    void setDocument(QTextDocument* doc);

    DocumentTokenizer* tokenizer();

    bool hasLabel(const QString& label) const;
    bool hasLabelAtLine(int line) const;
    bool hasLabelAtLine(const QString& label, int line) const;
    bool hasLine(int line) const;
    int lineNumberOfLabel(const QString& label) const;
    QString labelAtLine(int line) const;
    QList<QString> labels() const;

signals:
    void documentChanged(QTextDocument* newDocument);
    void labelAdded(const QString& label, int line);
    void labelRemoved(const QString& label, int line);
    void lineAdded(int afterLine);
    void lineRemoved(int lineNumber);

protected:
    void reset();
    void readFromTokenizer();
    QString readLabelsFromLine(const TokenList& tokensInLine);
    void addLabel(const QString& label, int line);
    void removeLabel(const QString& label, int line);

private slots:
    void onTokensAdded(const TokenList& tokens, int line);
    void onTokensRemoved(const TokenList& tokens, int line);
    void onLineAdded(int afterLine);
    void onLineRemoved(int lineNumber);

private:
    DocumentTokenizer* mTokenizer;

    QSet<QString> mLabels;
    QMap<int, const QString*> mLabelsByLine;
    QMap<const QString*, int> mLinesByLabel;

    const QString& findLabelRef(const QString& label) const;
};

#endif // DOCUMENTLABELINDEX_H
