#include "codeeditwidget.h"
#include "ui_codeeditwidget.h"

#include <QFont>
#include <QFontMetrics>

#include "syntaxhighlighter.h"

CodeEditWidget::CodeEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeEditWidget)
{
    ui->setupUi(this);

    setupTextEdit();
    setupSyntaxHighlighter();
}

CodeEditWidget::~CodeEditWidget()
{
    delete ui;
}

void CodeEditWidget::setupTextEdit()
{
    // Set the font to a monospace font
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(FONT_SIZE);

    ui->textEdit->setFont(font);

    // Set the tab width
    QFontMetrics metrics(font);
    ui->textEdit->setTabStopWidth(TAB_WIDTH * metrics.width(' '));

    // Set the cursor width
    ui->textEdit->setCursorWidth(CURSOR_WIDTH);
}

void CodeEditWidget::setupSyntaxHighlighter()
{
    QTextDocument* doc = ui->textEdit->document();
    highlighter = new SyntaxHighlighter(doc);
}
