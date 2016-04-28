#include "linenumberarea.h"

#include <QtGlobal>
#include <QPlainTextEdit>

#include "codeeditwidget.h"

const QColor LineNumberArea::SIDEBAR_COLOR = QColor::fromRgb(235, 235, 235);
const QColor LineNumberArea::LINE_NUMBER_COLOR = QColor::fromRgb(120, 120, 120);

LineNumberArea::LineNumberArea(CodeEditWidget* codeEdit) :
    QWidget(codeEdit->textEdit()),
    codeEdit(codeEdit)
{
    Q_ASSERT(codeEdit);
}

int LineNumberArea::lineNumberAreaWidth() const
{
    int digits = 1;
    const int numLines = codeEdit->textEdit()->blockCount();
    int max = qMax(1, numLines);
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = EXTRA_SPACE_LEFT + EXTRA_SPACE_RIGHT + codeEdit->fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    codeEdit->lineNumberAreaPaintEvent(event);
}
