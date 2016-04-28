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

#include "codeeditwidget.h"
#include "ui_codeeditwidget.h"

#include <QFileDialog>
#include <QFont>
#include <QFontMetrics>
#include <QMessageBox>
#include <QPainter>
#include <QStringListModel>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>

#include "linenumberarea.h"
#include <syntaxhighlighter.h>

CodeEditWidget::CodeEditWidget(QWidget* parent) :
    QPlainTextEdit(parent),
    highlighter(NULL),
    labelIndexer(NULL),
    firstSelectedLine(0),
    lastSelectedLine(0)
{
    lineNumberArea = new LineNumberArea(this);

    setupTextEdit();
    setupIntellisense();
    connectSignalsAndSlots();

    updateLineNumberAreaWidth();
}

CodeEditWidget::~CodeEditWidget()
{
    delete lineNumberArea;
    lineNumberArea = NULL;
    if (labelIndexer)
        delete labelIndexer;
    if (highlighter)
        delete highlighter;
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
    return this;
}

DocumentLabelIndex* CodeEditWidget::labelIndex()
{
    return labelIndexer;
}

void CodeEditWidget::setFileName(const QString& fullFileName)
{
    fileBeingEdited = fullFileName;
}

bool CodeEditWidget::load()
{
    return loadFile(fileBeingEdited);
}

void CodeEditWidget::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    // This is where we do all the magic of drawing the line numbers
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), LineNumberArea::SIDEBAR_COLOR);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber >= firstSelectedLine && blockNumber <= lastSelectedLine)
                painter.setPen(LineNumberArea::LINE_NUMBER_HIGHLIGHTED_COLOR);
            else
                painter.setPen(LineNumberArea::LINE_NUMBER_COLOR);
            painter.drawText(0, top, lineNumberArea->width() - LineNumberArea::EXTRA_SPACE_RIGHT,
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
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

void CodeEditWidget::highlightLines(int startLine, int endLine)
{
    QTextCursor selectionStart(document()->findBlockByNumber(startLine));
    QTextCursor selectionEnd(document()->findBlockByNumber(endLine));
    selectionEnd.setPosition(selectionEnd.block().position() + selectionEnd.block().length());

    QTextCursor selection = textCursor();
    selection.setPosition(selectionStart.position());
    selection.setPosition(selectionEnd.position(), QTextCursor::KeepAnchor);
    setTextCursor(selection);
}

void CodeEditWidget::closeEvent(QCloseEvent* event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void CodeEditWidget::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberArea->lineNumberAreaWidth(), cr.height()));
}

void CodeEditWidget::autoIndent()
{
    QTextCursor cursor = textCursor();
    QTextDocument* doc = document();

    // Auto-indent if we just started a new line
    if (cursor.atBlockStart()) {
        int previousLineNumber = cursor.block().blockNumber() - 1;
        QTextBlock previousBlock = doc->findBlockByNumber(previousLineNumber);
        QString previousLine = previousBlock.text();

        // Look at the previous line and copy all text that comes before
        // the first non-whitespace character (or, if whole line is whitespace,
        // take everything up until the newline)
        QRegExp whitespaceRegex("[^\\s]|\\n");
        int indexOfFirstNonWhitespace = whitespaceRegex.indexIn(previousLine);
        QString previousLineWhitespace = previousLine.left(indexOfFirstNonWhitespace);

        // Now insert this whitespace at the beginning of the current line
        cursor.insertText(previousLineWhitespace);
    }
}

void CodeEditWidget::onTokensAdded(const TokenList& tokens, int lineNumber)
{
    Q_UNUSED(lineNumber);
    qDebug() << "Tokens added:";
    foreach (const Token& token, tokens) {
        qDebug() << "   " << token;
    }
}

void CodeEditWidget::onTokensRemoved(const TokenList& tokens, int lineNumber)
{
    Q_UNUSED(lineNumber);
    qDebug() << "Tokens removed:";
    foreach (const Token& token, tokens) {
        qDebug() << "   " << token;
    }
}

void CodeEditWidget::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberArea->lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditWidget::updateLineNumberArea(const QRect& rect, int dy)
{
    if (lineNumberArea) {
        if (dy)
            lineNumberArea->scroll(0, dy);
        else
            lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

        if (rect.contains(viewport()->rect()))
            updateLineNumberAreaWidth();
    }
}

void CodeEditWidget::highlightCurrentLine()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        QTextCursor selectionBegin = cursor;
        QTextCursor selectionEnd = cursor;
        selectionBegin.setPosition(cursor.selectionStart());
        selectionEnd.setPosition(cursor.selectionEnd() - 1);
        firstSelectedLine = selectionBegin.block().blockNumber();
        lastSelectedLine = selectionEnd.block().blockNumber();
    } else {
        firstSelectedLine = lastSelectedLine = cursor.block().blockNumber();
    }
    update();
}

void CodeEditWidget::connectSignalsAndSlots()
{
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(autoIndent()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(highlightCurrentLine()));
}

void CodeEditWidget::setupTextEdit()
{
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

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
    setFont(font);

    // Set the tab width
    QFontMetrics metrics(font);
    setTabStopWidth(TAB_WIDTH * metrics.width(' '));

    // Set the cursor width
    setCursorWidth(CURSOR_WIDTH);
}

void CodeEditWidget::setupIntellisense()
{
    QTextDocument* doc = document();

    if (labelIndexer && labelIndexer->tokenizer()) {
        DocumentTokenizer* tokenizer = labelIndexer->tokenizer();
        disconnect(tokenizer, SIGNAL(tokensAdded(TokenList,int)), this,
                SLOT(onTokensAdded(TokenList,int)));
        disconnect(tokenizer, SIGNAL(tokensRemoved(TokenList,int)), this,
                SLOT(onTokensRemoved(TokenList,int)));
        delete labelIndexer;
    }
    if (highlighter)
        delete highlighter;

    labelIndexer = new DocumentLabelIndex(doc);
    highlighter = new SyntaxHighlighter(doc, labelIndexer);

    if (labelIndexer) {
        DocumentTokenizer* tokenizer = labelIndexer->tokenizer();
        connect(tokenizer, SIGNAL(tokensAdded(TokenList,int)), this,
                SLOT(onTokensAdded(TokenList,int)));
        connect(tokenizer, SIGNAL(tokensRemoved(TokenList,int)), this,
                SLOT(onTokensRemoved(TokenList,int)));
    }
}

bool CodeEditWidget::maybeSave()
{
    if (!document()->isModified())
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
    out << toPlainText();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    fileBeingEdited = fileName;
    document()->setModified(false);

    return true;
}

bool CodeEditWidget::loadFile(const QString& fileName)
{
    clear();

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
        setPlainText(in.readAll());
        highlighter->rehighlight();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }

    fileBeingEdited = fileName;

    return true;
}
