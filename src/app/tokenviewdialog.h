#ifndef TOKENVIEWDIALOG_H
#define TOKENVIEWDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class TokenViewDialog;
}

class CodeEditWidget;

class TokenViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TokenViewDialog(QWidget* parent = 0, CodeEditWidget* editor = 0);
    ~TokenViewDialog();

    CodeEditWidget* codeEditor();
    void setEditor(CodeEditWidget* newEditor);

private slots:
    void onTokensAdded();
    void onTokensRemoved();
    void updateTokens();

private:
    Ui::TokenViewDialog *ui;

    CodeEditWidget* editor;
    QStringListModel tokenModel;
};

#endif // TOKENVIEWDIALOG_H
