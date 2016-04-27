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

#include "tokenviewdialog.h"
#include "ui_tokenviewdialog.h"

#include "codeeditwidget.h"
#include <documenttokenizer.h>

TokenViewDialog::TokenViewDialog(QWidget* parent, CodeEditWidget* editor) :
    QDialog(parent),
    ui(new Ui::TokenViewDialog),
    editor(NULL)
{
    ui->setupUi(this);
    ui->listView->setModel(&tokenModel);

    setEditor(editor);
}

TokenViewDialog::~TokenViewDialog()
{
    delete ui;
}

CodeEditWidget*TokenViewDialog::codeEditor()
{
    return editor;
}

void TokenViewDialog::setEditor(CodeEditWidget* newEditor)
{
    if (newEditor == editor)
        return;

    if (editor) {
        DocumentTokenizer* tokenizer = editor->labelIndex()->tokenizer();
        if (tokenizer) {
            disconnect(tokenizer, SIGNAL(tokensAdded(TokenList,int)), this, SLOT(onTokensAdded()));
            disconnect(tokenizer, SIGNAL(tokensRemoved(TokenList,int)), this, SLOT(onTokensRemoved()));
        }
    }

    editor = newEditor;

    if (editor) {
        QString fileName(editor->fileName());
        setWindowTitle(QString("Tokens View for %1").arg(fileName));

        updateTokens();

        DocumentTokenizer* tokenizer = editor->labelIndex()->tokenizer();
        if (tokenizer) {
            connect(tokenizer, SIGNAL(tokensAdded(TokenList,int)), this, SLOT(onTokensAdded()));
            connect(tokenizer, SIGNAL(tokensRemoved(TokenList,int)), this, SLOT(onTokensRemoved()));
        }
    } else {
        tokenModel.setStringList(QStringList());
    }
}

void TokenViewDialog::onTokensAdded()
{
    if (isVisible())
        updateTokens();
}

void TokenViewDialog::onTokensRemoved()
{
    if (isVisible())
        updateTokens();
}

void TokenViewDialog::updateTokens()
{
    QStringList tokenStrings;
    DocumentTokenizer* tokenizer = editor->labelIndex()->tokenizer();
    for (int i = 0; i < tokenizer->numLines(); ++i) {
        TokenList tokensInLine = tokenizer->tokensInLine(i);
        foreach (const Token& token, tokensInLine) {
            QString tokenString = QString("%1: %2: %3").arg(i).arg(Token::TYPE_NAMES[token.type]).arg(token.value);
            tokenStrings.append(tokenString);
        }
    }
    tokenModel.setStringList(tokenStrings);
}
