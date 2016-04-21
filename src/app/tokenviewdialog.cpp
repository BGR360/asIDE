#include "tokenviewdialog.h"
#include "ui_tokenviewdialog.h"

#include "codeeditwidget.h"
#include <documenttokenizer.h>

TokenViewDialog::TokenViewDialog(QWidget* parent, CodeEditWidget* editor) :
    QDialog(parent),
    ui(new Ui::TokenViewDialog),
    editor(0)
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
    }
}

void TokenViewDialog::onTokensAdded()
{
    updateTokens();
}

void TokenViewDialog::onTokensRemoved()
{
    updateTokens();
}

void TokenViewDialog::updateTokens()
{
    QStringList tokenStrings;
    TokenList tokens = editor->labelIndex()->tokenizer()->tokens();
    foreach (const Token& token, tokens) {
        QString tokenString = QString("%1: %2").arg(Token::TYPE_NAMES[token.type]).arg(token.value);
        tokenStrings.append(tokenString);
    }
    tokenModel.setStringList(tokenStrings);
}
