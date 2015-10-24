#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ScriptEditor;
class ScriptDocument;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void displayDocument(ScriptDocument* doc);

    void on_actionNew_Lua_Script_triggered();

    void on_actionSplit_Editor_triggered(bool checked);

    void onEditorFocus(ScriptEditor* editor);

    void on_actionOpen_Addon_triggered();

    void on_actionSwitch_To_Main_Editor_triggered();

    void on_actionSwitch_To_Split_Editor_triggered();

    void on_actionComment_Selection_triggered();

    void onDocumentListUpdated();

    void on_documentList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::MainWindow *ui;

    ScriptEditor *currentEditor;
    QString addonName;
};

#endif // MAINWINDOW_H
