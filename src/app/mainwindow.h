#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CodeEditWidget;
class LabelViewDialog;
class InstructionViewDialog;
class TokenViewDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadFile(const QString& fileName);

protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

    bool closeTab(int index);
    bool closeActiveTab();
    void switchToTab(int index);
    void onTabSwitched(int index);
    void setEditor(CodeEditWidget* codeEdit);

    bool assemble();
    void configureAse();
    bool viewLabels();
    bool viewMif();
    void viewLabelIndex();
    void viewTokens();
    void viewInstructions();
    void reportBug();

    void onModifyCurrentFile();

private:
    Ui::MainWindow* ui;

    CodeEditWidget* currentEditor;
    LabelViewDialog* labelViewDialog;
    InstructionViewDialog* instructionViewDialog;
    TokenViewDialog* tokenViewDialog;

    QString currentFile;
    QString pathToAse100;
    QString pathToMostRecentFile;

    void connectSignalsAndSlots();
    void setupActions();

    void readSettings();
    void writeSettings();
    void updateCurrentFile();
};

#endif // MAINWINDOW_H
