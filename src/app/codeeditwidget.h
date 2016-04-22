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

#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QSyntaxHighlighter;
QT_END_NAMESPACE

#include <documentlabelindex.h>

namespace Ui {
class CodeEditWidget;
}

class CodeEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditWidget(QWidget* parent = 0);
    ~CodeEditWidget();

    QString fileExtension() const;
    QString fileName() const;
    QString fileNameWithoutExtension() const;
    QString fullFileName() const;

    QPlainTextEdit* textEdit();
    DocumentLabelIndex* labelIndex();

    void setFileName(const QString& fullFileName);

    bool load();

public slots:
    bool save();
    bool saveAs();

protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private slots:
    void autoIndent();
    void onTokensAdded(const TokenList& tokens, int lineNumber);
    void onTokensRemoved(const TokenList& tokens, int lineNumber);

private:
    static const int FONT_SIZE = 14;    // in points
    static const int TAB_WIDTH = 8;     // in spaces
    static const int CURSOR_WIDTH = 2;  // in pixels

    Ui::CodeEditWidget* ui;

    QSyntaxHighlighter* highlighter;
    DocumentLabelIndex* labelIndexer;
    QString fileBeingEdited;

    void connectSignalsAndSlots();
    void setupTextEdit();
    void setupIntellisense();

    bool maybeSave();
    bool saveFile(const QString& fileName);
    bool loadFile(const QString& fileName);
};

#endif // CODEEDITWIDGET_H
