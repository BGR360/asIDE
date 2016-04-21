#ifndef INSTRUCTIONVIEWDIALOG_H
#define INSTRUCTIONVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class InstructionViewDialog;
}

class InstructionViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionViewDialog(QWidget *parent = 0);
    ~InstructionViewDialog();

private:
    Ui::InstructionViewDialog *ui;
};

#endif // INSTRUCTIONVIEWDIALOG_H
