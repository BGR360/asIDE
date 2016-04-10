#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QStringListModel;
class QSyntaxHighlighter;
QT_END_NAMESPACE

#include <documenttokenizer.h>

namespace Ui {
class CodeEditWidget;
}

class CodeEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditWidget(QWidget *parent = 0);
    ~CodeEditWidget();

    QString fileExtension() const;
    QString fileName() const;
    QString fileNameWithoutExtension() const;
    QString fullFileName() const;
    QPlainTextEdit* textEdit();

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
    DocumentTokenizer* tokenizer;
    QString fileBeingEdited;

    QStringListModel* tokenModel;

    void connectSignalsAndSlots();
    void setupTextEdit();
    void setupIntellisense();

    bool maybeSave();
    bool saveFile(const QString& fileName);
    bool loadFile(const QString& fileName);
    void updateTokenView();
};

#endif // CODEEDITWIDGET_H
