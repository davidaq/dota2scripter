#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "luaeditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    LuaEditor* editor = new LuaEditor(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
