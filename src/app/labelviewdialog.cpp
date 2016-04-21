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
    const QStringList& labelStrings = editor->labelIndex()->labels();
    labelModel.setStringList(labelStrings);
}
