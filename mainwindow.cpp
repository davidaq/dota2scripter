#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addonselectdialog.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setDisabled(true);
    QTimer::singleShot(300, this, SLOT(on_actionOpen_Addon_triggered()));

    // setup initial state of the twin editors
    ui->textEditSplit->hide();
    currentEditor = ui->textEditMain;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Addon_triggered()
{
    setDisabled(true);
    AddonSelectDialog selectDialog;
    if (QDialog::Accepted == selectDialog.exec()) {

    }
    if (addonName.isEmpty()) {
        close();
    }
    setDisabled(false);
}

void MainWindow::on_actionNew_Lua_Script_triggered()
{
}

void MainWindow::on_actionSplit_Window_triggered(bool checked)
{
    if (checked) {
        ui->textEditSplit->show();
    } else {
        ui->textEditSplit->hide();
    }
}

void MainWindow::on_textEditMain_selectionChanged()
{
    currentEditor = ui->textEditMain;
}

void MainWindow::on_textEditSplit_selectionChanged()
{
    currentEditor = ui->textEditSplit;
}
