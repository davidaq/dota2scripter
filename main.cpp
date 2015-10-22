#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

QDir dotaDir;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool dotaDirFound = false;

    QSettings steamReg(QSettings::UserScope, "Valve", "Steam");
    QVariant steamPathVar = steamReg.value("SteamPath");
    if (steamPathVar.isValid() && !steamPathVar.isNull()) {
        dotaDirFound = true;
        QDir steamDir(steamPathVar.toString());
        dotaDirFound = dotaDirFound && steamDir.cd("steamapps");
        dotaDirFound = dotaDirFound && steamDir.cd("common");
        if (dotaDirFound) {
            dotaDirFound = dotaDirFound && (steamDir.cd("dota 2 beta") || steamDir.cd("dota 2"));
        }
        if (dotaDirFound) {
            dotaDir = steamDir;
        }
    }

    dotaDirFound = true; // DEBUG

    if (!dotaDirFound) {
        QMessageBox msg;
        msg.setWindowTitle(QObject::tr("Unable to continue"));
        msg.setText(QObject::tr("Can not detect Dota 2 directory!")
                    + "\n" + QObject::tr("You may try to locate the directory manuelly."));
        msg.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msg.setButtonText(QMessageBox::No,QObject:: tr("Abort"));
        msg.setButtonText(QMessageBox::Yes, QObject::tr("Setup Manuelly"));
        if(QMessageBox::No == msg.exec()) {
            return 0;
        }
        QString dirName = "C:/Program Files";
        if (QDir("C:/Program Files (x86)").exists()) {
            dirName = "C:/Program Files (x86)";
        }
        dirName = QFileDialog::getExistingDirectory(0, QObject::tr("Locate Dota 2 directory"), dirName);
        if (dirName.isEmpty()) {
            return 0;
        }
        dotaDir = dirName;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
