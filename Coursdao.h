#ifndef COURSDAO_H
#define COURSDAO_H

#include "Cours.h"
#include <QList>
#include <QString>
#include <QDate>

class CoursDAO
{
public:
    CoursDAO() = default;

    struct AlerteCours {
        int idCours;
        QString intitule;
        int placesRestantes;
        int capaciteMax;
        QString typeAlerte;
    };

    struct CoursClassement {
        int    idCours;
        QString intitule;
        int    nbInscrits;
        int    capaciteMax;
        double tauxRemplissage; // en %
    };

    bool ajouter(const Cours &c);
    bool modifier(const Cours &c);
    bool supprimer(int id);
    QList<Cours> afficherTous();
    Cours afficherParId(int id);
    QList<Cours> rechercherMulticritere(const QString &intitule = QString(),
                                        const QString &niveau = QString(),
                                        const QDate &dateDebutMin = QDate(),
                                        const QDate &dateDebutMax = QDate(),
                                        const QString &triColonne = QString("date_debut"),
                                        bool triAscendant = true);
    int nombreInscrits(int idCours);
    int placesDisponibles(int idCours);
    QList<AlerteCours> coursAvecAlerte(int seuilBientotComplet = 3,
                                       int seuilSousRempli = 3);
    QList<CoursClassement> coursLesPlusDemandes(int limite = 10);

    QString dernierErreur() const { return m_dernierErreur; }

private:
    QString m_dernierErreur;
};

#endif // COURSDAO_H
