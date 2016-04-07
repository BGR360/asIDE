#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QSyntaxHighlighter;
QT_END_NAMESPACE

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

private:
    static const int FONT_SIZE = 14;    // in points
    static const int TAB_WIDTH = 8;     // in spaces
    static const int CURSOR_WIDTH = 2;  // in pixels

    Ui::CodeEditWidget* ui;

    QSyntaxHighlighter* highlighter;
    QString fileBeingEdited;

    void connectSignalsAndSlots();
    void setupTextEdit();
    void setupSyntaxHighlighter();

    bool maybeSave();
    bool saveFile(const QString& fileName);
    bool loadFile(const QString& fileName);
};

#endif // CODEEDITWIDGET_H
