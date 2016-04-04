#include "aseconfigdialog.h"
#include "ui_aseconfigdialog.h"

#include <QDir>
#include <QFileDialog>
#include <QLineEdit>

AseConfigDialog::AseConfigDialog(QWidget* parent, const QString& pathToAse100) :
    QDialog(parent),
    ui(new Ui::AseConfigDialog)
{
    ui->setupUi(this);

    setPath(pathToAse100);

    connect(ui->pathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_path_changed(const QString&)));
}

AseConfigDialog::~AseConfigDialog()
{
    delete ui;
}

QString AseConfigDialog::getPath() const
{
    return pathToAse100;
}

void AseConfigDialog::setPath(const QString& str)
{
    ui->pathLineEdit->setText(str);
}

void AseConfigDialog::on_pathButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Select path to ase100"),
                                                    QDir::homePath());
    if (!filePath.isEmpty())
        setPath(filePath);
}

void AseConfigDialog::on_path_changed(const QString& str)
{
    pathToAse100 = str;
}
