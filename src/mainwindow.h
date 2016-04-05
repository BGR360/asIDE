#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CodeEditWidget;

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
    void onModifyCurrentFile();

private:
    Ui::MainWindow* ui;

    CodeEditWidget* editor;

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
