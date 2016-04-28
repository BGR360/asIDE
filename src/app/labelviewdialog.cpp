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

#include <algorithm> // std::sort
#include <QSettings>

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

    QSettings settings;
    const QByteArray geometry = settings.value("labelview/geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    const bool isShowing = settings.value("labelview/isShowing", false).toBool();
    if (isShowing)
        show();
}

LabelViewDialog::~LabelViewDialog()
{
    QSettings settings;
    settings.setValue("labelview/geometry", saveGeometry());
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
            disconnect(indexer, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelIndexChange()));
            disconnect(indexer, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelIndexChange()));
            disconnect(indexer, SIGNAL(lineAdded(int)), this, SLOT(onLabelIndexChange()));
            disconnect(indexer, SIGNAL(lineRemoved(int)), this, SLOT(onLabelIndexChange()));
        }
    }

    editor = newEditor;

    if (editor) {
        QString fileName(editor->fileName());
        setWindowTitle(QString("Labels View for %1").arg(fileName));

        updateLabels();

        DocumentLabelIndex* indexer = editor->labelIndex();
        if (indexer) {
            connect(indexer, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelIndexChange()));
            connect(indexer, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelIndexChange()));
            connect(indexer, SIGNAL(lineAdded(int)), this, SLOT(onLabelIndexChange()));
            connect(indexer, SIGNAL(lineRemoved(int)), this, SLOT(onLabelIndexChange()));
        }
    } else {
        labelModel.setStringList(QStringList());
    }
}

void LabelViewDialog::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    QSettings settings;
    settings.setValue("labelview/isShowing", true);
    updateLabels();
}

void LabelViewDialog::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);
    QSettings settings;
    settings.setValue("labelview/isShowing", false);
}

void LabelViewDialog::onLabelIndexChange()
{
    if (isVisible())
        updateLabels();
}

void LabelViewDialog::updateLabels()
{
    if (editor) {
        DocumentLabelIndex* indexer = editor->labelIndex();
        if (indexer) {
            QStringList labels = indexer->labels();
            QStringList::iterator i;
            for (i = labels.begin(); i != labels.end(); ++i) {
                QString& label = *i;
                const int lineNumberOfLabel = indexer->lineNumberOfLabel(label);
                label.insert(0, QString("%1: ").arg(lineNumberOfLabel));
            }
            std::sort(labels.begin(), labels.end());
            labelModel.setStringList(labels);
        }
    }
}
