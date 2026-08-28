#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Thème Catppuccin Mocha ---
    QPalette palette;
    palette.setColor(QPalette::Window,          QColor("#1e1e2e"));
    palette.setColor(QPalette::WindowText,      QColor("#cdd6f4"));
    palette.setColor(QPalette::Base,            QColor("#181825"));
    palette.setColor(QPalette::AlternateBase,   QColor("#252535"));
    palette.setColor(QPalette::Text,            QColor("#cdd6f4"));
    palette.setColor(QPalette::Button,          QColor("#313244"));
    palette.setColor(QPalette::ButtonText,      QColor("#cdd6f4"));
    palette.setColor(QPalette::Highlight,       QColor("#89b4fa"));
    palette.setColor(QPalette::HighlightedText, QColor("#1e1e2e"));
    palette.setColor(QPalette::ToolTipBase,     QColor("#313244"));
    palette.setColor(QPalette::ToolTipText,     QColor("#cdd6f4"));
    palette.setColor(QPalette::PlaceholderText, QColor("#6c7086"));
    a.setPalette(palette);

    // --- Connexion base de données ---
    if (!DatabaseManager::instance().connect("Source_Projet2A", "rayen", "12345678")) {
        QMessageBox::critical(nullptr, "Erreur de connexion",
                              "Impossible de se connecter à la base de données.");
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
