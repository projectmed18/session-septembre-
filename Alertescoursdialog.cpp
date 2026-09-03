#include "alertescoursdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

AlertesCoursDialog::AlertesCoursDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Alertes - Remplissage des cours");
    resize(450, 400);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Cours nécessitant votre attention :"));

    m_liste = new QListWidget(this);
    layout->addWidget(m_liste);

    chargerAlertes();
}

void AlertesCoursDialog::chargerAlertes()
{
    m_liste->clear();
    const auto alertes = m_dao.coursAvecAlerte();

    if (alertes.isEmpty()) {
        m_liste->addItem("Aucune alerte : tous les cours ont un remplissage normal.");
        return;
    }

    for (const auto &a : alertes) {
        QString texte;
        QColor couleur;

        if (a.typeAlerte == "COMPLET") {
            texte = QString("🔴 %1 — COMPLET (%2/%3 places)")
                        .arg(a.intitule).arg(a.capaciteMax).arg(a.capaciteMax);
            couleur = QColor("#e57373");
        } else if (a.typeAlerte == "BIENTOT_COMPLET") {
            texte = QString("🟠 %1 — Bientôt complet (%2 place(s) restante(s) sur %3)")
                        .arg(a.intitule).arg(a.placesRestantes).arg(a.capaciteMax);
            couleur = QColor("#ffb74d");
        } else { // SOUS_REMPLI
            texte = QString("🔵 %1 — Sous-rempli (%2 place(s) restante(s) sur %3)")
                        .arg(a.intitule).arg(a.placesRestantes).arg(a.capaciteMax);
            couleur = QColor("#64b5f6");
        }

        auto *item = new QListWidgetItem(texte);
        item->setForeground(couleur);
        m_liste->addItem(item);
    }
}
