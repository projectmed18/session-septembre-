#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::instance().connect("Source_Projet2A", "rayen", "MotDePasse123")) {
        QMessageBox::critical(nullptr, "Erreur de connexion",
                              "Impossible de se connecter à la base de données.");
        return 1;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Database is open");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Connection successful.\nClick OK to continue.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    MainWindow w;
    w.show();
    return a.exec();
}
