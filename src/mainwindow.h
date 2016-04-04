#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QSyntaxHighlighter;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    QSyntaxHighlighter* highlighter;

    void connectSignalsAndSlots();
    void setupActions();
    void setupFont();
    void setupSyntaxHighlighter();
};

#endif // MAINWINDOW_H
