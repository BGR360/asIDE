#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDebug>
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

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    editor(0)
{
    ui->setupUi(this);

    connectSignalsAndSlots();
    setupActions();

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
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
                                                    tr("E100 Assembly Files (*.e)"));
    if (!fileName.isEmpty()) {
        pathToMostRecentFile = fileName;
        loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (editor->save()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

bool MainWindow::saveAs()
{
    if (editor->saveAs()) {
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
            editor = NULL;
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
}

void MainWindow::setEditor(CodeEditWidget* codeEdit)
{
    if (codeEdit) {

        // Disconnect signals and slots from the previous editor
        if (editor) {
            QPlainTextEdit* textEdit = editor->textEdit();
            disconnect(ui->actionCut, SIGNAL(triggered(bool)), textEdit, SLOT(cut()));
            disconnect(ui->actionCopy, SIGNAL(triggered(bool)), textEdit, SLOT(copy()));
            disconnect(ui->actionPaste, SIGNAL(triggered(bool)), textEdit, SLOT(paste()));
            disconnect(textEdit, SIGNAL(textChanged()), this, SLOT(onModifyCurrentFile()));
        }

        editor = codeEdit;
        updateCurrentFile();

        // Hook up the necessary signals and slots for the code editor
        QPlainTextEdit* textEdit = editor->textEdit();
        connect(ui->actionCut, SIGNAL(triggered(bool)), textEdit, SLOT(cut()));
        connect(ui->actionCopy, SIGNAL(triggered(bool)), textEdit, SLOT(copy()));
        connect(ui->actionPaste, SIGNAL(triggered(bool)), textEdit, SLOT(paste()));
        connect(textEdit, SIGNAL(textChanged()), this, SLOT(onModifyCurrentFile()));
    }
}

bool MainWindow::assemble()
{
    // Assembly using ase100 is only supported on Windows and Linux
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
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
    }
    return true;
#else
    return false;
#endif
}

void MainWindow::configureAse()
{
    AseConfigDialog* dialog = new AseConfigDialog(this, pathToAse100);
    dialog->setWindowTitle("Configure ase100");
    if (dialog->exec() == QDialog::Accepted) {
        pathToAse100 = dialog->getPath();
    }
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
    connect(ui->actionConfigureAse, SIGNAL(triggered(bool)), this, SLOT(configureAse()));

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
#else
    ui->actionAssemble->setEnabled(false);
#endif
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
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
    settings.setValue("geometry", saveGeometry());
    settings.setValue("pathToAse100", pathToAse100);
    settings.setValue("pathToMostRecentFile", pathToMostRecentFile);
}

void MainWindow::updateCurrentFile()
{
    currentFile = (editor) ? editor->fullFileName() : QString();
    setWindowModified(false);

    QString shownName = currentFile;
    setWindowFilePath(shownName);

    QString stripped = (editor) ? editor->fileName() : QString();
    if (!stripped.isEmpty()) {
        if (editor->textEdit()->document()->isModified())
            stripped += "*";
        setWindowTitle(stripped + " - asIDE");
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), stripped);
    } else {
        setWindowTitle("asIDE");
    }
}

void MainWindow::loadFile(const QString& fileName)
{
    CodeEditWidget* codeEdit;

    if (ui->tabWidget->count() == 1 && editor->textEdit()->document()->isEmpty())
        codeEdit = editor;
    else
        codeEdit = new CodeEditWidget();

    codeEdit->setFileName(fileName);

    if (codeEdit->load()) {
        if (!fileName.isEmpty())
            statusBar()->showMessage(tr("File loaded"), 2000);

        setEditor(codeEdit);

        const int index = ui->tabWidget->addTab(editor, editor->fileName());
        switchToTab(index);
    } else {
        delete codeEdit;
        statusBar()->showMessage(tr("File failed to load"), 3000);
    }
}
