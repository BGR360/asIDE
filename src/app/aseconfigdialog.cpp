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

#include "aseconfigdialog.h"
#include "ui_aseconfigdialog.h"

#include <QDir>
#include <QFileDialog>
#include <QLineEdit>

AseConfigDialog::AseConfigDialog(QWidget* parent, const QString& pathToAse100) :
    QDialog(parent),
    ui(new Ui::AseConfigDialog)
{
    ui->setupUi(this);

    setPath(pathToAse100);

    connect(ui->pathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_path_changed(const QString&)));
}

AseConfigDialog::~AseConfigDialog()
{
    delete ui;
}

QString AseConfigDialog::getPath() const
{
    return pathToAse100;
}

void AseConfigDialog::setPath(const QString& str)
{
    ui->pathLineEdit->setText(str);
}

void AseConfigDialog::on_pathButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Select path to ase100"),
                                                    QDir::homePath());
    if (!filePath.isEmpty())
        setPath(filePath);
}

void AseConfigDialog::on_path_changed(const QString& str)
{
    pathToAse100 = str;
}
