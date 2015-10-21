#ifndef ADDONSELECTDIALOG_H
#define ADDONSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class AddonSelectDialog;
}

class AddonSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddonSelectDialog(QWidget *parent = 0);
    ~AddonSelectDialog();

    QString selectedAddon;

private slots:
    void on_cancel_clicked();

    void on_open_clicked();

private:
    Ui::AddonSelectDialog *ui;
};

#endif // ADDONSELECTDIALOG_H
