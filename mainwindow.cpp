#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addonselectdialog.h"
#include "scripteditor.h"
#include "scriptdocument.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    DEBUG
//    setDisabled(true);
//    QTimer::singleShot(300, this, SLOT(on_actionOpen_Addon_triggered()));

    // setup initial state of the twin editors
    ui->editorSplit->hide();
    currentEditor = ui->editorMain;
    connect(ui->editorMain, SIGNAL(onFocus(ScriptEditor*)), this, SLOT(onEditorFocus(ScriptEditor*)));
    connect(ui->editorSplit, SIGNAL(onFocus(ScriptEditor*)), this, SLOT(onEditorFocus(ScriptEditor*)));

    on_actionNew_Lua_Script_triggered();
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
    ScriptDocument *document = new ScriptDocument();
    ui->editorMain->setDocument(document);
}

void MainWindow::on_actionSplit_Editor_triggered(bool checked)
{
    if (checked) {
        ui->editorSplit->setDocument(ui->editorMain->document());
        ui->editorSplit->show();
        ui->editorSplit->setFocus();
    } else {
        ui->editorSplit->hide();
        ui->editorMain->setFocus();
    }
}

void MainWindow::onEditorFocus(ScriptEditor* editor)
{
    currentEditor = editor;
}

void MainWindow::on_actionSwitch_To_Main_Editor_triggered()
{
    ui->editorMain->setFocus();
}

void MainWindow::on_actionSwitch_To_Split_Editor_triggered()
{
    if (ui->editorSplit->isHidden()) {
        ui->actionSplit_Editor->trigger();
    } else {
        ui->editorSplit->setFocus();
    }
}

void MainWindow::on_actionComment_Selection_triggered()
{
    currentEditor->commentSelection();
}
