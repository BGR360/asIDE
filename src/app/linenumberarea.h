#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QColor>
#include <QWidget>

class CodeEditWidget;

class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(CodeEditWidget* codeEdit);

    int lineNumberAreaWidth() const;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    static const int EXTRA_SPACE_LEFT = 15;
    static const int EXTRA_SPACE_RIGHT = 15;
    static const QColor SIDEBAR_COLOR;
    static const QColor LINE_NUMBER_COLOR;

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    CodeEditWidget* codeEdit;
};

#endif // LINENUMBERAREA_H
