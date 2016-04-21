#ifndef LABELVIEWDIALOG_H
#define LABELVIEWDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class LabelViewDialog;
}

class CodeEditWidget;

class LabelViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabelViewDialog(QWidget* parent = 0, CodeEditWidget* editor = 0);
    ~LabelViewDialog();

    CodeEditWidget* codeEditor();
    void setEditor(CodeEditWidget* newEditor);

private slots:
    void onLabelAdded();
    void onLabelRemoved();
    void updateLabels();

private:
    Ui::LabelViewDialog* ui;

    CodeEditWidget* editor;
    QStringListModel labelModel;
};

#endif // LABELVIEWDIALOG_H
