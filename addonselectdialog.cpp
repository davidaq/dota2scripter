#include "addonselectdialog.h"
#include "ui_addonselectdialog.h"
#include "global.h"

AddonSelectDialog::AddonSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddonSelectDialog)
{
    ui->setupUi(this);
}

AddonSelectDialog::~AddonSelectDialog()
{
    delete ui;
}

void AddonSelectDialog::on_cancel_clicked()
{
    reject();
}

void AddonSelectDialog::on_open_clicked()
{
    if (ui->list->currentItem()) {
        selectedAddon = ui->list->currentItem()->text();
        accept();
    }
}
