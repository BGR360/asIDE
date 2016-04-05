#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSettings>
#include <QTextStream>

#include "aseconfigdialog.h"
#include "codeeditwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

void MainWindow::switchToTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::onTabSwitched(int index)
{
    QWidget* tab = ui->tabWidget->widget(index);
    CodeEditWidget* codeEdit = qobject_cast<CodeEditWidget*>(tab);
    if (codeEdit) {
        editor = codeEdit;
        updateCurrentFile();
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

void MainWindow::connectSignalsAndSlots()
{
    // Pair each action with a corresponding behavior in the MainWindow
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->actionAssemble, SIGNAL(triggered(bool)), this, SLOT(assemble()));
    connect(ui->actionConfigureAse, SIGNAL(triggered(bool)), this, SLOT(configureAse()));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabSwitched(int)));
}

void MainWindow::setupActions()
{
    // Attempt to give each action an icon from the system theme
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy", ui->actionCopy->icon()));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut", ui->actionCut->icon()));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste", ui->actionPaste->icon()));

    ui->actionNew->setIcon(QIcon::fromTheme("document-new", ui->actionNew->icon()));
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open", ui->actionOpen->icon()));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save", ui->actionSave->icon()));
    ui->actionSaveAs->setIcon(QIcon::fromTheme("document-save-as", ui->actionSaveAs->icon()));

    ui->actionAssemble->setIcon(QIcon::fromTheme("media-playback-start", ui->actionAssemble->icon()));
    ui->actionInfo->setIcon(QIcon::fromTheme("help-about", ui->actionInfo->icon()));

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

    QString stripped = (editor) ? editor->fileName().append(" - ") : QString();
    setWindowTitle(stripped.append("asIDE"));
}

void MainWindow::loadFile(const QString& fileName)
{
    CodeEditWidget* oldEditor = editor;
    editor = new CodeEditWidget();
    editor->setFileName(fileName);

    if (editor->load()) {
        const int index = ui->tabWidget->addTab(editor, editor->fileName());
        switchToTab(index);

        if (!fileName.isEmpty())
            statusBar()->showMessage(tr("File loaded"), 2000);

        updateCurrentFile();
    } else {
        delete editor;
        editor = oldEditor;
        statusBar()->showMessage(tr("File failed to load"), 3000);
    }
}
