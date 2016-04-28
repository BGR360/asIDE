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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QKeySequence>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSettings>
#include <QTextStream>

#include "aseconfigdialog.h"
#include "codeeditwidget.h"
#include "labelviewdialog.h"
#include "instructionviewdialog.h"
#include "tokenviewdialog.h"

static const QUrl BUG_REPORTING_URL("https://github.com/bgr360/asIDE/issues");
static const QUrl REDDIT_URL("https://reddit.com");
static const QUrl NIC_CAGE_URL("https://placecage.com");
static const QUrl CATS_URL("https://reddit.com/r/babybigcatgifs");

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentEditor(NULL)
{
    ui->setupUi(this);

    connectSignalsAndSlots();
    setupActions();

    readSettings();

    labelViewDialog = new LabelViewDialog(this);
    instructionViewDialog = new InstructionViewDialog(this);
    tokenViewDialog = new TokenViewDialog(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete labelViewDialog;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Attempt to close all tabs before we accept the close event
    while (ui->tabWidget->count() > 0)
    {
        if (!closeTab(0)) {
            event->ignore();
            return;
        }
    }

    writeSettings();
    event->accept();
}

void MainWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    if (currentFile.isEmpty())
        newFile();
}

void MainWindow::newFile()
{
    loadFile(QString());
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an assembly file"),
                                                    pathToMostRecentFile,
                                                    tr("E100 Assembly Files (*.e);;"
                                                       "E100 Labels Files (*.labels);;"
                                                       "MIF Files (*.mif)"));
    if (!fileName.isEmpty()) {
        pathToMostRecentFile = fileName;
        loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (currentEditor->save()) {
        updateCurrentFile();
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

bool MainWindow::saveAs()
{
    if (currentEditor->saveAs()) {
        updateCurrentFile();
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

bool MainWindow::closeTab(int index)
{
    switchToTab(index);
    QWidget* tab = ui->tabWidget->widget(index);
    if (tab->close()) {
        ui->tabWidget->removeTab(index);

        // If all tabs have been closed, set editor to null and reset the window title
        if (ui->tabWidget->count() == 0) {
            currentEditor = NULL;
            updateCurrentFile();
        }

        return true;
    }
    return false;
}

bool MainWindow::closeActiveTab()
{
    if (ui->tabWidget->count() > 0)
        return closeTab(ui->tabWidget->currentIndex());
    else
        return 0;
}

void MainWindow::switchToTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::onTabSwitched(int index)
{
    QWidget* tab = ui->tabWidget->widget(index);
    CodeEditWidget* codeEdit = qobject_cast<CodeEditWidget*>(tab);
    setEditor(codeEdit);
    updateCurrentFile();

    // Check to see if the "View Labels" and "View MIF" options are available for this file
    if (currentEditor) {
        QString fileExtension = currentEditor->fileExtension();
        if (fileExtension == "e") {
            ui->actionViewLabels->setEnabled(true);
            ui->actionViewMif->setEnabled(true);
        } else {
            ui->actionViewLabels->setEnabled(false);
            ui->actionViewMif->setEnabled(false);
        }
    }
}

void MainWindow::setEditor(CodeEditWidget* codeEdit)
{
    if (codeEdit == currentEditor)
        return;

    // Disconnect signals and slots from the previous editor
    if (currentEditor) {
        QPlainTextEdit* textEdit = currentEditor->textEdit();
        disconnect(ui->actionUndo, SIGNAL(triggered(bool)), textEdit, SLOT(undo()));
        disconnect(ui->actionRedo, SIGNAL(triggered(bool)), textEdit, SLOT(redo()));
        disconnect(ui->actionCut, SIGNAL(triggered(bool)), textEdit, SLOT(cut()));
        disconnect(ui->actionCopy, SIGNAL(triggered(bool)), textEdit, SLOT(copy()));
        disconnect(ui->actionPaste, SIGNAL(triggered(bool)), textEdit, SLOT(paste()));
        disconnect(textEdit, SIGNAL(textChanged()), this, SLOT(onModifyCurrentFile()));
    }

    currentEditor = codeEdit;
    labelViewDialog->setEditor(currentEditor);
    tokenViewDialog->setEditor(currentEditor);

    // Hook up the necessary signals and slots for the new editor
    if (currentEditor) {
        QPlainTextEdit* textEdit = currentEditor->textEdit();
        connect(ui->actionUndo, SIGNAL(triggered(bool)), textEdit, SLOT(undo()));
        connect(ui->actionRedo, SIGNAL(triggered(bool)), textEdit, SLOT(redo()));
        connect(ui->actionCut, SIGNAL(triggered(bool)), textEdit, SLOT(cut()));
        connect(ui->actionCopy, SIGNAL(triggered(bool)), textEdit, SLOT(copy()));
        connect(ui->actionPaste, SIGNAL(triggered(bool)), textEdit, SLOT(paste()));
        connect(textEdit, SIGNAL(textChanged()), this, SLOT(onModifyCurrentFile()));
        ui->actionViewLabels->setEnabled(true);
        ui->actionViewMif->setEnabled(true);
    } else {
        ui->actionViewLabels->setEnabled(false);
        ui->actionViewMif->setEnabled(false);
    }
}

bool MainWindow::assemble()
{
    // Assembly using ase100 is only supported on Windows and Linux
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    statusBar()->showMessage(tr("Assembling file..."));

    if (currentFile.contains("untitled")) {
        QMessageBox::warning(this, tr("ase100"),
                             tr("Unable to assemble untitled file.\n"
                                "Please save the file before assembling."),
                             QMessageBox::Ok);
        statusBar()->showMessage(tr("File assembly via ase100 failed."), 3000);
        return false;
    }

    QProcess process;
    if (!pathToAse100.isEmpty()) {
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start(pathToAse100, QStringList() << "-a" << currentFile);
        process.waitForFinished();

        QString output = process.readAll();
        if (!output.isEmpty()) {
            QMessageBox::critical(this, tr("ase100"), output, QMessageBox::Ok);
            statusBar()->showMessage(tr("File assembly via ase100 failed."), 3000);
        } else {
            QMessageBox::information(this, tr("ase100"),
                                     tr("File successfully assembled!"),
                                     QMessageBox::Ok);
            statusBar()->showMessage(tr("File successfully assembled."), 3000);
        }
    } else {
        const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("asIDE"),
                                   tr("The path to the ase100 "
                                      "executable has not been set.\n"
                                      "Do you want to set it now?"),
                                   QMessageBox::Cancel | QMessageBox::Yes);
        switch(ret)
        {
        case QMessageBox::Yes:
            ui->actionConfigureAse->trigger();
            break;
        default:
            break;
        }
    }
    return true;
#else
    return false;
#endif
}

bool MainWindow::launchAse()
{
    // ase100 is only supported on Windows and Linux
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if (!pathToAse100.isEmpty()) {
        QProcess::startDetached(pathToAse100);
    } else {
        const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("asIDE"),
                                   tr("The path to the ase100 "
                                      "executable has not been set.\n"
                                      "Do you want to set it now?"),
                                   QMessageBox::Cancel | QMessageBox::Yes);
        switch(ret)
        {
        case QMessageBox::Yes:
            ui->actionConfigureAse->trigger();
            break;
        default:
            break;
        }
    }
    return true;
#else
    return false;
#endif
}

void MainWindow::configureAse()
{
    AseConfigDialog* dialog = new AseConfigDialog(this, pathToAse100);
    dialog->setWindowTitle(tr("Configure ase100"));
    if (dialog->exec() == QDialog::Accepted) {
        pathToAse100 = dialog->getPath();
    }
}

bool MainWindow::viewLabels()
{
    statusBar()->showMessage(tr("Attempting to open Labels file..."));
    if (!currentEditor)
        return false;
    QString labelsPath = currentEditor->fileNameWithoutExtension().append(".labels");
    bool exists = QFileInfo(labelsPath).exists();
    if (!exists) {
        if (!assemble() || !QFileInfo(labelsPath).exists()) {
            statusBar()->showMessage(tr("Failed to open Labels file"), 3000);
            return false;
        }
    }
    loadFile(labelsPath);
    return true;
}

bool MainWindow::viewMif()
{
    statusBar()->showMessage(tr("Attempting to open MIF file..."));
    if (!currentEditor)
        return false;
    QString mifPath = currentEditor->fileNameWithoutExtension().append(".mif");
    bool exists = QFileInfo(mifPath).exists();
    if (!exists) {
        if (!assemble() || !QFileInfo(mifPath).exists()) {
            statusBar()->showMessage(tr("Failed to open MIF file"), 3000);
            return false;
        }
    }
    loadFile(mifPath);
    return true;
}

void MainWindow::viewLabelIndex()
{
    labelViewDialog->show();
}

void MainWindow::viewTokens()
{
    tokenViewDialog->show();
}

void MainWindow::viewInstructions()
{
    instructionViewDialog->show();
}

void MainWindow::about()
{
    QFile aboutHtml(":/html/about.html");
    QString aboutHtmlString = QString("%1 - %2")
            .arg(QApplication::applicationName())
            .arg(QApplication::applicationVersion());
    if (aboutHtml.open(QIODevice::ReadOnly | QIODevice::Text))
        aboutHtmlString.append(aboutHtml.readAll());
    else
        qDebug() << "Could not open about.html";
    QMessageBox::about(this, tr("About ase100"), aboutHtmlString);
}

void MainWindow::reportBug()
{
    QDesktopServices::openUrl(BUG_REPORTING_URL);
}

void MainWindow::complain()
{
    QMessageBox::warning(this, "No", "Don't do that. Nobody likes a whiner", QMessageBox::Ok);
}

void MainWindow::imBored()
{
    QDesktopServices::openUrl(REDDIT_URL);
}

void MainWindow::imLonely()
{
    int randWidth = qrand() % 800 + 100;
    int randHeight = qrand() % 800 + 150;
    QString dimensions = QString("/%1/%2").arg(randWidth).arg(randHeight);
    QString lonelyUrl = NIC_CAGE_URL.toString().append(dimensions);
    QDesktopServices::openUrl(QUrl(lonelyUrl));
}

void MainWindow::pickMeUp()
{
    QDesktopServices::openUrl(CATS_URL);
}

void MainWindow::onModifyCurrentFile()
{
    updateCurrentFile();
}

void MainWindow::connectSignalsAndSlots()
{
    // Pair each action with a corresponding behavior in the MainWindow
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->actionCloseFile, SIGNAL(triggered(bool)), this, SLOT(closeActiveTab()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));

    connect(ui->actionAssemble, SIGNAL(triggered(bool)), this, SLOT(assemble()));
    connect(ui->actionLaunchAse, SIGNAL(triggered(bool)), this, SLOT(launchAse()));
    connect(ui->actionConfigureAse, SIGNAL(triggered(bool)), this, SLOT(configureAse()));
    connect(ui->actionViewLabels, SIGNAL(triggered(bool)), this, SLOT(viewLabels()));
    connect(ui->actionViewMif, SIGNAL(triggered(bool)), this, SLOT(viewMif()));

    connect(ui->actionViewLabelIndex, SIGNAL(triggered(bool)), this, SLOT(viewLabelIndex()));
    connect(ui->actionViewTokens, SIGNAL(triggered(bool)), this, SLOT(viewTokens()));
    connect(ui->actionViewInstructions, SIGNAL(triggered(bool)), this, SLOT(viewInstructions()));

    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionReportBug, SIGNAL(triggered(bool)), this, SLOT(reportBug()));
    connect(ui->actionRequestFeature, SIGNAL(triggered(bool)), this, SLOT(reportBug()));
    connect(ui->actionComplain, SIGNAL(triggered(bool)), this, SLOT(complain()));
    connect(ui->actionBored, SIGNAL(triggered(bool)), this, SLOT(imBored()));
    connect(ui->actionLonely, SIGNAL(triggered(bool)), this, SLOT(imLonely()));
    connect(ui->actionPickMeUp, SIGNAL(triggered(bool)), this, SLOT(pickMeUp()));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabSwitched(int)));
}

void MainWindow::setupActions()
{
    // Set up keyboard shortcuts
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->actionCloseFile->setShortcut(QKeySequence::Close);
    ui->actionQuit->setShortcut(QKeySequence::Quit);

    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);

    ui->actionAssemble->setShortcut(QKeySequence::Refresh);

    // User should only be able to "assemble" on Linux or Windows
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    ui->actionAssemble->setEnabled(true);
    ui->actionConfigureAse->setEnabled(true);
    ui->actionLaunchAse->setEnabled(true);
#else
    ui->actionAssemble->setEnabled(false);
    ui->actionConfigureAse->setEnabled(false);
    ui->actionLaunchAse->setEnabled(false);
#endif
}

void MainWindow::readSettings()
{
    QSettings settings;
    const QByteArray geometry = settings.value("mainwindow/geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    QString defaultPathToAse100;

#if defined(Q_OS_LINUX)
        defaultPathToAse100 = "ase100";
#elif defined(Q_OS_WIN)
        defaultPathToAse100 = "ase100";
#else
        defaultPathToAse100 = QString();
#endif

    pathToAse100 = settings.value("pathToAse100", defaultPathToAse100).toString();
    pathToMostRecentFile = settings.value("pathToMostRecentFile", QDir::homePath()).toString();
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("pathToAse100", pathToAse100);
    settings.setValue("pathToMostRecentFile", pathToMostRecentFile);
}

void MainWindow::updateCurrentFile()
{
    currentFile = (currentEditor) ? currentEditor->fullFileName() : QString();
    setWindowModified(false);

    QString shownName = currentFile;
    setWindowFilePath(shownName);

    QString stripped = (currentEditor) ? currentEditor->fileName() : QString();
    if (!stripped.isEmpty()) {
        if (currentEditor->textEdit()->document()->isModified()) {
            stripped += "*";
            setWindowModified(true);
        }
        setWindowTitle(stripped + " - asIDE");
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), stripped);
    } else {
        setWindowTitle("asIDE");
    }
}

void MainWindow::loadFile(const QString& fileName)
{
    CodeEditWidget* codeEdit;

    // If all we have open is an untitled file, use the current tab
    // Otherwise, open a new one
    if (ui->tabWidget->count() == 1 && currentEditor->textEdit()->document()->isEmpty())
        codeEdit = currentEditor;
    else
        codeEdit = new CodeEditWidget();

    codeEdit->setFileName(fileName);

    if (codeEdit->load()) {
        if (!fileName.isEmpty())
            statusBar()->showMessage(tr("Loaded ") + fileName, 2000);

        setEditor(codeEdit);

        const int index = ui->tabWidget->addTab(currentEditor, currentEditor->fileName());
        switchToTab(index);
    } else {
        delete codeEdit;
        statusBar()->showMessage(tr("Failed to load ") + fileName, 3000);
    }
}
