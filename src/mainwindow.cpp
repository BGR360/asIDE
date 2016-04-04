#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSettings>
#include <QTextStream>

#include "syntaxhighlighter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    editor(0),
    highlighter(0)
{
    ui->setupUi(this);

    editor = ui->editor;

    connectSignalsAndSlots();
    setupActions();
    setupFont();
    setupSyntaxHighlighter();

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    if (currentFile.isEmpty())
        newFile();
}

void MainWindow::newFile()
{
    if (maybeSave())
    {
        editor->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (currentFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(currentFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
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

void MainWindow::connectSignalsAndSlots()
{
    // Pair each action with a corresponding behavior in the MainWindow
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->actionAssemble, SIGNAL(triggered(bool)), this, SLOT(assemble()));
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

void MainWindow::setupFont()
{
    // Set the font to a monospace font
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(14);

    ui->editor->setFont(font);

    // Set the tab width to 8 spaces
    const int tabStop = 8;
    QFontMetrics metrics(font);
    editor->setTabStopWidth(tabStop * metrics.width(' '));
}

void MainWindow::setupSyntaxHighlighter()
{
    QTextDocument* doc = editor->document();
    highlighter = new SyntaxHighlighter(doc);
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
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("pathToAse100", pathToAse100);
}

bool MainWindow::maybeSave()
{
    if (!editor->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("asIDE"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }

    return true;
}

bool MainWindow::saveFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("asIDE"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    out << editor->toPlainText();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    currentFile = fileName;
    editor->document()->setModified(false);
    setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);

    QString stripped = strippedName(shownName);
    setWindowTitle(stripped.append(" - asIDE"));
}

QString MainWindow::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("asIDE"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    editor->setPlainText(in.readAll());

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
