#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QTextEdit>

#include "syntaxhighlighter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    highlighter(0)
{
    ui->setupUi(this);

    connectSignalsAndSlots();
    setupActions();
    setupFont();
    setupSyntaxHighlighter();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;
}

void MainWindow::connectSignalsAndSlots()
{
    // Pair each action with a corresponding behavior in the MainWindow
}

void MainWindow::setupActions()
{
    // Attempt to give each action an icon from the system theme
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy", ui->actionCopy->icon()));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut", ui->actionCut->icon()));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste", ui->actionPaste->icon()));

    ui->actionNew->setIcon(QIcon::fromTheme("document-new", ui->actionNew->icon()));
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open", ui->actionOpen->icon()));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save", ui->actionSave->icon()));
    ui->actionSaveAs->setIcon(QIcon::fromTheme("document-save-as", ui->actionSaveAs->icon()));

    ui->actionInfo->setIcon(QIcon::fromTheme("help-about", ui->actionInfo->icon()));
}

void MainWindow::setupFont()
{
    // Set the font to a monospace font
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(14);

    ui->editor->setFont(font);

    // Set the tab width to 8 spaces
    const int tabStop = 8;
    QFontMetrics metrics(font);
    ui->editor->setTabStopWidth(tabStop * metrics.width(' '));
}

void MainWindow::setupSyntaxHighlighter()
{
    QTextDocument* doc = ui->editor->document();
    highlighter = new SyntaxHighlighter(doc);
}
