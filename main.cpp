#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion à la même base de données que gestion stagiaire
    if (!DatabaseManager::instance().connect("Source_Projet2A", "rayen", "MotDePasse123")) {
        QMessageBox::critical(nullptr, "Erreur de connexion",
                              "Impossible de se connecter à la base de données.\n\n"
                              "Détails Oracle/ODBC :\n" + DatabaseManager::instance().getLastError());
        return 1;
    }

    // Message de succès de connexion
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
