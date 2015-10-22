#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scripteditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNew_Lua_Script_triggered();

    void on_actionSplit_Editor_triggered(bool checked);

    void on_editorMain_selectionChanged();

    void on_editorSplit_selectionChanged();

    void on_actionOpen_Addon_triggered();

    void on_actionSwitch_To_Main_Editor_triggered();

    void on_actionSwitch_To_Split_Editor_triggered();

private:
    Ui::MainWindow *ui;

    ScriptEditor *currentEditor;
    QString addonName;
};

#endif // MAINWINDOW_H
