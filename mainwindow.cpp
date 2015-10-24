#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addonselectdialog.h"
#include "scripteditor.h"
#include "documentmanager.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    DEBUG
//    setDisabled(true);
//    QTimer::singleShot(300, this, SLOT(on_actionOpen_Addon_triggered()));
    ui->documentList->setSortingEnabled(true);

    // setup initial state of the twin editors
    ui->editorSplit->hide();
    currentEditor = ui->editorMain;
    connect(ui->editorMain, SIGNAL(onFocus(ScriptEditor*)), this, SLOT(onEditorFocus(ScriptEditor*)));
    connect(ui->editorSplit, SIGNAL(onFocus(ScriptEditor*)), this, SLOT(onEditorFocus(ScriptEditor*)));
    connect(DocumentManager::manager(), SIGNAL(listUpdated()), this, SLOT(onDocumentListUpdated()));

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


void MainWindow::displayDocument(ScriptDocument* doc)
{
    currentEditor->setDocument(doc);
    onEditorFocus(currentEditor);
}

void MainWindow::on_actionNew_Lua_Script_triggered()
{
    displayDocument(DocumentManager::manager()->open("", ScriptDocument::lua));
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
    for (int i = 0; i < ui->documentList->count(); i++) {
        QListWidgetItem* item = ui->documentList->item(i);
        if (item->data(Qt::UserRole).toULongLong() == (qulonglong)editor->document()) {
            if (!item->isSelected()) {
                ui->documentList->setCurrentItem(item);
            }
            break;
        }
    }
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

void MainWindow::onDocumentListUpdated()
{
    ui->documentList->clear();
    QListWidgetItem* selected = 0;
    for (int i = 0; i < DocumentManager::manager()->count(); i++) {
        ScriptDocument* doc = DocumentManager::manager()->at(i);
        QListWidgetItem* docListItem = new QListWidgetItem(doc->icon(), doc->title());
        docListItem->setData(Qt::UserRole, QVariant::fromValue((qulonglong)doc));
        ui->documentList->addItem(docListItem);
        if (doc == currentEditor->document()) {
            selected = docListItem;
        }
    }
    if (selected) {
        ui->documentList->setCurrentItem(selected);
    }
}

void MainWindow::on_documentList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current || current == previous)
        return;
    ScriptDocument* doc = dynamic_cast<ScriptDocument*>((QObject*)current->data(Qt::UserRole).toULongLong());
    if (doc) {
        displayDocument(doc);
    }
}
