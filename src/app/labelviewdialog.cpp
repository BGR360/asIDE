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

#include "labelviewdialog.h"
#include "ui_labelviewdialog.h"

#include "codeeditwidget.h"
#include <documentlabelindex.h>

LabelViewDialog::LabelViewDialog(QWidget* parent, CodeEditWidget* editor) :
    QDialog(parent),
    ui(new Ui::LabelViewDialog),
    editor(NULL)
{
    ui->setupUi(this);
    ui->listView->setModel(&labelModel);

    setEditor(editor);
}

LabelViewDialog::~LabelViewDialog()
{
    delete ui;
}

CodeEditWidget* LabelViewDialog::codeEditor()
{
    return editor;
}

void LabelViewDialog::setEditor(CodeEditWidget* newEditor)
{
    if (newEditor == editor)
        return;

    if (editor) {
        DocumentLabelIndex* indexer = editor->labelIndex();
        if (indexer) {
            disconnect(indexer, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelAdded()));
            disconnect(indexer, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelRemoved()));
        }
    }

    editor = newEditor;

    if (editor) {
        QString fileName(editor->fileName());
        setWindowTitle(QString("Labels View for %1").arg(fileName));

        updateLabels();

        DocumentLabelIndex* indexer = editor->labelIndex();
        if (indexer) {
            connect(indexer, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelAdded()));
            connect(indexer, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelRemoved()));
        }
    }
}

void LabelViewDialog::onLabelAdded()
{
    updateLabels();
}

void LabelViewDialog::onLabelRemoved()
{
    updateLabels();
}

void LabelViewDialog::updateLabels()
{
    labelModel.setStringList(editor->labelIndex()->labels());
}
