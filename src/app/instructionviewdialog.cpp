#include "instructionviewdialog.h"
#include "ui_instructionviewdialog.h"

#include <QDebug>
#include <QFile>
#include <QTextBrowser>

InstructionViewDialog::InstructionViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstructionViewDialog)
{
    ui->setupUi(this);
    QFile htmlFile(":/html/e100-instructions.html");
    if (htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textBrowser->setHtml(htmlFile.readAll());
    } else {
        qDebug() << "Could not load e100 instructions file.";
    }
}

InstructionViewDialog::~InstructionViewDialog()
{
    delete ui;
}
