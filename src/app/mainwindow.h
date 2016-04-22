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
    bool launchAse();
    void configureAse();
    bool viewLabels();
    bool viewMif();
    void viewLabelIndex();
    void viewTokens();
    void viewInstructions();

    void about();
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
