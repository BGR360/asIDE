#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
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

private:
    static const int FONT_SIZE = 14;    // in points
    static const int TAB_WIDTH = 8;     // in spaces
    static const int CURSOR_WIDTH = 2;  // in pixels

    Ui::CodeEditWidget *ui;

    QSyntaxHighlighter* highlighter;

    void setupTextEdit();
    void setupSyntaxHighlighter();
};

#endif // CODEEDITWIDGET_H
