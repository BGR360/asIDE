#ifndef ASECONFIGDIALOG_H
#define ASECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class AseConfigDialog;
}

class AseConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AseConfigDialog(QWidget* parent, const QString& pathToAse100);
    ~AseConfigDialog();

    QString getPath() const;

public slots:
    void setPath(const QString& str);

signals:
    void pathChanged(const QString& str);

private slots:
    void on_pathButton_clicked();
    void on_path_changed(const QString& str);

private:
    Ui::AseConfigDialog* ui;

    QString pathToAse100;
};

#endif // ASECONFIGDIALOG_H
