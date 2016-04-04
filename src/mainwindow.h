#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
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

    void loadFile(const QString& fileName);

protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool assemble();
    void configureAse();

private:
    Ui::MainWindow* ui;

    QPlainTextEdit* editor;
    QSyntaxHighlighter* highlighter;

    QString currentFile;
    QString pathToAse100;
    QString pathToMostRecentFile;

    void connectSignalsAndSlots();
    void setupActions();
    void setupFont();
    void setupSyntaxHighlighter();

    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
};

#endif // MAINWINDOW_H
