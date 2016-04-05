#include "codeeditwidget.h"
#include "ui_codeeditwidget.h"

#include <QFileDialog>
#include <QFont>
#include <QFontMetrics>
#include <QMessageBox>
#include <QTextStream>

#include "syntaxhighlighter.h"

CodeEditWidget::CodeEditWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CodeEditWidget)
{
    ui->setupUi(this);

    setupTextEdit();
    setupSyntaxHighlighter();
}

CodeEditWidget::~CodeEditWidget()
{
    delete ui;
}

QString CodeEditWidget::fileExtension() const
{
    return QFileInfo(fullFileName()).completeSuffix();
}

QString CodeEditWidget::fileName() const
{
    return QFileInfo(fullFileName()).fileName();
}

QString CodeEditWidget::fileNameWithoutExtension() const
{
    QString fullPath = fullFileName();
    QString extension = fileExtension();
    const int extensionLength = extension.length();
    const int cutoffLength = fullPath.length() - extensionLength - 1;
    return fullPath.mid(0, cutoffLength);
}

QString CodeEditWidget::fullFileName() const
{
    if (fileBeingEdited.isEmpty())
        return "untitled.e";
    else
        return fileBeingEdited;
}

QPlainTextEdit* CodeEditWidget::textEdit()
{
    return ui->textEdit;
}

void CodeEditWidget::setFileName(const QString& fullFileName)
{
    fileBeingEdited = fullFileName;
}

bool CodeEditWidget::load()
{
    return loadFile(fileBeingEdited);
}

bool CodeEditWidget::save()
{
    if (fileBeingEdited.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(fileBeingEdited);
    }
}

bool CodeEditWidget::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(fileBeingEdited.isEmpty() ? QDir::homePath() : fileBeingEdited);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void CodeEditWidget::closeEvent(QCloseEvent* event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void CodeEditWidget::setupTextEdit()
{
    // Set the font to a monospace font
    QFont font;

#if defined(Q_OS_LINUX)
    font.setFamily("Andale Mono");
#elif defined(Q_OS_MAC)
    font.setFamily("Courier");
#else
    font.setFamily("Courier");
#endif

    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(FONT_SIZE);

    textEdit()->setFont(font);

    // Set the tab width
    QFontMetrics metrics(font);
    textEdit()->setTabStopWidth(TAB_WIDTH * metrics.width(' '));

    // Set the cursor width
    textEdit()->setCursorWidth(CURSOR_WIDTH);
}

void CodeEditWidget::setupSyntaxHighlighter()
{
    QTextDocument* doc = textEdit()->document();
    highlighter = new SyntaxHighlighter(doc);
}

bool CodeEditWidget::maybeSave()
{
    if (!textEdit()->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("asIDE"),
                               fileName().append(
                                  tr(" has been modified.\n"
                                     "Do you want to save your changes?")),
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

bool CodeEditWidget::saveFile(const QString& fileName)
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

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QTextStream out(&file);
    out << textEdit()->toPlainText();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return true;
}

bool CodeEditWidget::loadFile(const QString& fileName)
{
    textEdit()->clear();

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("asIDE"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
            return false;
        }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        QTextStream in(&file);
        textEdit()->setPlainText(in.readAll());

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }

    fileBeingEdited = fileName;

    return true;
}
