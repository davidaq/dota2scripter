#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

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

    void on_actionSplit_Window_triggered(bool checked);

    void on_textEditMain_selectionChanged();

    void on_textEditSplit_selectionChanged();

    void on_actionOpen_Addon_triggered();

private:
    Ui::MainWindow *ui;

    QTextEdit* currentEditor;
    QString addonName;
};

#endif // MAINWINDOW_H
